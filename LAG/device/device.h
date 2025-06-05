#pragma once
#include <d3d11.h>
#include <stdexcept>
#include <cassert>
class grcDeviced3d
{
private:
	static inline grcDeviced3d* sm_pInstance = nullptr;
public:
	static void Set(grcDeviced3d* pdev) { sm_pInstance = pdev; }
	static grcDeviced3d* Get() { return sm_pInstance; }
	grcDeviced3d(HWND window) {
		D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

		DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
		swapchaindesc.BufferDesc.Width = 0; // use window width
		swapchaindesc.BufferDesc.Height = 0; // use window height
		swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // can't specify SRGB framebuffer directly when using FLIP model swap effect. see lines 49, 66
		swapchaindesc.SampleDesc.Count = 1;
		swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchaindesc.BufferCount = 2;
		swapchaindesc.OutputWindow = window;
		swapchaindesc.Windowed = TRUE;
		swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;



		HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG, featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION, &swapchaindesc, &swapChain, &outDevice, nullptr, &context); // D3D11_CREATE_DEVICE_DEBUG is optional, but provides useful d3d11 debug output
		if (hr != S_OK) {
			throw std::runtime_error("Failed to init D3D11 man you probably hurting rn.");
		}
		ID3D11Texture2D* frameBuf;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&frameBuf);
		assert(SUCCEEDED(hr));
		hr = outDevice->CreateRenderTargetView(frameBuf, nullptr, &backBuffer);
		assert(SUCCEEDED(hr));

		frameBuf->Release();
	}
	ID3D11DeviceContext* context = nullptr;
	ID3D11Device* outDevice = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11RenderTargetView* backBuffer = nullptr;
};