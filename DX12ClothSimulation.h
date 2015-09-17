#pragma once

#include "DX12Framework.h"

using namespace DirectX;
using namespace Microsoft::WRL;

// Illustrate how to handle window resizing and fullscreen mode.
class DX12ClothSimulation : public DX12Framework
{
public:
	DX12ClothSimulation( UINT width, UINT height, std::wstring name );

protected:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnSizeChanged( UINT width, UINT height, bool minimized );
	virtual void OnDestroy();
	virtual bool OnEvent( MSG msg );

private:
	static const UINT FrameCount = 2;
	static const UINT TriangleWidth = 500;

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	// Pipeline objects.
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> m_commandAllocators[FrameCount];
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT m_rtvDescriptorSize;

	// App resources.
	ComPtr<ID3D12Resource> m_vertexBuffer;
	ComPtr<ID3D12Resource> m_vertexBufferUpload;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValues[FrameCount];

	// Track the state of the window.
	// If it's minimized the app may decide not to render frames.
	bool m_windowVisible;
	bool m_resizeResources;

	void LoadPipeline();
	void LoadAssets();
	void LoadSizeDependentResources();
	void PopulateCommandList();
	void WaitForGpu();
	void MoveToNextFrame();
};
