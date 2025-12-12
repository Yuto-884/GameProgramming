#include <Windows.h>

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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    // --------------------
    // Window
    // --------------------
    Window window;
    window.create(hInstance, 1280, 720, "Game");

    // --------------------
    // DXGI / Device
    // --------------------
    DXGI dxgi;
    dxgi.setDisplayAdapter();

    Device device;
    device.create(dxgi);

    // --------------------
    // Command Queue / Allocator / List
    // --------------------
    CommandQueue commandQueue;
    commandQueue.create(device);

    CommandAllocator commandAllocator;
    commandAllocator.create(device, D3D12_COMMAND_LIST_TYPE_DIRECT);

    CommandList commandList;
    commandList.create(device, commandAllocator);

    // --------------------
    // SwapChain
    // --------------------
    SwapChain swapChain;
    swapChain.create(dxgi, window, commandQueue);

    // --------------------
    // RTV Heap
    // --------------------
    DescriptorHeap rtvHeap;
    rtvHeap.create(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2);

    RenderTarget renderTarget;
    renderTarget.createBackBuffer(device, swapChain, rtvHeap);

    // --------------------
    // RootSignature / Shader / Pipeline
    // --------------------
    RootSignature rootSignature;
    rootSignature.create(device);

    Shader shader;
    shader.create(device);

    PiplineStateObject pipeline;
    pipeline.create(device, shader, rootSignature);

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
    vertexBuffer.create(device, triangle, 3, sizeof(Vertex));

    // --------------------
    // Main Loop
    // --------------------
    while (window.messageLoop())
    {
        commandAllocator.reset();
        commandList.reset(commandAllocator);

        // ★いま描画すべきバックバッファ
        UINT backIndex = swapChain.get()->GetCurrentBackBufferIndex();

        // ★PRESENT -> RENDER_TARGET
        {
            D3D12_RESOURCE_BARRIER barrier{};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = renderTarget.get(backIndex);
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            commandList.get()->ResourceBarrier(1, &barrier);
        }

        commandList.get()->SetGraphicsRootSignature(rootSignature.get());
        commandList.get()->SetPipelineState(pipeline.get());

        // ★Viewport / Scissor（これ無いと出ない事ある）
        {
            auto [w, h] = window.size();
            D3D12_VIEWPORT vp{};
            vp.Width = (float)w;
            vp.Height = (float)h;
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;

            D3D12_RECT sc{};
            sc.left = 0;
            sc.top = 0;
            sc.right = w;
            sc.bottom = h;

            commandList.get()->RSSetViewports(1, &vp);
            commandList.get()->RSSetScissorRects(1, &sc);
        }

        // ★RTV は backIndex に合わせる
        auto rtv = renderTarget.getDescriptorHandle(device, rtvHeap, backIndex);
        commandList.get()->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

        float clearColor[] = { 0.1f, 0.1f, 0.3f, 1.0f };
        commandList.get()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

        // ★Draw
        auto vbView = vertexBuffer.view();
        commandList.get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList.get()->IASetVertexBuffers(0, 1, &vbView);
        commandList.get()->DrawInstanced(3, 1, 0, 0);

        // ★RENDER_TARGET -> PRESENT
        {
            D3D12_RESOURCE_BARRIER barrier{};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = renderTarget.get(backIndex);
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            commandList.get()->ResourceBarrier(1, &barrier);
        }

        commandList.get()->Close();

        ID3D12CommandList* lists[] = { commandList.get() };
        commandQueue.get()->ExecuteCommandLists(1, lists);

        swapChain.get()->Present(1, 0);
    }

}
