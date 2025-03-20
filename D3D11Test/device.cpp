#include "device.h"
#include <d3d11.h>
#include <cassert>
void lage::GDevice::Init(void* window) {
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
    this->m_pSwapChainDescriptor = (DGSwapChainDescriptor*)malloc(sizeof DXGI_SWAP_CHAIN_DESC);
    assert(this->m_pSwapChainDescriptor != NULL);
    memcpy(this->m_pSwapChainDescriptor, &swapchaindesc, sizeof(DXGI_SWAP_CHAIN_DESC)); //  I hate memcpy in this instance but I also hate porting so meh.
    this->m_pSwapChain->GetDesc(this->m_pSwapChainDescriptor);
    SetupFrameBuffer();
    SetupRenderTarget();
}

void lage::GDevice::Shutdown() {
    this->m_pDevice->Release();
    this->m_pContextHandle->Release();
    this->m_pRenderTarget->Release();
    this->m_pSwapChain->Release();
    free(this->m_pSwapChainDescriptor);
}

lage::DGDevice* lage::GDevice::GetDevice() {
    return this->m_pDevice;
}

lage::DGContext* lage::GDevice::GetContext() {
    return this->m_pContextHandle;
}

lage::DGSwapChain* lage::GDevice::GetSwapChain() {
    return this->m_pSwapChain;
}

lage::DGRenderTarget* lage::GDevice::GetRenderTarget() {
    return this->m_pRenderTarget;
}

lage::DGRenderTarget** lage::GDevice::GetRenderTargets() {
    return &this->m_pRenderTarget;
}

lage::DGSwapChainDescriptor* lage::GDevice::GetSwapChainDescriptor() {
    return this->m_pSwapChainDescriptor;
}

lage::DGTexture2D* lage::GDevice::GetFrameBuffer() {
    return this->m_pFramebuffer;
}

void lage::GDevice::SetupFrameBuffer() {
    DGTexture2D* frameBuffer;
    HRESULT hr;
    hr = GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&frameBuffer);
    this->m_pFramebuffer = frameBuffer;
}

void lage::GDevice::SetupRenderTarget() {
    HRESULT hr;
    D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVDesc = {};
    framebufferRTVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    framebufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = GetDevice()->CreateRenderTargetView(this->m_pFramebuffer, &framebufferRTVDesc, &this->m_pRenderTarget);
}
