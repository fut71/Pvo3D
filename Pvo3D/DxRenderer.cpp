#include "DxRenderer.h"
#include <cassert>

DxRenderer::DxRenderer()
{
}

DxRenderer::~DxRenderer()
{
	Finalize();
}

bool DxRenderer::Initialize(HWND InWindowHandle, uint32_t InWidth, uint32_t InHeight)
{
	RendererWidth = InWidth;
	RendererHeight = InHeight;

	if(!CreateDeviceAndSwapChain(InWindowHandle))
	{
		return false;
	}

	if(!CreateRenderTargets())
	{
		return false;
	}

	// Viewport 설정
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;
	Viewport.Width = static_cast<float>(RendererWidth);
	Viewport.Height = static_cast<float>(RendererHeight);
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;

	return true;
}

bool DxRenderer::CreateDeviceAndSwapChain(HWND InWindowHandle)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = RendererWidth;
	swapChainDesc.BufferDesc.Height = RendererHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = InWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	UINT creationFlags = 0;

#if defined(_DEBUG)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};

	D3D_FEATURE_LEVEL createdFeatureLevel;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		featureLevels,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		SwapChain.GetAddressOf(),
		Device.GetAddressOf(),
		&createdFeatureLevel,
		DeviceContext.GetAddressOf());

	return SUCCEEDED(hr);
}

bool DxRenderer::CreateRenderTargets()
{
	// Back buffer 얻기
	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if(FAILED(hr))
	{
		return false;
	}

	// Render target view 생성
	hr = Device->CreateRenderTargetView(backBuffer.Get(), nullptr, RenderTargetView.GetAddressOf());
	if(FAILED(hr))
	{
		return false;
	}

	// Depth stencil texture 생성
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = RendererWidth;
	depthDesc.Height = RendererHeight;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ComPtr<ID3D11Texture2D> depthStencilBuffer;
	hr = Device->CreateTexture2D(&depthDesc, nullptr, depthStencilBuffer.GetAddressOf());
	if(FAILED(hr))
	{
		return false;
	}

	hr = Device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, DepthStencilView.GetAddressOf());
	if(FAILED(hr))
	{
		return false;
	}

	return true;
}

void DxRenderer::ReleaseRenderTargets()
{
	RenderTargetView.Reset();
	DepthStencilView.Reset();
}

void DxRenderer::BeginRender()
{
	assert(DeviceContext);
	assert(RenderTargetView);
	assert(DepthStencilView);

	const float clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), clearColor);
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
	DeviceContext->RSSetViewports(1, &Viewport);
}

void DxRenderer::Render()
{
	// 여기에 MeshObject, LightObject, CameraObject 등의 드로우 루프가 들어갈 예정
	// ex) for (auto& Mesh : MeshList) { Mesh->Draw(DeviceContext.Get()); }
}

void DxRenderer::EndRender()
{
	assert(SwapChain);
	SwapChain->Present(1, 0); // vsync on
}

void DxRenderer::Resize(uint32_t InWidth, uint32_t InHeight)
{
	RendererWidth = InWidth;
	RendererHeight = InHeight;

	ReleaseRenderTargets();

	HRESULT hr = SwapChain->ResizeBuffers(0, RendererWidth, RendererHeight, DXGI_FORMAT_UNKNOWN, 0);
	if(FAILED(hr))
	{
		return;
	}

	CreateRenderTargets();

	Viewport.Width = static_cast<float>(RendererWidth);
	Viewport.Height = static_cast<float>(RendererHeight);
}

bool DxRenderer::Finalize()
{
	ReleaseRenderTargets();
	DeviceContext.Reset();
	Device.Reset();
	SwapChain.Reset();
	return true;
}
