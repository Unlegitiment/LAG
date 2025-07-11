#include "modelload.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <d3d11.h>       // D3D interface
#include <dxgi.h>        // DirectX driver interface
#include <d3dcompiler.h> // shader compiler
#include <stdexcept>
#include <cassert>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include "Shader\Shadergroup.h"
#include "Buffer\Buffer.h"
#include <LAG\model\grcmodel.h>
#include <LAG\InputAssembly\InputAssembly.h>
#include <LAG\State\Stateblock.h>
#include <__msvc_chrono.hpp>
//#include <assimp/Importer.hpp>
#include <DirectXMath.h>
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"
#pragma comment( lib, "user32" )          // link against the win32 library
#pragma comment( lib, "d3d11.lib" )       // direct3D library
#pragma comment( lib, "dxgi.lib" )        // directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler
//@@@@ Important! - On the next iteration of the renderer we need to successfully start working on Dynamic Vertex Stuff which is just gonna be specified/created when we create the vbo objects. Also scratch the grcGeometry concept. grcTexture should also only handle renderer specific stuff. Should be tied to a higher system that differenciated between texture reading/filenames and their raw data.
// We basically just want a buffer between like REAL code like stbi_image loading and then like actual render stuff. 

//namespace legit {
//template<typename T = unsigned long long> using u64 = T;
//template<typename T = long long> using s64 = T;
//template<typename T = unsigned int> using u32 = T;
//template<typename T = int> using s32 = T;
//template<typename T = unsigned short> using u16 = T;
//template<typename T = signed short> using s16 = T;
//template<typename T = unsigned char> using u8 = T;
//template<typename T = signed char> using s8 = T;
//template<typename T = float> using f32 = T;
//template<typename T = double> using f64 = T;
//namespace lit {
//// To write a template library you must first invent the universe.
//template<typename T, T val>
//struct Type {
//	using ValueType = T;
//	using Constant = Type;
//	static constexpr T value = val;
//};
//template<bool val> using TypeConditional = Type<bool, val>; using TrueConditional = TypeConditional<true>; using FalseConditional = TypeConditional<false>;
//template<typename T> struct IsPrimitive : public FalseConditional {};
//template<> struct IsPrimitive<int> : public TrueConditional {};
//template<typename T> struct IsPointer : public FalseConditional {};
//template<typename T> struct IsPointer<T*> : public TrueConditional {};
//template<typename T> struct StripPointer {
//	using Type = T;
//	using NoPointerType = T;
//};
//template<typename T> struct StripPointer<T*> {
//	using Type = T; // check?
//	using NoPointerType = typename StripPointer<T>::NoPointerType; // OHH ITS RECURSION THATS FUCKING FIRE DUDE!
//};
//template<typename T> struct IsDefaultConstructable {
//private:
//	template<typename U, typename = decltype(U())> static TrueConditional Test(int);
//	template<typename> static FalseConditional Test(...);
//public:
//	static constexpr bool value = decltype(Test<T>(1))::value;
//};
//template<typename T> struct AddRValue {
//	using type = T&&;
//};
//template<typename T> struct AddRValue<T&> {
//	using type = T&;
//};
//template<> struct AddRValue<void> {
//	using type = void;
//};
//template<typename T> typename AddRValue<T>::type DeclaredValue(); // This basically means like the value WILL exist but right now we are in compiler so the value doesn't.
//template<typename T> struct IsCopyable {
//private:
//	template<typename U, typename = decltype(U(DeclaredValue<const U&>()))> static TrueConditional Test(int);
//	template<typename> static FalseConditional Test(...);
//public:
//	static constexpr bool value = decltype(Test<T>(1))::value;
//};
//template<bool Condition, typename T = void> struct EnableTypeIf { };
//template<typename T> struct EnableTypeIf<true, T> {
//	using type = T;
//};
//template<bool Condition> struct EnableValueIf : public FalseConditional { };
//template<> struct EnableValueIf<true> : public TrueConditional { };
//// I only care about One operation right now. 
//enum eOperation {
//	COMPARISON,
//};
//template<eOperation operation, typename T, typename = typename EnableTypeIf<!IsPrimitive<T>::value>::type> struct IsOperationSpecified {
//	static constexpr bool value = false;
//};
//template<typename T> struct IsOperationSpecified<COMPARISON, T> {
//private:
//	template<typename U, typename = decltype(DeclaredValue<U&&>().operator==(DeclaredValue<const U&>()))> static TrueConditional Test(int); //bool operator==(const U&) const {}
//	template<typename> static FalseConditional Test(...);
//public:
//	static constexpr bool value = decltype(Test<T>(1))::value;
//};
//template<typename T> static constexpr bool IsCopyableV = IsCopyable<T>::value;
//template<typename T> constexpr bool HasComparisonOperatorV = IsOperationSpecified<COMPARISON, T>::value;
//template<typename T>
//class Allocator {
//public:
//	T* Allocate(u32 amt) {
//		void* test = nullptr;
//		bool b = IsPointer<decltype(test)>::value;
//		return new T[amt]; //  we don't specify constructor arguments bc we don't know if there will be one. 
//	}
//private:
//};
//template <typename T, bool = IsPrimitive<T>::value>
//struct DefaultComparison
//{ // for customs
//	static bool Compare(T& a, T& b)
//	{
//		static_assert(HasComparisonOperatorV<T> && "Operator T does not have valid comparison");
//		return a == b;
//	}
//};
//template <typename T>
//struct DefaultComparison<T, true>
//{
//	static bool Compare(T& a, T& b)
//	{
//		return a == b;
//	}
//};
//template<typename T, Allocator<T> Allocator, DefaultComparison<T> comparison = DefaultComparison<T>()> class List {
//private:
//	void AddCommon(T t) {
//	}
//public:
//	void AddAndGrow(T&& val) { // value is temporary
//
//	}
//	void AddAndGrow(const T& val) { // value COULD be temp or perminent. either way we aren't gonna take chances and just assume its temporary.
//
//	}
//	void AddAndGrow(T& val) { // value has to exist. (fun fact this is not how they work. )
//
//	}
//	T operator[](u32 location) {
//		static_assert(location < m_Size && __FUNCTION__" Location is greater than alloted size.");
//		return m_pArray[location];
//	}
//private:
//	T* m_pArray;
//	u32 m_Size; // namespace Legit. once we move LIT out of LEGIT this will cause issues. 
//	u32 m_Compacity;
//};
//} // namespace lit.
//
//template<typename T = const char*, lit::Allocator<T> Allocator>
//class lfsPath {
//public:
//	lfsPath(const char* path) : {
//
//	}
//	void StripPath() {
//
//	}
//private:
//	char* m_Path;
//};
//} // namespace legit.

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
	void* GetTexture() { return this->m_pTexture; }
	ID3D11ShaderResourceView** GetSRVPtr() { return &this->m_pView; }
	~grcTexture2D() {
		if (this->m_pView) {
			m_pView->Release();
			m_pView = nullptr;
		}
		if (m_pRawTexture) {
			m_pRawTexture->Release(); // gpu Texture
			m_pRawTexture = nullptr; // Clear the Texture.
		}
		if (m_pTexture) {
			stbi_image_free(m_pTexture); // other texture. what physical mem? I mean its temp we don't write could just allocate it then send it to the GPU.  why store it in two places right?
			m_pTexture = nullptr;
		}
	}
private:
	DXGI_FORMAT m_iFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	int m_iFormatStride = 4;
	ID3D11ShaderResourceView* m_pView = nullptr;
	void* m_pTexture = nullptr; // this is the stbi_image stuff not sure 
	ID3D11Texture2D* m_pRawTexture = nullptr;
};
class grcMaterial {
public:
	using Texture = grcTexture2D;
	using Textures = std::vector<Texture*>;
	grcMaterial() = default;
	grcMaterial(Textures texture) : m_Textures(texture){

	}
	bool DoesTextureExistAt(int geomSlotIdx) {
		if (geomSlotIdx > m_Textures.size()) return false;
		if (m_Textures[geomSlotIdx] == nullptr) return false;
		return true;
	}
	bool IsEmpty() { return m_Textures.empty(); }
	Textures& GetTextures() { return m_Textures; }
	bool SetTextureAt(int index, Texture* texture) {
		if (index > m_Textures.size()) { 
			size_t diff = m_Textures.size() - index;
			if (diff == 1) { // we'd like to push back. 
				m_Textures.push_back(texture);
				return true;
			}
			return false; 
		}// index is greater than alloted textures. 
		m_Textures[index] = texture;
		return true;
	}
	void Bind() {
		for (int i = 0; i < m_Textures.size(); i++) {
			if (!m_Textures[i]) {
				m_Textures[i] = new grcTexture2D("W:\\GTAV Scripts\\LAG\\LAG\\Assets\\missing.png");
			}
			m_Textures[i]->GetShaderResource();
			grcDeviced3d::Get()->context->PSSetShaderResources(i, (UINT)1u, m_Textures[i]->GetSRVPtr()); // potentially the LEAST EFFICIENT way to  do this lmao. Should really just like do something else lmao. 
		}
	}
private:
	Textures m_Textures;
};

template<typename MeshType, typename MatType, typename SceneType, typename eTextureType, typename OutTexture = grcTexture2D> struct TextureTypes {
	using Mesh = typename MeshType;
	using Material = typename MatType;
	using Scene = typename SceneType;
	using oTexture = typename OutTexture;
	using iTextureEnumType = typename eTextureType;
};
template<typename T> class grcMaterialLoader {

};
#include <iostream>
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"
#include <numeric>
std::string FixPath(std::string str) {
	for (auto& c : str) if (c == '/') c = '\\';
	return str;
}
using Texture2D = grcTexture2D;
#include <array>
class TextureRecord {
private:
	void ClearAll() {
		for (auto& vec : m_Textures) {
			for (auto ptr : vec) delete ptr;
			vec.clear();
		}
	}
public:
	TextureRecord(const TextureRecord&) = delete;
	TextureRecord& operator=(const TextureRecord&) = delete;
	TextureRecord(TextureRecord&& other) noexcept {
		m_Textures = std::move(other.m_Textures);
		// other.m_Textures cleared automatically
	}

	TextureRecord& operator=(TextureRecord&& other) noexcept {
		if (this != &other) {
			ClearAll();
			m_Textures = std::move(other.m_Textures);
		}
		return *this;
	}

	static constexpr size_t CONTAINER_MAX = aiTextureType::aiTextureType_UNKNOWN + 1;
	TextureRecord() = default;

	// Add raw pointer, takes ownership
	void AddTexture(aiTextureType type, Texture2D* texture) {
		m_Textures[type].push_back(texture);
	}

	void ClearTexture(aiTextureType type) {
		for (auto ptr : m_Textures[type]) {
			delete ptr;
		}
		m_Textures[type].clear();
	}

	void ClearTexture(aiTextureType type, size_t index) {
		if (index < m_Textures[type].size()) {
			delete m_Textures[type][index];
			m_Textures[type].erase(m_Textures[type].begin() + index);
		}
	}

	Texture2D* GetTextureAt(aiTextureType ai, size_t idx) const {
		if (idx >= m_Textures[ai].size()) return nullptr;
		return m_Textures[ai][idx];
	}

	size_t GetAmountOfTexturesAtType(aiTextureType ai) const {
		return m_Textures[ai].size();
	}

	size_t GetBounds() const { return CONTAINER_MAX; }

	~TextureRecord() {
		for (size_t i = 0; i < CONTAINER_MAX; ++i) {
			ClearTexture(static_cast<aiTextureType>(i));
		}
	}
private:
	std::array<std::vector<Texture2D*>, CONTAINER_MAX> m_Textures;
};
class MaterialLoader {
public:
	MaterialLoader(const aiScene* scene, const aiMesh* target) : m_pTarget(target), m_pScene(scene) {}

	Texture2D* GetSingleTexture(aiMaterial* material, aiTextureType type, unsigned int index) {
		aiString temp;
		material->GetTexture(type, index, &temp);
		if (temp.length == 0) return nullptr;

		std::string basePath = "W:\\GTAV Scripts\\LAG\\LAG\\Assets\\" + FixPath(temp.C_Str());
		Texture2D* tex = new Texture2D(basePath.c_str());
		if (!tex->GetTexture()) {
			printf("Failed to load texture: %s\n", basePath.c_str());
			delete tex;
			return nullptr;
		}
		return tex;
	}

	TextureRecord GetTextureList(aiMaterial* material, aiTextureType type) {
		TextureRecord list;
		int count = material->GetTextureCount(type);
		for (int i = 0; i < count; ++i) {
			Texture2D* ptr = GetSingleTexture(material, type, i);
			if (ptr && ptr->GetTexture()) {
				list.AddTexture(type, ptr); // ownership transferred
			}
			else {
				delete ptr;
			}
		}
		return list;
	}

	std::vector<TextureRecord> GetTextures() {
		int idx = m_pTarget->mMaterialIndex;
		aiMaterial* material = m_pScene->mMaterials[idx];
		std::vector<TextureRecord> ret;

		for (int i = 0; i < aiTextureType::aiTextureType_UNKNOWN + 1; i++) {
			TextureRecord record;
			for (unsigned int j = 0; j < material->GetTextureCount((aiTextureType)i); j++) {
				Texture2D* tex = GetSingleTexture(material, (aiTextureType)i, j);
				if (tex) {
					record.AddTexture((aiTextureType)i, tex);
				}
			}
			ret.push_back(std::move(record));
		}

		return ret;
	}
private:
	const aiScene* m_pScene = nullptr;
	const aiMesh* m_pTarget = nullptr;
};

//using AssimpMaterialTypes = TextureTypes<aiMesh, aiMaterial, aiScene, aiTextureType>;
//class grcMaterialLoader<AssimpMaterialTypes>{
//public:
//	using mtl = AssimpMaterialTypes::Material;
//	using msh = AssimpMaterialTypes::Mesh;
//	using scn = AssimpMaterialTypes::Scene;
//	using oTxr = AssimpMaterialTypes::oTexture;
//	using eiTxr = AssimpMaterialTypes::iTextureEnumType;
//	grcMaterialLoader(scn* scene, msh* msh) : pScene(scene), pMesh(msh){
//		mtlIndex = msh->mMaterialIndex;
//	}
//private:
//	lit::u32 mtlIndex = 0;
//	scn* pScene = nullptr;
//	msh* pMesh = nullptr;
//	mtl* pMtl = nullptr;
//};
class CVector3 {
public:
	float x, y, z; // sample.
};
class CVector2 {
public:
	float x, y;
};
struct VertexSpecifier {
	VertexSpecifier(CVector3 v1, CVector2 v2) : vPos(v1), vUVCoord(v2) {};
	CVector3 vPos;
	CVector2 vUVCoord;
};
class grcGeometry { // geometry
public:
	grcGeometry(std::vector<VertexSpecifier> vertexpos, std::vector<lit::u32> indices) {
		this->m_Geometry = vertexpos;
		this->m_Indices = indices;
		//this->m_UV = UV;
	}
	std::vector<VertexSpecifier>& GetGeometry() { return m_Geometry; }
	std::vector<lit::u32>& GetIndices() { return m_Indices; }
	std::vector<CVector2>& GetUV() { return m_UV; }
private:
	std::vector<VertexSpecifier> m_Geometry;
	std::vector<lit::u32> m_Indices;
	std::vector<CVector2> m_UV;
};
class grcMesh {
public:
	grcMesh(grcGeometry* pGeo, std::vector<grcTexture2D*> textures) : m_pGeometry(pGeo), m_Mat(textures){
		Init();
	}
	grcMesh(grcGeometry* pGeo, grcMaterial material) : m_pGeometry(pGeo), m_Mat(material) {
		Init();
	}
	grcMesh(grcGeometry* pGeo, std::vector<TextureRecord>&& textureRecord) : m_pGeometry(pGeo) , m_TextureRecord(std::move(textureRecord)){
		Init();
	}
	void Init() {
		auto geo = m_pGeometry->GetGeometry();
		m_vvBuffer = new grcVertexBuffer(geo.data(), (UINT)(m_pGeometry->GetGeometry().size() * sizeof(VertexSpecifier)), 5 * sizeof(float));
		m_viBuffer = new grcIndexBuffer(m_pGeometry->GetIndices().data(), (UINT)m_pGeometry->GetIndices().size() * sizeof(int));
	}
	void DrawMesh() {
		std::vector<ID3D11ShaderResourceView*> pShaderResources;
		//m_Mat.Bind();
		for (auto& texture : m_TextureRecord) {
			for (int i = 0; i < texture.GetBounds(); i++) {
				for (int j = 0; j < texture.GetAmountOfTexturesAtType((aiTextureType)i); j++) {
					auto a = texture.GetTextureAt((aiTextureType)i, j);
					pShaderResources.push_back(a->GetShaderResource());
				}
			}
		}
		grcDeviced3d::Get()->context->PSSetShaderResources(0u, (UINT)pShaderResources.size(), pShaderResources.data());
		m_vvBuffer->Bind();
		m_viBuffer->Bind();
		grcDeviced3d::Get()->context->DrawIndexed((UINT)m_pGeometry->GetIndices().size(), 0, 0);
	}
private:
	grcGeometry* m_pGeometry;
	grcVertexBuffer* m_vvBuffer;
	grcIndexBuffer* m_viBuffer;
	grcMaterial m_Mat;
	std::vector<TextureRecord> m_TextureRecord;
};
// So since models need to kinda give information to the actual render process (bc we need to pass buffers) we can just allocate stuff here and add some method like GetConstantBuffers() that allows me to set them up earlier than when we get here.
// A lot nicer than trying to access the info here lmao. 
// also get rid of the camera shit here move that to like the segment lmao. 
class grcModel_new {
private:
	std::vector<grcMesh*> m_Meshes;
public:
	void Init() {
		m_pShader = new grcShaderGroup(L"W:\\GTAV Scripts\\LAG\\shaders\\VertexShader.hlsl"); // Shader Module which means we have to read from mtl data or from model information Idk
		if (buffer) m_pShader->AppendShaderConstantBuffer(0, buffer); // needs to be here lmao. 
		CCameraMgr::Init();
		int retVal = CCameraMgr::Get().PushNewCamera(camera);
		CCameraMgr::Get().ActivateCamera(retVal); // woohoo.
	}
	grcModel_new(std::vector<grcMesh*> mesh) : m_Meshes(mesh){
		Init();
	}
	grcModel_new(std::vector<grcMesh*> mesh, std::vector<TextureRecord*> records) {
		Init();
	}
	void DrawItem(float x, float y, float z) {
		this->camera->Update(); // move to scene have the current scene own the camera lend to Renderpass or just hand a smaller object. /shrug
		Test.model = DirectX::XMMatrixRotationZ(0.0f) * DirectX::XMMatrixScaling(1, 1, 1) * DirectX::XMMatrixTranslation(x, y, z);
		Test.view = camera->GetViewMatrix();
		Test.projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(camera->Zoom), 1920.f / 1080.f, 0.5f, 10000.0f);
		Test.Transform = DirectX::XMMatrixTranspose(Test.model * Test.view * Test.projection);
		buffer->Update(&Test, sizeof(Constants)); // fun
		m_pShader->Bind();
		for (auto* mesh : m_Meshes) {
			mesh->DrawMesh();
		}
	}
	grcModel_new(std::vector<grcGeometry*> apGeo) : m_apGeometry(apGeo) {
		m_pShader = new grcShaderGroup(L"W:\\GTAV Scripts\\LAG\\shaders\\VertexShader.hlsl"); // Shader Module which means we have to read from mtl data or from model information Idk
		m_apVertexBuffers.resize(m_apGeometry.size());
		m_apIndexBuffers.resize(m_apGeometry.size());
		for (int i = 0; i < (int)m_apGeometry.size(); i++) {
			grcGeometry* pGeo = m_apGeometry[i];
			m_apVertexBuffers[i] = new grcVertexBuffer(pGeo->GetGeometry().data(), (UINT)(pGeo->GetGeometry().size() * sizeof(VertexSpecifier)), 5 * sizeof(float));
			m_apIndexBuffers[i] = new grcIndexBuffer(pGeo->GetIndices().data(), (UINT)pGeo->GetIndices().size() * sizeof(int));
			if (buffer) m_pShader->AppendShaderConstantBuffer(0, buffer);

		}
		CCameraMgr::Init();
		int retVal = CCameraMgr::Get().PushNewCamera(camera);
		CCameraMgr::Get().ActivateCamera(retVal); // woohoo.
	}
	void Draw(float x, float y, float z) {
		this->camera->Update(); // move to scene have the current scene own the camera lend to Renderpass or just hand a smaller object. /shrug
		Test.model = DirectX::XMMatrixRotationZ(0.0f) * DirectX::XMMatrixScaling(1, 1, 1) * DirectX::XMMatrixTranslation(x, y, z);
		Test.view = camera->GetViewMatrix();
		Test.projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(camera->Zoom), 1920.f / 1080.f, 0.5f, 10000.0f);
		Test.Transform = DirectX::XMMatrixTranspose(Test.model * Test.view * Test.projection);
		buffer->Update(&Test, sizeof(Constants)); // fun
		m_pShader->Bind();

	}
	grcShaderGroup* GetShaderGroup() { return this->m_pShader; }
	grcVertexBuffer** GetVertexBuffer() { return this->m_apVertexBuffers.data(); }
	grcIndexBuffer** GetIndexBuffer() { return this->m_apIndexBuffers.data(); }
	CCamera* camera = new CCamera();
private:
	struct Constants {
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX Transform;
	}Test;
	//CEntityDrawHandler* m_pDrawHandler;
	grcCBuffer* buffer = new grcCBuffer(&Test, sizeof(Constants));
	std::vector<grcGeometry*> m_apGeometry;
	std::vector<grcVertexBuffer*> m_apVertexBuffers;
	std::vector<grcIndexBuffer*> m_apIndexBuffers;
	grcShaderGroup* m_pShader = nullptr;
};
class fwEntity {
public:
	//fwEntity(grcModel* pModel) {
	//	this->m_pModel = pModel; // replace but right now it works :\ 
	//}
	fwEntity(grcModel_new* pModel) {
		this->m_pNewModel = pModel;
	}
	void Update() {

	}
	//grcModel* GetModel() {
	//	return this->m_pModel;
	//}
	grcModel_new* GetModelNew() {
		return this->m_pNewModel;
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
	//grcModel* m_pModel = nullptr;
	grcModel_new* m_pNewModel = nullptr;
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
//class grcModelCache {
//public:
//	friend class grcModelFactory;
//	void Add(const char* modelName, grcModel* model) {
//		if (GetModel(modelName)) return; // we don't want a double insert.
//		this->m_Models.insert({ modelName, model });
//	}
//	grcModel* GetModel(const char* modelName) { // should find a quick hash algo that allows me to do this automatically? Since I liked JOOAT maybe that? 
//		auto it = m_Models.find(modelName);
//		if (it == m_Models.end()) return nullptr; // we did not find the model.
//		return it->second;
//	}
//	void RemoveModel(const char* modelName) {
//		auto it = m_Models.find(modelName);
//		if (it == m_Models.end()) return; // did not find.
//		m_Models.erase(it);
//		return;
//	}
//private:
//	std::unordered_map<std::string, grcModel*> m_Models; // I seriously hate std::iterators. would rather step on legos after getting out of the shower 1000 times than write another it == m_Map.end()
//};
//class grcModelFactory {
//public:
//	static void Init() {
//		sm_pModelCache = new grcModelCache();
//	}
//	static grcModel* CreateModel(const char* filePath) {
//		if (auto* model = sm_pModelCache->GetModel(filePath)) {
//			return model;
//		}
//		ModelImporter importer = ModelImporter(filePath);
//		importer.FillImporter();
//		grcModel* model = new grcModel(importer.CreateGeometry()[0]); // need to take a vector of points or just an ID3D11Buffer
//		sm_pModelCache->Add(filePath, model);
//		return model;
//	}
//	static void Shutdown() {
//		for (auto [key, val] : sm_pModelCache->m_Models) {
//			delete val;
//			val = nullptr;
//		}
//		sm_pModelCache->m_Models.clear(); // clear the map since we are done with it.
//		delete sm_pModelCache;
//		sm_pModelCache = nullptr;
//	}
//private:
//	static inline grcModelCache* sm_pModelCache = nullptr;
//};
class CGame {
public:
	static void Init() {
		//grcModelFactory::Init();
		m_pScene = new fwScene();
	}
	static fwScene* GetScene() {
		return m_pScene;
	}
	static void Shutdown() {
		delete m_pScene;
		//grcModelFactory::Shutdown();
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
			entity->GetModelNew()->DrawItem(entity->GetPosition().x, entity->GetPosition().y, entity->GetPosition().z);
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
		for (int i = 0; i < m_RenderSegments.size(); i++) {
			CRenderSegment* seg = m_RenderSegments.data()[i];
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




using AssimpImport = ImporterParams<Assimp::Importer, aiScene, aiMesh>;
template<>
class CGameModelLoader<AssimpImport> {
private:
	using Importer = typename AssimpImport::ImporterBase;
	using Scene = typename AssimpImport::ImporterScene;
	using Mesh = typename AssimpImport::Mesh;
public:
	CGameModelLoader(const char* fileName, lit::u32 flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs) { // @Todo: Add Baggie of parameters here. For multi-compilations or just specify it inside of another thing. Eitherway need someway to do funnies. 
		m_FileName = fileName;  // might not be safe. 
		m_iFlags = flags;
	}
	void FillImporter() {
		m_pScene = this->m_Importer.ReadFile(m_FileName, m_iFlags);
		if (!m_pScene) {
			throw std::exception("Model has failed to import.");
		}
	}
	const Scene* GetScene() const { return m_pScene; }
	const Scene** GetScenePtr() { return &m_pScene; }
	//Caller must delete geometry when it is necessary. 
	//std::vector<grcGeometry*> CreateGeometry() { // technically a model. 
	//	std::vector<grcGeometry*> SceneGeometry;
	//	Recursive(this->m_pScene->mRootNode, SceneGeometry);
	//	return SceneGeometry;
	//}
	grcModel_new CreateModel() {
		std::vector<grcMesh*> meshes;
		Recursive(m_pScene->mRootNode, meshes);
		return grcModel_new(meshes);
	}
private:
	grcGeometry* Geo(aiNode* pNode) {
		for (unsigned int i = 0; i < pNode->mNumMeshes; i++) {
			unsigned int meshIndex = pNode->mMeshes[i];
			aiMesh* mesh = m_pScene->mMeshes[meshIndex];
			if (mesh->HasPositions()) {
				return LoadOneModel(mesh);
			}
		}
		return nullptr;
	}
	void CollectGeometry(aiNode* pNode, std::vector<grcGeometry*>& SceneGeometry) {
		for (unsigned int i = 0; i < pNode->mNumMeshes; i++) {
			unsigned int meshIndex = pNode->mMeshes[i];
			aiMesh* mesh = m_pScene->mMeshes[meshIndex];
			if (mesh->HasPositions()) {
				SceneGeometry.push_back(LoadOneModel(mesh));
			}
		}
		for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
			CollectGeometry(pNode->mChildren[i], SceneGeometry);
		}
	}

	void Recursive(aiNode* pNode, std::vector<grcMesh*>& SceneMeshes) {
		for (unsigned int i = 0; i < pNode->mNumMeshes; i++) {
			unsigned int meshIndex = pNode->mMeshes[i];
			aiMesh* mesh = m_pScene->mMeshes[meshIndex];

			std::vector<grcGeometry*> geom;
			CollectGeometry(pNode, geom); // Now clearly calls geometry version

			for (auto a : geom) {
				MaterialLoader m_Loader = MaterialLoader(m_pScene, mesh);
				SceneMeshes.push_back(new grcMesh(a, std::move(m_Loader.GetTextures())));
			}
		}

		for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
			Recursive(pNode->mChildren[i], SceneMeshes);
		}
	}
	std::vector<lit::u32> GetIndices(Mesh* mesh) const {
		if (!mesh || !mesh->HasFaces()) {
			throw std::runtime_error("Passed mesh is either null or doesn't have faces.");
		}
		if (!(m_iFlags & aiProcess_Triangulate)) {
			throw std::runtime_error("Failed to give triangulate flag to model initialization.");
		}
		std::vector<lit::u32> Indices;
		Indices.reserve(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			const aiFace& curFace = mesh->mFaces[i];
			if (curFace.mNumIndices != 3) {
				printf("Warning: Non-tri face found: %d indices\n", curFace.mNumIndices);
				continue;
			}
			for (unsigned int j = 0; j < 3; j++) {
				Indices.push_back(curFace.mIndices[j]);
			}
		}
		return Indices;
	}
	std::vector<TextureRecord> GetTextures(aiMesh* mesh) {
		MaterialLoader m_Loader = MaterialLoader(m_pScene, mesh);
		return m_Loader.GetTextures();
	}
	std::vector<CVector2> GetUv(Mesh* mesh) const {
		if (!mesh || !mesh->HasTextureCoords(0)) return {};

		std::vector<CVector2> vec;
		vec.reserve(mesh->mNumVertices);

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D uv = mesh->mTextureCoords[0][i];
			vec.push_back({ uv.x, uv.y });
		}

		return vec;
	}
#include <assimp/material.h>
	std::vector<VertexSpecifier> GetVertexData(Mesh* mesh) const {
		std::vector<VertexSpecifier> vertices;
		if (!mesh || !mesh->HasPositions() || !mesh->HasTextureCoords(0)) return vertices;
		mesh->mMaterialIndex;
		aiMaterial* mat = m_pScene->mMaterials[mesh->mMaterialIndex];
		aiString str;
		if (mat->GetTexture(aiTextureType_AMBIENT, 0, &str) == aiReturn_SUCCESS) {

		}
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				unsigned int index = face.mIndices[j];
				const aiVector3D& pos = mesh->mVertices[index];
				const aiVector3D& uv = mesh->mTextureCoords[0][index];

				vertices.push_back({
					CVector3{ pos.x, pos.y, pos.z },
					CVector2{ uv.x, uv.y }
					});
			}
		}
		return vertices;
	}
	grcGeometry* LoadOneModel(Mesh* mesh) const {
		std::vector<VertexSpecifier> verts = GetVertexData(mesh);
		std::vector<lit::u32> indices(verts.size());
		std::iota(indices.begin(), indices.end(), 0);

		return new grcGeometry(std::move(verts), std::move(indices));
		//return new grcGeometry(GetVertexData(mesh), this->GetIndices(mesh)); // larger models are gonna be really annoying, but this should be done pretty much last or when the model actually NEEDS to be acquired from the game. 
		// I kinda need a system around this that stores the models and allows me to fetch them via like their name or something like that? 
	}
	CVector3 ConvertModelVectorToCommon(aiVector3D& vec) const {
		CVector3 ret{};
		ret.x = vec.x;
		ret.y = vec.y;
		ret.z = vec.z;
		return ret;
	}
private:
	lit::u32 m_iFlags = 0;
	const char* m_FileName = nullptr;
	Importer m_Importer; // 
	const Scene* m_pScene = nullptr;
};
using ModelImporter = CGameModelLoader<
	AssimpImport
>;
int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, [[maybe_unused]] PWSTR pCmdLine, [[maybe_unused]] int nCmdShow)
{
	//CGameModelLoader<AssimpImport> m_AssimpModelImporter = CGameModelLoader<AssimpImport>("filename", 0 | 2 | 4 | 6);
	ModelImporter* i = new ModelImporter("W:\\GTAV Scripts\\LAG\\LAG\\Assets\\mzbnk.obj");
	i->FillImporter();
	//auto geometry = i->CreateGeometry();

	// Register the window class.
	CWindow::Init(hInstance);
	CGame::Init();
	CRenderer::Init();
	// Run the message loop.
	grcDeviced3d* raw_Dev = CRenderer::GetDevice();
	if (!raw_Dev) return 1; // failed
	grcDeviced3d device = *raw_Dev; // Why not just make this NOT like this you know?
	//device.Set(&device);
	//texture.GetShaderResource();
	//std::vector<grcMaterial> materials = { std::vector<grcTexture2D*>{nullptr},std::vector<grcTexture2D*>{nullptr},std::vector<grcTexture2D*>{nullptr},std::vector<grcTexture2D*>{nullptr},std::vector<grcTexture2D*>{nullptr},std::vector<grcTexture2D*>{nullptr}, std::vector<grcTexture2D*>{nullptr},std::vector<grcTexture2D*>{nullptr},std::vector<grcTexture2D*>{nullptr},std::vector<grcTexture2D*>{nullptr},std::vector<grcTexture2D*>{nullptr},std::vector<grcTexture2D*>{nullptr}, std::vector<grcTexture2D*>{nullptr}, std::vector<grcTexture2D*>{nullptr}, std::vector<grcTexture2D*>{nullptr}, std::vector<grcTexture2D*>{&texture}, std::vector<grcTexture2D*>{nullptr}}; // @ todo: ADD A FUCKING TEXTURE SYSTEM YOU DUNCE! ~Tysm <3
	//std::vector<grcMaterial> Materials((int)geometry.size(), grcMaterial({ nullptr }));
	//Materials[15] = grcMaterial({ &texture });
	//std::vector<grcMesh*> meshes;
	//for (int j = 0; j < (int)geometry.size(); j++){
	//	auto* geom = geometry[j];
	//	meshes.push_back(new grcMesh(geom, Materials[j])); // so we still hardcode the stuff here btw lmao so we are gonna get the same result. 
	//}
	grcModel_new* model = new grcModel_new(i->CreateModel()); // we don't create any differing model this is just to verify it works.
	grcInputLayout iaLayout = grcInputLayout(model->GetShaderGroup()->GetVertexShader()); // move this into model since it relies on info of model
	//float fClearCol[4] = { 0 / 255.0f, 0 / 255.0f, 32 / 255.0f, 255 / 255.0f }; 
	MSG msg = { };
	bool m_bShouldClose = false;
	RECT winRect;
	GetClientRect((HWND)CWindow::GetHandle(), &winRect);
	grcStateBlock::sm_pRect = &winRect;
	grcStateBlock::Init();
	CTimer timer;
	HRESULT hr = S_OK;
	CRenderPass renderPass = CRenderPass(); // I need this right now because I need the Depth stuff, but ideally just repurpose into a DSV class.
	renderPass.InitClass();
	//int x = 0, y = 0, comp = 0;
	//stbi_uc* uc = stbi_load("W:\\GTAV Scripts\\LAG\\LAG\\Assets\\char_social_club.jpg", &x, &y, &comp, 4); // oopse

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
	fwEntity* entity2 = new fwEntity(model);
	entity2->SetPosition(1, 1, 1);
	CGame::GetScene()->AddEntityToScene(entity);
	CGame::GetScene()->AddEntityToScene(entity2);
	entity->SetPosition(-1, 0, -4);
	CGame::GetScene()->AddEntityToScene(entity);
	while (!m_bShouldClose)
	{
		//printf("Helllo CSTDIO");
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
		ImGui::SliderFloat("Zoom", &model->camera->Zoom, 0.1f, 120);
		if (ImGui::Button("Reset")) {
			model->camera->POSITION = { 0.0,0.0,0.0,0.0f }; // make vector!
			model->camera->FRONT = { 0.0,0.0,-1.0f };
			model->camera->UP = { 0.0,1.0,0.0 };
			model->camera->RIGHT;
			model->camera->WORLDUP = { 0.0,1.0,0.0 };
			model->camera->Yaw = -90.0;
			model->camera->Pitch = 90.0f;
			model->camera->Zoom = 45.f;
		}
		ImGui::End();
		grcStateBlock::BeginFrame();
		GetClientRect((HWND)CWindow::GetHandle(), &winRect);
		renderPass.BeginFrame();
		iaLayout.Bind();
		//device.context->PSSetShaderResources(0, 1, texture.GetSRVPtr());
		device.context->PSSetSamplers(0, 1, &samplerState); // OOPSIES LMAO ( had this lower than the first render which means it wasn't used lmao
		CRenderer::Render();
		//We could abstract this part out right now and setup so that we have some sort of intermingling of shader sampler shit. hmm.
		renderPass.EndFrame(); // Does nothing but makes sense here. -- wow wonderful explaination get this guy a medal.
		grcStateBlock::EndFrame(); // clear and await more instruction.
		
		//finalization of rendering. 
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		device.swapChain->Present(1, 0);
		Sleep(0);
	}
	delete i;
	i = nullptr;
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