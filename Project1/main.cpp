#include <Windows.h>
#include <d3d12.h>

#include "window.h"
#include "DXGI.h"
#include "device.h"
#include "command_queue.h"
#include "command_allocator.h"
#include "command_list.h"
#include "swap_chain.h"
#include "descriptor_heap.h"
#include "render_target.h"
#include "root_signature.h"
#include "shader.h"
#include "pipline_state_object.h"
#include "vertex_buffer.h"

// ちょい便利：失敗したら即終了
static void Die(const char* msg)
{
    MessageBoxA(nullptr, msg, "DX12 Error", MB_OK | MB_ICONERROR);
    ExitProcess(1);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    // --------------------
    // Window
    // --------------------
    Window window;
    if (FAILED(window.create(hInstance, 1280, 720, "Game"))) {
        Die("Window::create failed");
    }

    // --------------------
    // DXGI / Device
    // --------------------
    DXGI dxgi;
    dxgi.setDisplayAdapter();

    Device device;
    if (!device.create(dxgi)) {
        Die("Device::create failed");
    }

    // --------------------
    // Command Queue / Allocator / List
    // --------------------
    CommandQueue commandQueue;
    if (!commandQueue.create(device)) {
        Die("CommandQueue::create failed");
    }

    CommandAllocator commandAllocator;
    if (!commandAllocator.create(device, D3D12_COMMAND_LIST_TYPE_DIRECT)) {
        Die("CommandAllocator::create failed");
    }

    CommandList commandList;
    if (!commandList.create(device, commandAllocator)) {
        Die("CommandList::create failed");
    }

    // --------------------
    // SwapChain
    // --------------------
    SwapChain swapChain;
    if (!swapChain.create(dxgi, window, commandQueue)) {
        Die("SwapChain::create failed");
    }

    // --------------------
    // RTV Heap / BackBuffer
    // --------------------
    DescriptorHeap rtvHeap;
    if (!rtvHeap.create(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2)) {
        Die("DescriptorHeap(RTV)::create failed");
    }

    RenderTarget renderTarget;
    if (!renderTarget.createBackBuffer(device, swapChain, rtvHeap)) {
        Die("RenderTarget::createBackBuffer failed");
    }

    // --------------------
    // RootSignature / Shader / Pipeline
    // --------------------
    RootSignature rootSignature;
    if (!rootSignature.create(device)) {
        Die("RootSignature::create failed");
    }

    Shader shader;
    if (!shader.create(device)) {
        Die("Shader::create failed (shader.hlsl path?)");
    }

    PiplineStateObject pipeline;
    if (!pipeline.create(device, shader, rootSignature)) {
        Die("PiplineStateObject::create failed");
    }

    // --------------------
    // Vertex Buffer
    // --------------------
    struct Vertex {
        float pos[3];
        float color[4];
    };

    Vertex triangle[3] = {
        {{ 0.0f,  0.5f, 0.0f }, {1,0,0,1}},
        {{ 0.5f, -0.5f, 0.0f }, {0,1,0,1}},
        {{-0.5f, -0.5f, 0.0f }, {0,0,1,1}},
    };

    VertexBuffer vertexBuffer;
    if (!vertexBuffer.create(device, triangle, 3, sizeof(Vertex))) {
        Die("VertexBuffer::create failed");
    }

    // --------------------
    // Fence (GPU同期) ★これ無いと落ちやすい
    // --------------------
    ID3D12Fence* fence = nullptr;
    UINT64 fenceValue = 0;
    HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) Die("CreateEvent failed");

    HRESULT hr = device.get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    if (FAILED(hr) || !fence) Die("CreateFence failed");

    // --------------------
    // Main Loop
    // --------------------
    while (window.messageLoop())
    {
        // GPUが前フレーム終わるまで待つ
        if (fence->GetCompletedValue() < fenceValue) {
            fence->SetEventOnCompletion(fenceValue, fenceEvent);
            WaitForSingleObject(fenceEvent, INFINITE);
        }

        const UINT backIndex = swapChain.get()->GetCurrentBackBufferIndex();
        ID3D12Resource* backBuffer = renderTarget.get(backIndex);
        auto rtv = renderTarget.getDescriptorHandle(device, rtvHeap, backIndex);

        commandAllocator.reset();
        commandList.reset(commandAllocator);

        // Present -> RenderTarget
        D3D12_RESOURCE_BARRIER toRT{};
        toRT.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        toRT.Transition.pResource = backBuffer;
        toRT.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        toRT.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        toRT.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList.get()->ResourceBarrier(1, &toRT);

        commandList.get()->SetGraphicsRootSignature(rootSignature.get());
        commandList.get()->SetPipelineState(pipeline.get());

        // viewport / scissor
        auto [w, h] = window.size();
        D3D12_VIEWPORT viewport{};
        viewport.Width = (float)w;
        viewport.Height = (float)h;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        D3D12_RECT scissor{};
        scissor.right = w;
        scissor.bottom = h;

        commandList.get()->RSSetViewports(1, &viewport);
        commandList.get()->RSSetScissorRects(1, &scissor);

        commandList.get()->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

        float clearColor[] = { 0.1f, 0.1f, 0.3f, 1.0f };
        commandList.get()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

        auto vbView = vertexBuffer.view();
        commandList.get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList.get()->IASetVertexBuffers(0, 1, &vbView);
        commandList.get()->DrawInstanced(3, 1, 0, 0);

        // RenderTarget -> Present
        D3D12_RESOURCE_BARRIER toPresent = toRT;
        toPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        toPresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        commandList.get()->ResourceBarrier(1, &toPresent);

        commandList.get()->Close();

        ID3D12CommandList* lists[] = { commandList.get() };
        commandQueue.get()->ExecuteCommandLists(1, lists);

        swapChain.get()->Present(1, 0);

        // GPUに「ここまで終わったら値を進めろ」って指示
        fenceValue++;
        commandQueue.get()->Signal(fence, fenceValue);
    }

    // 後始末
    if (fence) fence->Release();
    if (fenceEvent) CloseHandle(fenceEvent);

    return 0;
}

