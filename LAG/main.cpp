#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>       // D3D interface
#include <dxgi.h>        // DirectX driver interface
#include <d3dcompiler.h> // shader compiler
#include <stdexcept>
#include <cassert>
#include <vector>
#include <cstdlib>
#include "Shader\Shadergroup.h"
#include "Buffer\Buffer.h"
#include <model\grcmodel.h>
#include <InputAssembly\InputAssembly.h>
#include <State\Stateblock.h>
#include <__msvc_chrono.hpp>
#include <assimp/Importer.hpp>
#include <DirectXMath.h>
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"

#pragma comment( lib, "user32" )          // link against the win32 library
#pragma comment( lib, "d3d11.lib" )       // direct3D library
#pragma comment( lib, "dxgi.lib" )        // directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*
	Research Notes on Concept of RenderPass:
		- Effectively In Vulkan its a manager of subpasses 
		- Subpasses are basically dependancy related objects that tell the driver that we intend to output to resource a or we are using resource a in subpass b. 
		- Renderpasses do not own the objects they relay on they simply manage them so the driver can handle them (this is Vulkan with CMD Buffers and a bunch of shi.)
		- A renderpass in the case of D3D11 can just be something that manages the output merger. Unlike grcStateBlock which controls Rasterizer States, and Topology, this class basically manages OutputMerger objects.
		- Things like Depth (+ its state), RenderTargets, etc all run through a renderpass. 
		- We should also specify that a RenderSegment is different from a RenderPass cause in my head a RenderSegment is an entire like segment of a Draw they also own RenderPasses, the objects they intend to draw etc. basically an entire pass.
*/
class CRenderPass {
public:
	void InitClass() { // Setup for Depth State so we own the object. Ideally we'd also do some *magic* with other info. but this right now just relies on RenderTarget stuff
		HRESULT hr = S_OK;
		D3D11_DEPTH_STENCIL_DESC dsdDESC = {};
		dsdDESC.DepthEnable = true;
		dsdDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsdDESC.DepthFunc = D3D11_COMPARISON_LESS;
		ID3D11DepthStencilState* pDSState = nullptr;
		hr = grcDeviced3d::Get()->outDevice->CreateDepthStencilState(&dsdDESC, &pDSState);
		this->m_pDepthStencilState = pDSState;
		assert(SUCCEEDED(hr) && "CreateDepthStencilState Failed");
		D3D11_TEXTURE2D_DESC descDepth = {};
		DXGI_SWAP_CHAIN_DESC swpdesc = {};
		grcDeviced3d::Get()->swapChain->GetDesc(&swpdesc);
		descDepth.Width = swpdesc.BufferDesc.Width;
		descDepth.Height = swpdesc.BufferDesc.Height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		ID3D11Texture2D* pDSTexture = nullptr;
		hr = grcDeviced3d::Get()->outDevice->CreateTexture2D(&descDepth, nullptr, &pDSTexture);
		assert(SUCCEEDED(hr) && "CreateTexture2D Failed DSV Setup");
		ID3D11DepthStencilView* pDSV = nullptr;
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0u;
		hr = grcDeviced3d::Get()->outDevice->CreateDepthStencilView(pDSTexture, &descDSV, &pDSV); // dawh shit mf!
		assert(SUCCEEDED(hr) && "CreateDepthStencilView Failed!");
		dsdDESC.StencilEnable = false;
		m_pDepthStencilView = pDSV;
	}
	// Ownership transfer: ID3D11RenderTargetView* now owned by CRenderPass::m_RenderTargets
	void PushRenderTarget(ID3D11RenderTargetView* rendertarget) {
		this->m_RenderTargets.push_back(rendertarget);
	}
	void BeginFrame() {
		assert(!m_RenderTargets.empty() && "There is no RenderTargets created! How you gonna render to nothing lol!");
		std::for_each(m_RenderTargets.begin(), m_RenderTargets.end(), [&](ID3D11RenderTargetView* render)->void {
				grcDeviced3d::Get()->context->ClearRenderTargetView(render, m_ClearColor); // As it is a apart of the stateblock what we need to do is Clear when we do a NEW FRAME (not begin a frame because we have to setup like topology inside of that.
		});
		grcDeviced3d::Get()->context->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u);
		grcDeviced3d::Get()->context->OMSetRenderTargets((UINT)m_RenderTargets.size(), m_RenderTargets.data(), m_pDepthStencilView); // I guess this does need to be here?
		grcDeviced3d::Get()->context->OMSetDepthStencilState(m_pDepthStencilState, 1u);
	}
	ID3D11RenderTargetView* GetTarget(int _index) {
		assert(_index <= m_RenderTargets.size() && __FUNCTION__" supplied argument over bounds");
		return m_RenderTargets[_index];
	}
	ID3D11DepthStencilView* GetDepthStencil() { return this->m_pDepthStencilView; }
	ID3D11DepthStencilState* GetDepthState() { return this->m_pDepthStencilState; }
	void EndFrame() {

	}
	void DestroyClass() {
		std::for_each(m_RenderTargets.begin(), m_RenderTargets.end(), [&](ID3D11RenderTargetView* render) -> void {
			render->Release();
		});
		m_pDepthStencilView->Release();
		m_pDepthStencilState->Release();
	}
	
private:
	FLOAT m_ClearColor[4] = { 0 / 255.0f, 0 / 255.0f, 32 / 255.0f, 255 / 255.0f };
	std::vector<ID3D11RenderTargetView*> m_RenderTargets;
	ID3D11DepthStencilState* m_pDepthStencilState = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
};

#define LAG_MAX_PATH 512
std::vector<wchar_t> ConvertString([[maybe_unused]] const char* string, [[maybe_unused]] size_t size) { 
	//std::vector<wchar_t> charw;
	//charw.resize(size + 1);
	//mbstowcs_s(charw.data(), string, LAG_MAX_PATH);
	return {};
}
#include "thirdparty\imgui.h"
#include "thirdparty\imgui_impl_win32.h"
#include "thirdparty\imgui_impl_dx11.h"
/*
	Returns whether WM_QUIT is triggered!;
*/
bool HandleWindowMessages(MSG* msg) {
	while (PeekMessage(msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(msg);
		DispatchMessage(msg);
		if (msg->message == WM_QUIT) {
			return true;
		}
	}
	return false;
}
class CTimer {
public:
	CTimer() {
		m_Last = std::chrono::high_resolution_clock::now();
	}
	float GetDelta() {
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> delta = now - m_Last;
		m_Last = now;
		return delta.count(); // in seconds
	}
private:
	std::chrono::high_resolution_clock::time_point	m_Last;
};


void SetupDebug(HWND hwnd) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(grcDeviced3d::Get()->outDevice, grcDeviced3d::Get()->context);
}

void* CreateWinTest([[maybe_unused]] HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, [[maybe_unused]] PWSTR pCmdLine, [[maybe_unused]] int nCmdShow) {
	const wchar_t CLASS_NAME[] = L"grcWindow";
	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);
	HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Grand Theft Auto VI", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) return 0;
	ShowWindow(hwnd, nCmdShow);
	return hwnd;
}
class CWindow {
public:
	static constexpr const wchar_t* WNDCLASSNAME = L"lagWindow";
	static void Init(HINSTANCE hinstnace) {
		wc.lpfnWndProc = WindowProc; 
		wc.hInstance = hinstnace;
		wc.lpszClassName = WNDCLASSNAME;
		RegisterClass(&wc);
		HWND hwnd = CreateWindowEx(0, WNDCLASSNAME, L"Grand Theft Auto VI", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, NULL, NULL, wc.hInstance, NULL);
		if (hwnd == NULL) throw std::runtime_error("Problem with Window Creation!");
		ShowWindow(hwnd, 1); // nCmdShow check.
		WINDOW_HANDLE = hwnd;
	}
	static void* GetHandle() {
		return WINDOW_HANDLE;
	}
	static void Destroy() {
		CloseWindow((HWND)WINDOW_HANDLE);
	}
private:
	static inline WNDCLASS wc = { };
	static inline void* WINDOW_HANDLE = nullptr;
};
class CRenderSegment {
public:
	virtual void BuildDrawList() { // builds a base list of drawables.
		throw std::exception("Unimplemented right now!");
	}
private:

};
class CRenderSegement_SceneEntity : public CRenderSegment {
	void BuildDrawList() {
		// fetch list of entities from CScene here get their drawables -> call draw. just also gotta setup shader context stuff which should be handled by some intermitten stuff. probably like a drawhandler or something. just need to store a location of a texture though for a drawable which could go inside of the drawable unless a drawable should just a model stuff? not sure rn kinda tired. 
		// also need something like LAGGLOBALS as a variable register. come back tmr for more stuff. 
	}
};
class CRenderSegmentMgr {
public:

private:
	std::vector<CRenderSegment*> m_RenderSegments;
};
class CRenderSetup {
public:
	static void InitRenderSegments() {

	}
private:
	static inline CRenderSegment* sm_pRenderSegment = nullptr;
};
// Depends on CWINDOW
class CRenderer { 
public:
	static void Init() {
		device = new grcDeviced3d((HWND)CWindow::GetHandle()); 
		device->Set(device);
	}
	static void Destroy() {
		delete device;
		device = nullptr;
	}
	static grcDeviced3d* GetDevice() { return device; }
private:
	static inline grcDeviced3d* device = nullptr;
};

int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance, [[maybe_unused]]HINSTANCE hPrevInstance, [[maybe_unused]]PWSTR pCmdLine, [[maybe_unused]]int nCmdShow)
{
	// Register the window class.
	CWindow::Init(hInstance);
	CRenderer::Init();
	// Run the message loop.
	grcDeviced3d* raw_Dev = CRenderer::GetDevice();
	if (!raw_Dev) return 1; // failed
	grcDeviced3d device = *raw_Dev; // Why not just make this NOT like this you know?
	//device.Set(&device);
	grcModel model = grcModel();
	grcInputLayout iaLayout = grcInputLayout(model.GetShaderGroup()->GetVertexShader()); // move this into model since it relies on info of model
	//float fClearCol[4] = { 0 / 255.0f, 0 / 255.0f, 32 / 255.0f, 255 / 255.0f }; 
	MSG msg = { };
	bool m_bShouldClose = false;
	RECT winRect;
	GetClientRect((HWND)CWindow::GetHandle() , &winRect);
	grcStateBlock::sm_pRect = &winRect;
	grcStateBlock::Init();
	CTimer timer;
	HRESULT hr = S_OK;
	CRenderPass renderPass = CRenderPass();
	renderPass.InitClass();
	ID3D11ShaderResourceView* char_social_club_srv = nullptr;
	ID3D11Texture2D* Texture;
	int x = 0, y = 0, comp = 0;
	stbi_uc* uc = stbi_load("W:\\GTAV Scripts\\LAG\\LAG\\Assets\\char_social_club.jpg", &x, &y, &comp, 4); // oopse
	assert(uc != nullptr && "STBI failed to load image. Check image's path.");
	D3D11_TEXTURE2D_DESC DESC = {};
	DESC.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DESC.Width = x;
	DESC.Height = y;
	DESC.Usage = D3D11_USAGE_DEFAULT;
	DESC.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	DESC.MipLevels = 1;
	DESC.ArraySize = 1;
	DESC.SampleDesc.Count = 1;
	D3D11_SUBRESOURCE_DATA init = {};
	init.pSysMem = uc;
	init.SysMemPitch = x * 4; // why 4x?
	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
	srvdesc.Format = DESC.Format;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Texture2D.MipLevels = 1;
	hr = device.outDevice->CreateTexture2D(&DESC, &init, &Texture);
	assert(SUCCEEDED(hr) && "Failed to do CreateTexture2D");
	hr = device.outDevice->CreateShaderResourceView(Texture, &srvdesc, &char_social_club_srv);
	assert(SUCCEEDED(hr) && "Failed to do CreateShaderResourceView");
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	ID3D11SamplerState* samplerState = nullptr;
	hr = device.outDevice->CreateSamplerState(&sampDesc, &samplerState);
	assert(SUCCEEDED(hr) && "Failed to do CreateSamplerState");
	stbi_image_free(uc);
	//ImGUI setup
	SetupDebug((HWND)CWindow::GetHandle());
	//This is typically setup on per-pass shit. Its like what we render to so we don't need to like actually put it inside of a whatever blah blah blah
	renderPass.PushRenderTarget(grcDeviced3d::Get()->backBuffer);
	while (!m_bShouldClose)
	{

		//float fDelta = timer.GetDelta();
		m_bShouldClose = HandleWindowMessages(&msg);
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
			ImGui::Begin("Cam");
			ImGui::SliderFloat("X", &model.camera->POSITION.m128_f32[0], -360, 360);
			ImGui::SliderFloat("Y", &model.camera->POSITION.m128_f32[1], -360, 360);
			ImGui::SliderFloat("Z", &model.camera->POSITION.m128_f32[2], -360, 360);
			ImGui::SliderFloat("Yaw", &model.camera->Yaw, -360, 360);
			ImGui::SliderFloat("Pitch", &model.camera->Pitch, -360, 360);
			ImGui::SliderFloat("Zoom", &model.camera->Zoom, 0, 100);
			if (ImGui::Button("Reset")) {
				model.camera->POSITION = { 0.0,0.0,0.0,0.0f }; // make vector!
				model.camera->FRONT = { 0.0,0.0,-1.0f };
				model.camera->UP = { 0.0,1.0,0.0 };
				model.camera->RIGHT;
				model.camera->WORLDUP = { 0.0,1.0,0.0 };
				model.camera->Yaw = -90.0;
				model.camera->Pitch = 0.0f;
				model.camera->Zoom = 45.f;
			}
		ImGui::End();
		grcStateBlock::BeginFrame();
		GetClientRect((HWND)CWindow::GetHandle(), &winRect);
		renderPass.BeginFrame();
		iaLayout.Bind(); 
		//We could abstract this part out right now and setup so that we have some sort of intermingling of shader sampler shit. hmm.
		device.context->PSSetShaderResources(0, 1, &char_social_club_srv);
		device.context->PSSetSamplers(0, 1, &samplerState);
		model.Draw(0.0f, 0.0f, -5);
		model.Draw(1, 1, 5.f); // double draw sim shit.
		renderPass.EndFrame(); // Does nothing but makes sense here.
		grcStateBlock::EndFrame(); // clear and await more instruction.
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		device.swapChain->Present(1, 0);
	}
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	grcStateBlock::DestroyStateBlock();
	renderPass.DestroyClass();
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return true;
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}