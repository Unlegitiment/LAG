#pragma once
/// <summary>
/// GRAPHICS DEVICE
/// </summary>
#define LAG_D3D11
#ifdef LAG_D3D11
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct DXGI_SWAP_CHAIN_DESC;
struct ID3D11Texture2D;
#endif
namespace lage {
#ifdef LAG_PS4
typedef void* DGDevice;
typedef void* DGContext;
typedef void* DGSwapChain;
typedef void* DGRenderTarget;
#elif defined(LAG_D3D11)
typedef ID3D11Device DGDevice;
typedef ID3D11DeviceContext DGContext;
typedef IDXGISwapChain DGSwapChain;
typedef ID3D11RenderTargetView DGRenderTarget;
typedef DXGI_SWAP_CHAIN_DESC DGSwapChainDescriptor;
typedef ID3D11Texture2D DGTexture2D;
#endif // ifdef LAG_D3D11

class GDevice {
public:
	void Init(void* window);
	void Shutdown();
	DGDevice* GetDevice();
	DGContext* GetContext();
	DGSwapChain* GetSwapChain();
	DGRenderTarget* GetRenderTarget();
	DGRenderTarget** GetRenderTargets();
	DGSwapChainDescriptor* GetSwapChainDescriptor();
	DGTexture2D* GetFrameBuffer();
private:
	void SetupFrameBuffer();
	void SetupRenderTarget();
	//WRAP THESE OBJECTS BELOW IN THE COMING DAYS ALLOWING FOR THE FLEXIBLE USE OF THE OBJECT. SO THAT WE ARE NOT AS CLOSELY TIED TO D3D11
	DGDevice* m_pDevice = nullptr; // 
	DGContext* m_pContextHandle = nullptr;
	DGSwapChain* m_pSwapChain = nullptr;
	DGRenderTarget* m_pRenderTarget = nullptr;
	//End of Wrapper Objects. 
	DGSwapChainDescriptor* m_pSwapChainDescriptor = nullptr;
	DGTexture2D* m_pFramebuffer = nullptr;
};
}