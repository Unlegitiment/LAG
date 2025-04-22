#include "Graphics.h"
#include "LAGGraphics\renderer\device.h"
#include <d3d11.h>
#include "cassert"
void CGraphics::Init(unsigned int mode)
{
    ID3D11Device* dev;
    IDXGISwapChain* swpchn;
    DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
    swapchaindesc.BufferDesc.Width = 0; // use window width
    swapchaindesc.BufferDesc.Height = 0; // use window height
    swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // can't specify SRGB framebuffer directly when using FLIP model swap effect. see lines 49, 66
    swapchaindesc.SampleDesc.Count = 1;
    swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchaindesc.BufferCount = 2;
    swapchaindesc.OutputWindow = (HWND)window;
    swapchaindesc.Windowed = TRUE;
    swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    D3D_FEATURE_LEVEL featureLevel;
    UINT flgs = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined ( DEBUG ) || defined ( _DEBUG )
    flgs |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    //Swapchain Creation + DEVICE Creation
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flgs, NULL, 0, D3D11_SDK_VERSION, &swapchaindesc, &this->m_pSwapChain, &this->m_pDevice, &featureLevel, &this->m_pContextHandle);
    assert(S_OK == hr && m_pSwapChain && this->m_pContextHandle && this->m_pDevice);
    SetupFrameBuffer();
    SetupRenderTarget();
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, , , D3D11_SDK_VERSION, , &swpchn, &dev);
	if (sm_API == D3D11) {
		sm_pDevice = new grcD3D11Device(dev);
	}
}

void CGraphics::Shutdown()
{
}
