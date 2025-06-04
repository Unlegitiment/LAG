#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>       // D3D interface
#include <dxgi.h>        // DirectX driver interface
#include <d3dcompiler.h> // shader compiler
#include <stdexcept>
#include <cassert>
#include <vector>
#include <cstdlib>

#pragma comment( lib, "user32" )          // link against the win32 library
#pragma comment( lib, "d3d11.lib" )       // direct3D library
#pragma comment( lib, "dxgi.lib" )        // directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


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
typedef wchar_t wchar;
enum eShaderType {
	INVALID,
	VERTEX,
	PIXEL
};
class grcShaderBase {
public:
	grcShaderBase(eShaderType shader) : m_ShaderType(shader){

	}
	void CompileShaderAtPath(const wchar* path, const char* entry, const char* version) { 
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
		flags |= D3DCOMPILE_DEBUG; // disable 
		ID3DBlob* outblob, * errorBlob;
		HRESULT hr = D3DCompileFromFile(path, nullptr, nullptr, entry, version, 0, 0, &outblob, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer()); // This is actually kinda cool lowkey. 
				errorBlob->Release();
			}
			if (outblob) outblob->Release();
			assert(false && "We failed to generate a blob!");
		}
		m_pBlob = outblob;
	}
	virtual void Bind() = 0; // Dependant on a grcDevice instance existing! Which if you don't *WHAT ARE YOU DOING LMAO!*
	virtual void* GetNativeHandle() = 0; // So we can access the native handle at a higher level I.E ID3D11VertexShader, VkShaderModule, OpenGLProgram, 
	virtual ~grcShaderBase() {
		m_pBlob->Release();
	}
	eShaderType GetShaderType() { return m_ShaderType; }
	ID3DBlob* GetBlob() { return this->m_pBlob; } // No setting thats internal!
protected:
	eShaderType m_ShaderType = INVALID; // default invalid shader type.
	ID3DBlob* m_pBlob = nullptr;
};
#define LAG_MAX_PATH 512
std::vector<wchar_t> ConvertString([[maybe_unused]] const char* string, [[maybe_unused]] size_t size) { 
	//std::vector<wchar_t> charw;
	//charw.resize(size + 1);
	//mbstowcs_s(charw.data(), string, LAG_MAX_PATH);
	return {};
}
class grcShaderVertex : public grcShaderBase{
public:
	grcShaderVertex(const wchar* path, const char* entry) : grcShaderBase(VERTEX){ 
		int IsShaderNotCompiled = 1;
		if (IsShaderNotCompiled) {
			CompileShaderAtPath(path, entry, "vs_5_0"); // ideally pre-compile shaders
		}
		if (m_pBlob) { // shader is compiled
			grcDeviced3d::Get()->outDevice->CreateVertexShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &Shader);
		}
	}
	void Bind() override
	{
		grcDeviced3d::Get()->context->VSSetShader(Shader, nullptr, 0);
	}
	void* GetNativeHandle() override
	{
		return (void*)Shader;
	}
	~grcShaderVertex() {
		Shader->Release();
	}
private:
	ID3D11VertexShader* Shader;
};

// LAG_MAX_PATH ATTR USED ONLY IN FILE HANDLING.
class grcShaderPixel : public grcShaderBase {
public:
	grcShaderPixel(const wchar* path, const char* entry) : grcShaderBase(PIXEL) { // declare fiStream for reading file patterns. Since all we need to do is check ext.
		int IsShaderNotCompiled = 1;
		if (IsShaderNotCompiled) {
			CompileShaderAtPath(path, entry, "ps_5_0"); // this pops pops it inside of protected ID3DBlob. do not store result
		}
		if (m_pBlob) { // shader is compiled
			grcDeviced3d::Get()->outDevice->CreatePixelShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &Shader);
		}
	}
	void Bind() override
	{
		assert(Shader != nullptr && "[Pixel Shader] Shader not found!");
		grcDeviced3d::Get()->context->PSSetShader(Shader, nullptr, 0);
	}
	void* GetNativeHandle() override
	{
		return (void*)Shader;
	}
	~grcShaderPixel() {
		Shader->Release();
	}
private:
	ID3D11PixelShader* Shader = nullptr; 
};
class grcShaderGroup {
public:
	grcShaderGroup(const wchar* path) {
		VertexShader = new grcShaderVertex(path, "vs_main");
		PixelShader = new grcShaderPixel(path, "ps_main");
	}
	grcShaderVertex* GetVertexShader() { return this->VertexShader; }
	grcShaderPixel* GetPixelShader() { return this->PixelShader; }
	void Bind() {
		//assert(grcDeviced3d::Get() == nullptr && "Device does not exist how are you binding!");
		//assert(grcDeviced3d::Get()->context == nullptr && "Context does not exist how are you binding!");
		VertexShader->Bind();
		PixelShader->Bind();
	}
	~grcShaderGroup() {
		delete VertexShader;
		VertexShader = nullptr;
		delete PixelShader;
		PixelShader = nullptr;
	}
private:
	grcShaderVertex* VertexShader = nullptr;
	grcShaderPixel* PixelShader = nullptr;
};
class grcInputLayout {
public:
	grcInputLayout(grcShaderGroup* shader) {
		D3D11_INPUT_ELEMENT_DESC inputEleDesc[] = {
			{"POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		HRESULT hr = grcDeviced3d::Get()->outDevice->CreateInputLayout(inputEleDesc, 1, shader->GetVertexShader()->GetBlob()->GetBufferPointer(), shader->GetVertexShader()->GetBlob()->GetBufferSize(), &pLayout);
		assert(SUCCEEDED(hr));
	}
	void Bind() {
		grcDeviced3d::Get()->context->IASetInputLayout(pLayout);
	}
	~grcInputLayout() {
		if (pLayout) pLayout->Release();
	}
	ID3D11InputLayout* pLayout = nullptr;
private:
};
class grcBufferD3D {
public:
	grcBufferD3D(D3D11_BIND_FLAG bindType, void* data, UINT size) { // check val 
		D3D11_BUFFER_DESC verBuffDescr = { };
		verBuffDescr.ByteWidth = size; // this would just be the type?
		verBuffDescr.Usage = D3D11_USAGE_DEFAULT;
		verBuffDescr.BindFlags = bindType;
		D3D11_SUBRESOURCE_DATA subRes;
		subRes.pSysMem = data;
		grcDeviced3d::Get()->outDevice->CreateBuffer(&verBuffDescr, &subRes, &m_pBuffer); 
	}
	ID3D11Buffer* GetBuffer() {
		return m_pBuffer;
	}
	ID3D11Buffer** GetAddrOf() { return &m_pBuffer; }
	~grcBufferD3D()
	{		
		
	}
private:
	ID3D11Buffer* m_pBuffer = nullptr;
};
class grcVertexBuffer {
public:
	grcVertexBuffer(void* data, UINT size, UINT stride) { // stride is equal to sizeof(T) * arraySize. So Ig take array size?
		this->m_Stride = stride;
		m_pBuffer = new grcBufferD3D(D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, data, size);
	}
	void Bind() {
		grcDeviced3d::Get()->context->IASetVertexBuffers(0, 1, m_pBuffer->GetAddrOf(), &m_Stride, &m_Offset);
	}
	grcBufferD3D* Get() {
		return m_pBuffer;
	}
	grcBufferD3D** GetAddressOf() { return &m_pBuffer; } // needed for lib access shit
	~grcVertexBuffer() {
		delete m_pBuffer;
	}
private:
	UINT m_Stride = 0;
	UINT m_Offset = 0;
	grcBufferD3D* m_pBuffer = nullptr;
};
class grcIndexBuffer {
public:
	grcIndexBuffer(int* data, UINT size) {
		m_pBuffer = new grcBufferD3D(D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER, data, size);
	}
	void Bind() {
		grcDeviced3d::Get()->context->IASetIndexBuffer(m_pBuffer->GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
	}
	grcBufferD3D* GetBuffer() { return m_pBuffer; }
	grcBufferD3D** GetBufferAddr() { return &m_pBuffer; }
	~grcIndexBuffer() {
		delete m_pBuffer;
		m_pBuffer = nullptr;
	}
private:
	grcBufferD3D* m_pBuffer = nullptr;
};
class grcModel {
public:
	grcModel() {
		m_pShader = new grcShaderGroup(L"W:\\GTAV Scripts\\LAG\\shaders\\VertexShader.hlsl"); // Shader Module which means we have to read from mtl data or from model information Idk
		m_pVertexBuffer = new grcVertexBuffer(fPtr, 12*sizeof(float), 3*sizeof(float));
		m_pIndexBuffer = new grcIndexBuffer(mIndex, 6*sizeof(int));
	}
	void SetupRenderState() { // Does not call Draw().
		m_pShader->Bind();
		m_pIndexBuffer->Bind();
		m_pVertexBuffer->Bind();
	}
	~grcModel() {
		delete m_pIndexBuffer;
		m_pIndexBuffer = nullptr;
		delete m_pVertexBuffer;
		m_pVertexBuffer = nullptr;
		delete m_pShader;
		m_pShader = nullptr;
	}
	grcShaderGroup* GetShaderGroup() { return this->m_pShader; }
	grcVertexBuffer* GetVertexBuffer() { return this->m_pVertexBuffer; }
	grcIndexBuffer* GetIndexBuffer() { return this->m_pIndexBuffer; }
private:
	float fPtr[12] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	int mIndex[6] = {
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
	};
	grcShaderGroup* m_pShader = nullptr;
	grcVertexBuffer* m_pVertexBuffer = nullptr;
	grcIndexBuffer* m_pIndexBuffer = nullptr;
};
class grcStateBlock {
public:
	static void Init() {
		sm_vViewports = new std::vector<D3D11_VIEWPORT>();
		sm_pRasterState = CreateDefaultRasterState();
		assert(sm_pRect != nullptr && __FUNCTION__"Rect is null! Can't create a Viewport off of nothing!");
		sm_vViewports->push_back(CreateViewport(*sm_pRect));
	}
	static void BeginFrame() {
		grcDeviced3d::Get()->context->RSSetState(sm_pRasterState);
		grcDeviced3d::Get()->context->RSSetViewports((UINT)sm_vViewports->size(), sm_vViewports->data()); // yes the cast is intential shut compiler
		grcDeviced3d::Get()->context->IASetPrimitiveTopology(Topology);
	}
	static ID3D11RasterizerState* GetRasterState() { return sm_pRasterState; }
	//Purpose: Assist with clearing or removing state from the D3D11 Immediate Context. 
	static void EndFrame() {
		grcDeviced3d::Get()->context->RSSetState(nullptr); 
		grcDeviced3d::Get()->context->RSSetViewports(0, nullptr); // check clear?
		grcDeviced3d::Get()->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);

	}
	//One call. Destroy the class. Do not call until end. This frees all resources encompassed by this machine.
	static void DestroyStateBlock() {
		if (sm_pRasterState) sm_pRasterState->Release(); sm_pRasterState = nullptr;
		if (sm_vViewports) sm_vViewports->clear(); delete sm_vViewports; sm_vViewports = nullptr;
	}
	static inline D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	static inline RECT* sm_pRect = nullptr;
private:
	static inline ID3D11RasterizerState* sm_pRasterState = nullptr;
	static inline std::vector<D3D11_VIEWPORT>* sm_vViewports = nullptr;
	static ID3D11RasterizerState* CreateDefaultRasterState() {
		ID3D11RasterizerState* ret = nullptr;
		D3D11_RASTERIZER_DESC desc = {};
		desc.CullMode = D3D11_CULL_BACK;
		desc.FillMode = D3D11_FILL_SOLID;
		HRESULT hr = grcDeviced3d::Get()->outDevice->CreateRasterizerState(&desc, &ret);
		assert(SUCCEEDED(hr)); //cap that mf
		return ret;
	}
	static D3D11_VIEWPORT CreateViewport(const RECT& rect) {
		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)(rect.right - rect.left), (FLOAT)(rect.bottom - rect.top), 0.0f, 1.0f };
		return viewport;
	}
};
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
	grcInputLayout iaLayout = grcInputLayout(model.GetShaderGroup()); // move this into model since it relies on info of model
	float fClearCol[4] = { 0 / 255.0f, 0 / 255.0f, 32 / 255.0f, 255 / 255.0f }; 
	MSG msg = { };
	bool m_bShouldClose = false;
	RECT winRect;
	GetClientRect(hwnd, &winRect);
	grcStateBlock::sm_pRect = &winRect;
	grcStateBlock::Init();
	while (!m_bShouldClose)
	{
		m_bShouldClose = HandleWindowMessages(&msg);
		grcStateBlock::BeginFrame();
		GetClientRect(hwnd, &winRect);
		device.context->ClearRenderTargetView(device.backBuffer, fClearCol); // As it is a apart of the stateblock what we need to do is Clear when we do a NEW FRAME (not begin a frame because we have to setup like topology inside of that.
		device.context->OMSetRenderTargets(1, &device.backBuffer, nullptr); //I need someway to have dynamic render targets or rendertarget creation dynamically. Handled internally.
		iaLayout.Bind(); 
		model.SetupRenderState();
		device.context->DrawIndexed(6, 0, 0);
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