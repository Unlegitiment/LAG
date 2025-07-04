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

namespace legit {
	template<typename T = unsigned long long> using u64 = T;
	template<typename T = long long> using s64 = T;
	template<typename T = unsigned int> using u32 = T;
	template<typename T = int> using s32 = T;
	template<typename T = unsigned short> using u16 = T;
	template<typename T = signed short> using s16 = T;
	template<typename T = unsigned char> using u8 = T;
	template<typename T = signed char> using s8 = T;
	template<typename T = float> using f32 = T;
	template<typename T = double> using f64 = T;
	namespace lit {
		// To write a template library you must first invent the universe.
		template<typename T, T val>
		struct Type {
			using ValueType = T;
			using Constant = Type;
			static constexpr T value = val;
		};
		template<bool val> using TypeConditional = Type<bool, val>; using TrueConditional = TypeConditional<true>; using FalseConditional = TypeConditional<false>;
		template<typename T> struct IsPrimitive : public FalseConditional {};
		template<> struct IsPrimitive<int> : public TrueConditional {};
		template<typename T> struct IsPointer : public FalseConditional {};
		template<typename T> struct IsPointer<T*> : public TrueConditional {};
		template<typename T> struct StripPointer {
			using Type = T;
			using NoPointerType = T;
		};
		template<typename T> struct StripPointer<T*> { 
			using Type = T; // check?
			using NoPointerType = typename StripPointer<T>::NoPointerType; // OHH ITS RECURSION THATS FUCKING FIRE DUDE!
		};
		template<typename T> struct IsDefaultConstructable {
		private:
			template<typename U, typename = decltype(U())> static TrueConditional Test(int);
			template<typename> static FalseConditional Test(...);
		public:
			static constexpr bool value = decltype(Test<T>(1))::value;
		};
		template<typename T> struct AddRValue {
			using type = T&&;
		};
		template<typename T> struct AddRValue<T&> {
			using type = T&;
		};
		template<> struct AddRValue<void> {
			using type = void;
		};
		template<typename T> typename AddRValue<T>::type DeclaredValue(); // This basically means like the value WILL exist but right now we are in compiler so the value doesn't.
		template<typename T> struct IsCopyable {
		private:
			template<typename U, typename = decltype(U(DeclaredValue<const U&>()))> static TrueConditional Test(int);
			template<typename> static FalseConditional Test(...);
		public:
			static constexpr bool value = decltype(Test<T>(1))::value;
		};
		template<bool Condition, typename T = void> struct EnableTypeIf { };
		template<typename T> struct EnableTypeIf<true, T> {
			using type = T;
		};
		template<bool Condition> struct EnableValueIf : public FalseConditional { };
		template<> struct EnableValueIf<true> : public TrueConditional { };
		// I only care about One operation right now. 
		enum eOperation {
			COMPARISON,
		};
		template<eOperation operation, typename T, typename = typename EnableTypeIf<!IsPrimitive<T>::value>::type> struct IsOperationSpecified {
			static constexpr bool value = false;
		};
		template<typename T> struct IsOperationSpecified<COMPARISON, T> {
		private:
			template<typename U, typename = decltype(DeclaredValue<U&&>().operator==(DeclaredValue<const U&>()))> static TrueConditional Test(int); //bool operator==(const U&) const {}
			template<typename> static FalseConditional Test(...);
		public:
			static constexpr bool value = decltype(Test<T>(1))::value;
		};
		template<typename T> static constexpr bool IsCopyableV = IsCopyable<T>::value;
		template<typename T> constexpr bool HasComparisonOperatorV = IsOperationSpecified<COMPARISON, T>::value;
		template<typename T>
		class Allocator {
		public:
			T* Allocate(u32 amt) {
				void* test = nullptr;
				bool b = IsPointer<decltype(test)>::value;
				return new T[amt]; //  we don't specify constructor arguments bc we don't know if there will be one. 
			}
		private:
		};
		template<typename T> struct DefaultComparison {
			static bool Compare(const T& a, const T& b) {
				return a == b; // assume operator== exists. @Todo implement Function Exists TMP.
			}
		};
		template<typename T, Allocator<T> Allocator, template<typename> typename Comparison = DefaultComparison> class List {
		private:
			void AddCommon(T t) {
			}
		public:
			void AddAndGrow(T&& val) { // value is temporary

			}
			void AddAndGrow(const T& val) { // value COULD be temp or perminent. either way we aren't gonna take chances and just assume its temporary.

			}
			void AddAndGrow(T& val) { // value has to exist. (fun fact this is not how they work. )

			}
			T operator[](u32 location) {
				static_assert(location < m_Size && __FUNCTION__" Location is greater than alloted size.");
				return m_pArray[location];
			}
		private:
			T* m_pArray;
			u32 m_Size; // namespace Legit. once we move LIT out of LEGIT this will cause issues. 
			u32 m_Compacity;
		};
	}

template<typename T = const char*, lit::Allocator<T> Allocator>
class lfsPath{
public:
	lfsPath(const char* path) : {

	}
	void StripPath() {

	}
private:
	char* m_Path;
};
} // namespace legit.

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*
	Research Notes on Concept of RenderPass:
		- Effectively In Vulkan its a manager of subpasses 
		- Subpasses are basically dependancy related objects that tell the driver that we intend to output to resource a or we are using resource a in subpass b. 
		- Renderpasses do not own the objects they relay on they simply manage them so the driver can handle them (this is Vulkan with CMD Buffers and a bunch of shi.)
		- A renderpass in the case of D3D11 can just be something that manages the output merger. Unlike grcStateBlock which controls Rasterizer States, and Topology, this class basically manages OutputMerger objects.
		- Things like Depth (+ its state), RenderTargets, etc all run through a renderpass. 
		- We should also specify that a RenderSegment is different from a RenderPass cause in my head a RenderSegment is an entire like segment of a Draw they also own RenderPasses, the objects they intend to draw etc. basically an entire pass.
		- Old Varient of the class. Do not use. Check RenderSegment
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
		throw std::exception(__FUNCTION__"Unimplemented right now!");
	}
	virtual void ExecuteDrawLists() {
		throw std::exception(__FUNCTION__"Unimplemented");
	}
private:

};
struct sVector3 {
	float x, y, z;
};

class fwEntity {
public:
	fwEntity(grcModel* pModel) {
		this->m_pModel = pModel; // replace but right now it works :\ 
	}
	void Update() {

	}
	grcModel* GetModel() {
		return this->m_pModel;
	}
	sVector3& GetPosition() {
		return position;
	}
	void SetPosition(float x, float y, float z) {
		this->position.x = x;
		this->position.y = y;
		this->position.z = z;
	}
private:
	grcModel* m_pModel = nullptr;
	sVector3 position;
};
class fwScene {
public:
	void AddEntityToScene(fwEntity* e) {
		if (!m_bCanEntitiesBeAddedToScene) return;
		this->m_pEntities.push_back(e);
	}
	void LockScene() {
		this->m_bCanEntitiesBeAddedToScene = false;
	}
	void UnlockScene() {
		this->m_bCanEntitiesBeAddedToScene = true;
	}
	std::vector<fwEntity*>& GetEntities() {
		return this->m_pEntities;
	}
private:
	bool m_bCanEntitiesBeAddedToScene = true;
	std::vector<fwEntity*> m_pEntities; // list of our entities in our current scene.
};
class grcModelCache {
public:
	friend class grcModelFactory;
	void Add(const char* modelName, grcModel* model) {
		if (GetModel(modelName)) return; // we don't want a double insert.
		this->m_Models.insert({ modelName, model });
	}
	grcModel* GetModel(const char* modelName) { // should find a quick hash algo that allows me to do this automatically? Since I liked JOOAT maybe that? 
		auto it = m_Models.find(modelName);
		if (it == m_Models.end()) return nullptr; // we did not find the model.
		return it->second;
	}
	void RemoveModel(const char* modelName) {
		auto it = m_Models.find(modelName);
		if (it == m_Models.end()) return; // did not find.
		m_Models.erase(it);
		return;
	}
private:
	std::unordered_map<std::string, grcModel*> m_Models; // I seriously hate std::iterators. would rather step on legos after getting out of the shower 1000 times than write another it == m_Map.end()
};
class grcModelFactory {
public:
	static void Init() {
		sm_pModelCache = new grcModelCache();
	}
	static grcModel* CreateModel(const char* filePath) {		
		if (auto* model = sm_pModelCache->GetModel(filePath)) {
			return model;
		}
		grcModel* model = new grcModel(); // need to take a vector of points or just an ID3D11Buffer
		sm_pModelCache->Add(filePath, model);
		return model;
	}
	static void Shutdown() {
		for (auto [key, val] : sm_pModelCache->m_Models) {
			delete val;
			val = nullptr;
		}
		sm_pModelCache->m_Models.clear(); // clear the map since we are done with it.
		delete sm_pModelCache;
		sm_pModelCache = nullptr;
	}
private:
	static inline grcModelCache* sm_pModelCache = nullptr;
};
class CGame {
public:
	static void Init() {
		grcModelFactory::Init();
		m_pScene = new fwScene();
	}
	static fwScene* GetScene() {
		return m_pScene;
	}
	static void Shutdown() {
		delete m_pScene;
		grcModelFactory::Shutdown();
	}
private:
	static inline fwScene* m_pScene = nullptr;
};
class CSceneRenderSegment : public CRenderSegment {
public:
	void BuildDrawList() {
		CGame::GetScene()->LockScene();
		for (auto* entity : CGame::GetScene()->GetEntities()) {
			this->m_pEntities.push_back(entity);
		}
	}
	void ExecuteDrawLists() {
		for (int i = 0; i < m_pEntities.size(); i++) {
			fwEntity* entity = m_pEntities[i];
			entity->GetModel()->Draw(entity->GetPosition().x, entity->GetPosition().y, entity->GetPosition().z);
		}
		CGame::GetScene()->UnlockScene();
		m_pEntities.clear(); // clear the cache of entities. 
	}
	std::vector<fwEntity*>& GetEntities() { return this->m_pEntities; }
private:
	std::vector<fwEntity*> m_pEntities;
};
class CRenderSegmentMgr {
	friend class CRenderer;
public:
	void AddRenderSegment(CRenderSegment* pSegment) {
		this->m_RenderSegments.push_back(pSegment);
	}
	CRenderSegment* GetCurrentSegment() {
		return m_pCurrentRenderSegment;
	}
	void Execute() {
		for (auto* seg : m_RenderSegments) {
			this->m_pCurrentRenderSegment = seg;
			seg->BuildDrawList();
			seg->ExecuteDrawLists(); 
		}
	}
private:
	std::vector<CRenderSegment*> m_RenderSegments;
	CRenderSegment* m_pCurrentRenderSegment = nullptr;
};
// Depends on CWINDOW
class CRenderer { 
public:
	static void Init() {
		device = new grcDeviced3d((HWND)CWindow::GetHandle()); 
		device->Set(device);
		InitSegments();
	}
	static void Render() {
		sm_pRenderSegments->Execute();
	}
	static void Destroy() {
		DestroySegments();
		delete device;
		device = nullptr;
	}
	static grcDeviced3d* GetDevice() { return device; }
private:
	static void InitSegments() {
		sm_pRenderSegments = new CRenderSegmentMgr();
		sm_pRenderSegments->AddRenderSegment(new CSceneRenderSegment());
	}
	static void DestroySegments() {
		size_t size = sm_pRenderSegments->m_RenderSegments.size();
		for (size_t i = 0; i < size; i++) {
			delete sm_pRenderSegments->m_RenderSegments[i];
		}
		sm_pRenderSegments->m_RenderSegments.clear();
	}
	static inline CRenderSegmentMgr* sm_pRenderSegments = nullptr;
	static inline grcDeviced3d* device = nullptr;
};
//how tf do I define what texture is lmao?
class grcTexture2D {

public:
	explicit grcTexture2D(const char* path) {
		int x, y, comp;
		this->m_pTexture = stbi_load(path, &x, &y, &comp, 4); // 4 is bc we need RGBA values but should be passed. but should specify in image config.  
		D3D11_TEXTURE2D_DESC textureDesc = D3D11_TEXTURE2D_DESC();
		textureDesc.Format = m_iFormat; // Should specify in constructor what we want the BYTE_STRIDE to be. (probably have a pre-arranged global list of shit)
		textureDesc.Width = x;
		textureDesc.Height = y;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = this->m_pTexture;
		data.SysMemPitch = x * m_iFormatStride; // its the per byte stride so like ie. 0xff'ff'ff'ff is 4 byte types that equal a colour per-pixel. three would be 0xff'ff'ff and would only be RGB ( this is my thoughprocess idrk if this is actually how it works )
		if (grcDeviced3d::Get()->outDevice->CreateTexture2D(&textureDesc, &data, &m_pRawTexture) != S_OK) {
			// must 
			OutputDebugStringA("failed to create texture.");
		}
	}
	ID3D11ShaderResourceView* GetShaderResource() {
		if (m_pView) return m_pView; // no need to create if we already have it. might want to remove this bc it could be bad. if the texture corrupts or something
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = m_iFormat; // This must be equal to textureDesc's Format. Cache format to class. 
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;
		ID3D11ShaderResourceView* srv = nullptr;
		if (grcDeviced3d::Get()->outDevice->CreateShaderResourceView(this->m_pRawTexture, &desc, &srv) != S_OK) {
			throw std::runtime_error(__FUNCTION__"[SRV] couldn't create resource view");
		}
		this->m_pView = srv;
		return srv;
	}
	ID3D11Texture2D* GetRawTexture() {
		return this->m_pRawTexture;
	}
	ID3D11ShaderResourceView** GetSRVPtr() { return &this->m_pView; }
	~grcTexture2D() {
		if (this->m_pView) {
			m_pView->Release();
			m_pView = nullptr;
		}
		m_pRawTexture->Release(); // gpu Texture
		m_pRawTexture = nullptr; // Clear the Texture.
		stbi_image_free(m_pTexture); // other texture. what physical mem? I mean its temp we don't write could just allocate it then send it to the GPU.  why store it in two places right?
		m_pTexture = nullptr;
	}
private:
	DXGI_FORMAT m_iFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	int m_iFormatStride = 4;
	ID3D11ShaderResourceView* m_pView = nullptr;
	void* m_pTexture = nullptr; // this is the stbi_image stuff not sure 
	ID3D11Texture2D* m_pRawTexture = nullptr;
};
int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance, [[maybe_unused]]HINSTANCE hPrevInstance, [[maybe_unused]]PWSTR pCmdLine, [[maybe_unused]]int nCmdShow)
{
	// Register the window class.
	CWindow::Init(hInstance);
	CGame::Init();
	CRenderer::Init();
	// Run the message loop.
	grcDeviced3d* raw_Dev = CRenderer::GetDevice();
	if (!raw_Dev) return 1; // failed
	grcDeviced3d device = *raw_Dev; // Why not just make this NOT like this you know?
	//device.Set(&device);
	grcModel* model = grcModelFactory::CreateModel("C\\Windows\\System32\\test.obj"); // we don't create any differing model this is just to verify it works.
	grcInputLayout iaLayout = grcInputLayout(model->GetShaderGroup()->GetVertexShader()); // move this into model since it relies on info of model
	//float fClearCol[4] = { 0 / 255.0f, 0 / 255.0f, 32 / 255.0f, 255 / 255.0f }; 
	MSG msg = { };
	bool m_bShouldClose = false;
	RECT winRect;
	GetClientRect((HWND)CWindow::GetHandle() , &winRect);
	grcStateBlock::sm_pRect = &winRect;
	grcStateBlock::Init();
	CTimer timer;
	HRESULT hr = S_OK;
	CRenderPass renderPass = CRenderPass(); // I need this right now because I need the Depth stuff, but ideally just repurpose into a DSV class.
	renderPass.InitClass();
	//int x = 0, y = 0, comp = 0;
	//stbi_uc* uc = stbi_load("W:\\GTAV Scripts\\LAG\\LAG\\Assets\\char_social_club.jpg", &x, &y, &comp, 4); // oopse
	grcTexture2D texture = grcTexture2D("W:\\GTAV Scripts\\LAG\\LAG\\Assets\\char_social_club.jpg"); 
	texture.GetShaderResource();
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	ID3D11SamplerState* samplerState = nullptr; // WAH WAH I DON'T WANT THE SAMPLER STATES MOMMMM
	hr = device.outDevice->CreateSamplerState(&sampDesc, &samplerState);
	assert(SUCCEEDED(hr) && "Failed to do CreateSamplerState");
	//stbi_image_free(uc);
	//ImGUI setup
	SetupDebug((HWND)CWindow::GetHandle());
	//honestly this system is very suspicious and I wouldn't recommend it.
	renderPass.PushRenderTarget(grcDeviced3d::Get()->backBuffer);
	fwEntity* entity = new fwEntity(model);
	entity->SetPosition(0.5, 0.5, 0.5);
	fwEntity* entity2 = new fwEntity(grcModelFactory::CreateModel("C\\Windows\\System32\\test.obj"));
	entity2->SetPosition(1, 1, 1);
	CGame::GetScene()->AddEntityToScene(entity);
	CGame::GetScene()->AddEntityToScene(entity2);
	entity->SetPosition(-1, 0, -4);
	//CGame::GetScene()->AddEntityToScene(entity);
	while (!m_bShouldClose)
	{
		//float fDelta = timer.GetDelta();
		m_bShouldClose = HandleWindowMessages(&msg);
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Cam");
			ImGui::SliderFloat("X(Obj)", &entity->GetPosition().x, -100, 100);
			ImGui::SliderFloat("Y(Obj)", &entity->GetPosition().y, -100, 100);
			ImGui::SliderFloat("Z(Obj)", &entity->GetPosition().z, -100, 100);
			ImGui::SliderFloat("X(Cam)", &model->camera->POSITION.m128_f32[0], -360, 360); // 
			ImGui::SliderFloat("Y(Cam)", &model->camera->POSITION.m128_f32[1], -360, 360);
			ImGui::SliderFloat("Z(Cam)", &model->camera->POSITION.m128_f32[2], -360, 360); // need to recalc pos
			ImGui::SliderFloat("Yaw", &model->camera->Yaw, -360, 360);
			ImGui::SliderFloat("Pitch", &model->camera->Pitch, -360, 360);
			ImGui::SliderFloat("Zoom", &model->camera->Zoom, 0, 100);
			if (ImGui::Button("Reset")) {
				model->camera->POSITION = { 0.0,0.0,0.0,0.0f }; // make vector!
				model->camera->FRONT = { 0.0,0.0,-1.0f };
				model->camera->UP = { 0.0,1.0,0.0 };
				model->camera->RIGHT;
				model->camera->WORLDUP = { 0.0,1.0,0.0 };
				model->camera->Yaw = -90.0;
				model->camera->Pitch = 0.0f;
				model->camera->Zoom = 45.f;
			}
		ImGui::End();
		grcStateBlock::BeginFrame();
		GetClientRect((HWND)CWindow::GetHandle(), &winRect);
		renderPass.BeginFrame();
		iaLayout.Bind(); 
		device.context->PSSetShaderResources(0, 1, texture.GetSRVPtr());
		device.context->PSSetSamplers(0, 1, &samplerState); // OOPSIES LMAO ( had this lower than the first render which means it wasn't used lmao
		CRenderer::Render();
		//We could abstract this part out right now and setup so that we have some sort of intermingling of shader sampler shit. hmm.
		renderPass.EndFrame(); // Does nothing but makes sense here. -- wow wonderful explaination get this guy a medal.
		grcStateBlock::EndFrame(); // clear and await more instruction.

		//finalization of rendering. 
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		device.swapChain->Present(1, 0);
	}
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	grcStateBlock::DestroyStateBlock();
	//renderPass.DestroyClass();
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