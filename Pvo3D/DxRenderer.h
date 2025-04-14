#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <wrl/client.h>
#include <stdint.h>
using namespace Microsoft::WRL;

class DxRenderer
{
public:
	DxRenderer();
	~DxRenderer();

	bool Initialize(HWND InWindowHandle, uint32_t InWidth, uint32_t InHeight);
	void BeginRender();
	void Render();
	void EndRender();
	void Resize(uint32_t InWidth, uint32_t InHeight);
	bool Finalize();

	ID3D11Device* GetDevice() const 
	{
		return Device.Get();
	}
	ID3D11DeviceContext* GetContext() const 
	{
		return DeviceContext.Get();
	}

private:
	bool CreateDeviceAndSwapChain(HWND InWindowHandle);
	bool CreateRenderTargets();
	void ReleaseRenderTargets();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;

	D3D11_VIEWPORT Viewport = {};

	uint32_t RendererWidth = 0;
	uint32_t RendererHeight = 0;
};
