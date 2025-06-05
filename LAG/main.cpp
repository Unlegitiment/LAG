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

#pragma comment( lib, "user32" )          // link against the win32 library
#pragma comment( lib, "d3d11.lib" )       // direct3D library
#pragma comment( lib, "dxgi.lib" )        // directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);





#define LAG_MAX_PATH 512
std::vector<wchar_t> ConvertString([[maybe_unused]] const char* string, [[maybe_unused]] size_t size) { 
	//std::vector<wchar_t> charw;
	//charw.resize(size + 1);
	//mbstowcs_s(charw.data(), string, LAG_MAX_PATH);
	return {};
}
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
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"
int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance, [[maybe_unused]]HINSTANCE hPrevInstance, [[maybe_unused]]PWSTR pCmdLine, [[maybe_unused]]int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"grcWindow";
	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);
	HWND hwnd = CreateWindowEx( 0, CLASS_NAME, L"Grand Theft Auto VI", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) return 0;
	ShowWindow(hwnd, nCmdShow);
	// Run the message loop.
	grcDeviced3d device = grcDeviced3d(hwnd);
	device.Set(&device);
	grcModel model = grcModel();
	grcInputLayout iaLayout = grcInputLayout(model.GetShaderGroup()->GetVertexShader()); // move this into model since it relies on info of model
	float fClearCol[4] = { 0 / 255.0f, 0 / 255.0f, 32 / 255.0f, 255 / 255.0f }; 
	MSG msg = { };
	bool m_bShouldClose = false;
	RECT winRect;
	GetClientRect(hwnd, &winRect);
	grcStateBlock::sm_pRect = &winRect;
	grcStateBlock::Init();
	CTimer timer;
	ID3D11ShaderResourceView* char_social_club_srv = nullptr;
	ID3D11Texture2D* Texture;
	int x = 0, y = 0, comp = 0;
	stbi_uc* uc = stbi_load("W:\\GTAV Scripts\\LAG\\LAG\\Assets\\char_social_club.jpg", &x, &y, &comp, 4);
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
	HRESULT hr = device.outDevice->CreateTexture2D(&DESC, &init, &Texture);
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
	while (!m_bShouldClose)
	{
		float fDelta = timer.GetDelta();
		m_bShouldClose = HandleWindowMessages(&msg);
		grcStateBlock::BeginFrame();
		GetClientRect(hwnd, &winRect);
		device.context->ClearRenderTargetView(device.backBuffer, fClearCol); // As it is a apart of the stateblock what we need to do is Clear when we do a NEW FRAME (not begin a frame because we have to setup like topology inside of that.
		device.context->OMSetRenderTargets(1, &device.backBuffer, nullptr); //I need someway to have dynamic render targets or rendertarget creation dynamically. Handled internally.
		iaLayout.Bind(); 
		//We could abstract this part out right now and setup so that we have some sort of intermingling of shader sampler shit. hmm.
		device.context->PSSetShaderResources(0, 1, &char_social_club_srv);
		device.context->PSSetSamplers(0, 1, &samplerState);
		model.Draw(fDelta);
		grcStateBlock::EndFrame(); // clear and await more instruction.
		device.swapChain->Present(1, 0);
	}
	grcStateBlock::DestroyStateBlock();
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}