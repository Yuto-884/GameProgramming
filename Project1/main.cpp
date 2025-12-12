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

        commandList.get()->SetGraphicsRootSignature(rootSignature.get());
        commandList.get()->SetPipelineState(pipeline.get());

        auto rtv = renderTarget.getDescriptorHandle(device, rtvHeap, 0);
        commandList.get()->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

        float clearColor[] = { 0.1f, 0.1f, 0.3f, 1.0f };
        commandList.get()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

        auto vbView = vertexBuffer.view();
        commandList.get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList.get()->IASetVertexBuffers(0, 1, &vbView);
        commandList.get()->DrawInstanced(3, 1, 0, 0);

        commandList.get()->Close();

        ID3D12CommandList* lists[] = { commandList.get() };
        commandQueue.get()->ExecuteCommandLists(1, lists);

        swapChain.get()->Present(1, 0);
    }

    return 0;
}
