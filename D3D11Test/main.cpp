// From 2/1/25 We are working on a rework of the Graphic Engines core values. Should take a day or two to rework the entire engine from the ground up.
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <cassert>
#include <vector>
#include <chrono>
#include <sstream>
#include "xube.h"
#include "thirdparty/imgui/imgui.h"
#include "thirdparty/imgui/imgui_impl_dx11.h"
#include "thirdparty/imgui/imgui_impl_win32.h"
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"
#include "thirdparty/tiny_obj_loader.h"
#include <algorithm>
#include <shellapi.h>
#include "resourcehandling.h"
#include "device.h"
#include <unordered_map>
using namespace lage;
/*
    Ok jesus christ. We've finally figured out textures. which means we've basically done looked into everything in terms of how stuff is working
    Views are just different metadata for buffers/textures. 
    Textures and Buffers are synonomous one just on the surface means an Image whilst the other means a Buffer of data. 
    Views allow you to have multiple inputs and outputs to textures. which explains that we need a rendertargetview to the backbuffer.
*/
/*
    Jesus Christ. Ok so for context. I keep thinking that things are more advanced in other engines but clearly the object relations are just so
    drawn out so far out that they don't seem connected but they are. 
    For ours we're just gonna go simplisitic. 
    Material information goes in one area. mesh information in another. 
    They are connected its just you've got to go through like so many systems its kinda difficult to understand where it connects anyways i come back in like 3 hrs or smth
    and I stay awake for long time

*/

class grcDevice {
public:
    enum ShaderStages {
        Vertex,
        Hull,
        Compute,
        Pixel,
    };
    void Init() {

    }
    ID3D11Device*           GetDevice() { return this->m_pDevice; } 
    ID3D11DeviceContext*    GetContext() { return this->m_pContext; }
    IDXGISwapChain*         GetSwapChain() { return this->m_pSwapChain; }
    ID3D11RenderTargetView* GetRenderTarget() { return this->m_pRenderTargetView; }
    ID3D11DepthStencilView* GetDepthStencilView() { return this->m_pDepthStencilView; }
    void Shutdown();
private:
    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pContext;
    IDXGISwapChain* m_pSwapChain;
    ID3D11RenderTargetView* m_pRenderTargetView;
    ID3D11DepthStencilView* m_pDepthStencilView; // keep track of depth information;
};
grcDevice* g_Device = new grcDevice();
#define GRCDEVICE g_Device
#define LAG_PC
//We lock before a bind operation to prevent any data from being outgoing to a buffer object. So when we set data set buffer as unlocked.
//class GPUBuffer {
//public:
//    enum BufferLockStatus {
//        LOCKED = false,
//        UNLOCKED = true,
//    };
//protected:
//    ID3D11Buffer* m_pBuffer;
//    BufferLockStatus m_bBufferLockStatus;
//public:
//    GPUBuffer();
//    virtual void CreateBufferOnGPU() {
//        //throw an unimplemented exception here;
//        return;
//
//    };
//    virtual void LockBuffer() noexcept {
//        this->m_bBufferLockStatus = LOCKED;
//    }
//    virtual void UnlockBuffer() noexcept {
//        this->m_bBufferLockStatus = UNLOCKED;
//    }
//    virtual bool IsBufferLocked() noexcept {
//        this->m_pBuffer = nullptr;
//        return m_bBufferLockStatus == LOCKED;
//    }
//    virtual void SetData(void* ptr) = 0;
//    virtual void Bind() const = 0;
//};
/* Example to illustrate why I don't fw SOLID but also it kinda proves my anti-point a lil bit cause im an idiot. The code is so drawn out but also so decoupled and its strange that I just thought of this. 
*/
//platform specific details
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
/*
    Naming Scheme:
    'fw' is equivalent to 'I' in the case of Interfaces. I prefer Framework more. Like a schema. for example fwShaderProgram = Framework Shader Program. 
    Graphics:
    'grc' = Graphics Core. Anything that is generally associated with graphics. Also its very nice to type on the keyboard quickly :)


*/
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;
struct float3 { float x, y, z; };
struct matrix { float m[4][4]; };

matrix operator*(const matrix& m1, const matrix& m2);

std::vector<char> rawBuffer;
namespace dxm = DirectX;
//Creates two buffers on the GPU one for index buffer of a model and the other for the vertex buffers.

std::pair<ID3D11Buffer*, ID3D11Buffer*> CreateD3DBuffer(ID3D11Device* dev, const float* vertexData, size_t vertexDataSize, const UINT* indexData, size_t indexDataSize) {
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.ByteWidth = sizeof (float) * vertexDataSize;
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vertexBufferSRD = { vertexData };
    ID3D11Buffer* vertexbuffer;
    dev->CreateBuffer(&vertexBufferDesc, &vertexBufferSRD, &vertexbuffer);
    D3D11_BUFFER_DESC indexbufferdesc = {};
    indexbufferdesc.ByteWidth = sizeof (int) * indexDataSize;
    indexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA indexbufferSRD = { indexData };
    ID3D11Buffer* indexBuffer;
    dev->CreateBuffer(&indexbufferdesc, &indexbufferSRD, &indexBuffer);
    return { vertexbuffer, indexBuffer };
}
struct Transform {
    dxm::XMFLOAT3 WORLDPOSITION;
    DirectX::XMFLOAT3 WORLDROTATION; // Change to Quartnion when understanding.
    DirectX::XMFLOAT3 WORLDSCALE;
    DirectX::XMMATRIX GetMat4() { // this is the core math. effectively what summons something in game. 
        DirectX::XMMATRIX rotatex = { 1, 0, 0, 0, 0, (float)cos(WORLDROTATION.x), -(float)sin(WORLDROTATION.x), 0, 0, (float)sin(WORLDROTATION.x), (float)cos(WORLDROTATION.x), 0, 0, 0, 0, 1 };
        DirectX::XMMATRIX rotatey = { (float)cos(WORLDROTATION.y), 0, (float)sin(WORLDROTATION.y), 0, 0, 1, 0, 0, -(float)sin(WORLDROTATION.y), 0, (float)cos(WORLDROTATION.y), 0, 0, 0, 0, 1 };
        DirectX::XMMATRIX rotatez = { (float)cos(WORLDROTATION.z), -(float)sin(WORLDROTATION.z), 0, 0, (float)sin(WORLDROTATION.z), (float)cos(WORLDROTATION.z), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
        DirectX::XMMATRIX scale = { WORLDSCALE.x, 0, 0, 0, 0, WORLDSCALE.y, 0, 0, 0, 0, WORLDSCALE.z, 0, 0, 0, 0, 1 };
        DirectX::XMMATRIX translate = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, WORLDPOSITION.x, WORLDPOSITION.y, WORLDPOSITION.z, 1 };
        return rotatex * rotatey * rotatez * scale * translate;
    }
    
};
struct Vertex {
    /*
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 position
        { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 normal
        { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float2 texcoord
        { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 color
    */
    DirectX::XMFLOAT3 VERTEX_POS; // Object Space .obj
    DirectX::XMFLOAT3 NORMAL;
    DirectX::XMFLOAT2 UVCOORD;
    DirectX::XMFLOAT3 COLOR;

    //Returns success
    static bool LoadFromFile(const char* fileName, Vertex& vertexOut) {
        //tinyobj::LoadObj()
        return true;
    }
};
struct Mesh {
    std::vector<Vertex> VertexBuffer;
    std::vector<int> IndexBuffer; // these both store certain objects.
    ID3D11Buffer* gpuVertexBuffer = nullptr;
    ID3D11Buffer* gpuIndexBuffer = nullptr;
};
struct Material {
    ID3D11ShaderResourceView* TextureSRV; // Temporary work for a Graphics Core Texture. class. Shorten to grcTexture. ****RAGE NERD****
};
struct TextureData {
    int textureWidth;
    int textureHeight;
    int MIPLevel = 1;
    void* textureData;
    DXGI_FORMAT Format;
};
int64_t JOAAT(const char* string) {
    size_t i = 0;
    uint32_t hash = 0;
    while (i != '\0') {
        hash += string[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}
class Physical { // physical representation of something
public:
    Physical(std::vector<Vertex>& vertex, Transform transform) :
        transform(transform),
        modelVertices(vertex)
    {

    }
    void SetMesh(ID3D11Device* Device, std::vector<Vertex>& Vertexdata, std::vector<int>& IndexData) {

        this->ObjectMesh.VertexBuffer = Vertexdata;
        this->ObjectMesh.IndexBuffer = IndexData;
        D3D11_BUFFER_DESC vBufferDesc = {};
        vBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        vBufferDesc.ByteWidth = sizeof(Vertex) * ObjectMesh.VertexBuffer.size();
        vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA initData{};
        initData.pSysMem = this->ObjectMesh.VertexBuffer.data();
        Device->CreateBuffer(&vBufferDesc, &initData, &this->ObjectMesh.gpuVertexBuffer);
        D3D11_BUFFER_DESC indexbufferdesc = {};
        indexbufferdesc.ByteWidth = sizeof(int) * IndexData.size();
        indexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
        indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        D3D11_SUBRESOURCE_DATA indexbufferSRD = { IndexData.data() };
        ID3D11Buffer* indexBuffer;
        Device->CreateBuffer(&indexbufferdesc, &indexbufferSRD, &this->ObjectMesh.gpuIndexBuffer);
    }
    /*
        Soo I need something more grandiose. 
        So basically issue is that grcResourceManager doesn't know about the index buffer. 
        So I need someway to sub-resource initate small data into a grcResource or also get access to a different part of it? 
        Since the hash can work issue is that I need to access different data. 
    */
    void SetMeshNew(const char* fileName, void* vertexdata, int size_vertex, int* indexData, int size_index) {

    }
    void SetTexture(ID3D11Device* Device, TextureData textureIn) {
        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = textureIn.textureWidth;
        textureDesc.Height = textureIn.textureHeight;
        textureDesc.MipLevels = 1; // im not gonna abstract these yet.
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        D3D11_SUBRESOURCE_DATA textureSRD = {};
        textureSRD.pSysMem = textureIn.textureData; // raw texture data. so if we load texture data it goes here? then what goes in SysMemPitch?
        textureSRD.SysMemPitch = textureIn.textureWidth * sizeof(UINT); // The distance from the beginning of one line to the next line? MSDN. this represents the format? im not quite sure how to modularize this yet?
        // im still not exactly sure what ^ is but ive been told its basically the image dementions * 4? I don't know lul.
        ID3D11Texture2D* texture; 
        Device->CreateTexture2D(&textureDesc, &textureSRD, &texture); // setup a global device pointer. for now just run with the input parameter.
        ID3D11ShaderResourceView* textureSRV;
        Device->CreateShaderResourceView(texture, nullptr, &textureSRV);
        this->ObjectMaterial.TextureSRV = textureSRV; // set :checkmark:
    }
    Transform transform;
    Mesh ObjectMesh;
    Material ObjectMaterial;
    std::vector<Vertex> modelVertices;
private:

};
template<typename T> class SingletonDef : protected T {
private:
    static T* sm_pInstance;
    SingletonDef() : T() {}
public:
    static T* GetInstancePtr() { assert(sm_pInstance != NULL); return this->sm_pInstance; }
    static T& GetInstance() { assert(sm_pInstance != NULL); return *this->sm_pInstance; }
    static bool DoesInstanceExist() { return this->sm_pInstance != nullptr; }
};


/*
    A Pass can have a lot of things that encompass it. Its basically an entire render context.
    Shaders,
    Input Assemblers, 
    Vertex Info ( I think idk );

*/

template<typename T>
T wrap_angle(T theta) noexcept {
    constexpr T twoPi = (T)2 * (T)PI_D;
    const T mod = (T)fmod(theta, twoPi);
    if (mod > (T)PI_D) {
        return mod - twoPi;
    } else if (mod < -(T)PI_D) {
        return mod + twoPi;
    }
    return mod;
}
// thanks overflow

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool global_Size = false;
std::vector <IDXGIAdapter*> EnumerateAdapters(void) {
    IDXGIAdapter* pAdapter;
    std::vector <IDXGIAdapter*> vAdapters;
    IDXGIFactory* pFactory = NULL;
    // Create a DXGIFactory object.
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory))) {
        return vAdapters;
    }
    for (UINT i = 0;
        pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND;
        ++i) {
        vAdapters.push_back(pAdapter);
    }
    if (pFactory) {
        pFactory->Release();
    }
    return vAdapters;

}

matrix ConvertXMMATRIXToMatrix(const DirectX::XMMATRIX& xmMatrix) {
    matrix result;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            result.m[row][col] = xmMatrix.r[row].m128_f32[col];
        }
    }
    return result;
}
double GetGlfwTimer() {
    using namespace std::chrono;
    using SecondsFP = std::chrono::duration<double>;
    return duration_cast<SecondsFP>(high_resolution_clock::now().time_since_epoch()).count();
}
/*
    Ok it seems like they don't cache? They just have a void* to ResourceData map?
    So like no idea wtf is going on?
*/
enum ResourceType {
    RT_Texture,
    RT_DSV,
    RT_RSV,
    RT_SRV,
    RT_ConstantBuffer,
    RT_VertexBuffer,
    RT_IndexBuffer,
    RT_Max
};
typedef struct grcResourceDesc {
    union {
        D3D11_TEXTURE1D_DESC Text1D;
        D3D11_TEXTURE2D_DESC Text2D;
        D3D11_TEXTURE3D_DESC Text3D;

        D3D11_BUFFER_DESC Buffer;
        D3D11_DEPTH_STENCIL_VIEW_DESC DSV;
        D3D11_RENDER_TARGET_VIEW_DESC RTV;
        D3D11_SHADER_RESOURCE_VIEW_DESC SRV;
    };
};
namespace std {
template <>
struct hash<std::pair<ResourceType, unsigned long long>> {
    size_t operator()(const std::pair<ResourceType, unsigned long long>& p) const {
        size_t hash1 = std::hash<int>()(static_cast<int>(p.first));  // Hash ResourceType (which is enum)
        size_t hash2 = std::hash<unsigned long long>()(p.second);   // Hash the unsigned long long (the hash value)
        return hash1 ^ (hash2 << 1);  // Combine the two hashes (simple example)
    }
};
}
class grcResourceCache {
    struct sResourceData;
    template<typename T, typename K>    
    using Pair = std::pair<T, K>;
    using ResourceKey = Pair<ResourceType, u64>;
    struct KeyHash {
    };
    typedef std::unordered_map<ResourceKey, sResourceData*> ActiveResources; // So idk
    static grcResourceCache* sm_Instance;
    GDevice* device;
public:
    static void InitClass(GDevice* dev) {
        sm_Instance = new grcResourceCache();
        sm_Instance->device = dev;
    }
    static grcResourceCache& GetInstance() { return *sm_Instance; }
    struct sResourceData {
        ResourceType Type;
        u32 uSize;
        grcResourceDesc Resource;
        u64 hash;
        void* outMemory;
    };
    //So lets just hash on some internal data like size of stuff. generate something similar to a unique hash cause I really don't want to take a name here in case you literally are unaware. 
    // I'll just store it in accordance to that figure. Should make it valuable in retrospect :)
    HRESULT CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture) {
        assert(pDesc != nullptr);
        u64 hash = GenHash(pDesc);
        if (resource.find({ RT_Texture, hash }) != resource.end()) {
            *ppTexture = (ID3D11Texture2D*)resource[{RT_Texture, hash}]->outMemory;
            return S_OK;
        }
        sResourceData* data = new sResourceData(); // I wish to die. haha
        data->hash = hash;
        data->Type = RT_Texture;
        device->GetDevice()->CreateTexture2D(pDesc, pInitialData, (ID3D11Texture2D**) &data->outMemory);
        data->uSize = sizeof *pDesc;
        data->Resource.Text2D = *pDesc; // uhh shitfuck?
        *ppTexture = (ID3D11Texture2D*)data->outMemory;
        Cache(data);
        return S_OK;
    }

private:
    void Cache(sResourceData* data) {
        resource[{data->Type, data->hash}] = data;
    }
    template<class ResourceDescription> u64 GenHash(const ResourceDescription* pDesc) { return 0;}
    template<> u64 GenHash<>(const D3D11_BUFFER_DESC* pDesc) {
        size_t hash = 0;
        hash ^= static_cast<size_t>(pDesc->BindFlags) * 73856093;
        hash ^= static_cast<size_t>(pDesc->CPUAccessFlags) * 19349663;
        hash ^= static_cast<size_t>(pDesc->Usage) * 83492791;
        hash ^= static_cast<size_t>(pDesc->ByteWidth) * 2654435761;
        hash ^= static_cast<size_t>(pDesc->MiscFlags) * 486187739;
        hash ^= static_cast<size_t>(pDesc->StructureByteStride) * 15485863;
        return hash;
    }
    template<> u64 GenHash<>(const D3D11_TEXTURE2D_DESC* pDesc) {
        size_t hash = 0;
        hash ^= std::hash<UINT>()(pDesc->Width) * 73856093;      // Hash the width
        hash ^= std::hash<UINT>()(pDesc->Height) * 19349663;     // Hash the height
        hash ^= std::hash<UINT>()(pDesc->MipLevels) * 83492791;  // Hash the mip levels
        hash ^= std::hash<UINT>()(pDesc->ArraySize) * 2654435761; // Hash the array size
        hash ^= std::hash<DXGI_FORMAT>()(pDesc->Format) * 486187739;  // Hash the format
        hash ^= std::hash<D3D11_USAGE>()(pDesc->Usage) * 15485863;  // Hash the usage
        hash ^= std::hash<UINT>()(pDesc->BindFlags) * 4782969;   // Hash the bind flags
        hash ^= std::hash<UINT>()(pDesc->CPUAccessFlags) * 3254069;  // Hash the CPU access flags
        hash ^= std::hash<UINT>()(pDesc->MiscFlags) * 1953095;   // Hash the misc flags
        return hash;
    }
    //void Create(sResourceData& data) {
    //    switch (data.Type) {
    //    case RT_Texture:
    //        switch (data.uSize) {
    //            case sizeof(D3D11_TEXTURE2D_DESC) :
    //                device->GetDevice()->CreateTexture2D(&data.Resource.Text2D, (D3D11_SUBRESOURCE_DATA*)data.inMemory, (ID3D11Texture2D**)&data.outMemory);
    //        }
    //    }
    //}
    ActiveResources resource;
};
grcResourceCache* grcResourceCache::sm_Instance = nullptr;
/*
* Purpose:
    This class functions more as a binding library between DirectXMath and my own stuff. 
*/
class Vector3 {
private:
    dxm::XMVECTOR m_Vector = { 0,0,0,1.0f };
public:
    Vector3(dxm::XMVECTOR vector) {
        this->m_Vector = vector;
    }
    Vector3(float x, float y, float z) {
        this->m_Vector = dxm::XMVectorSet(x, y, z, 1.0f);
    }
    Vector3 operator-(const Vector3& x) {
        return dxm::XMVectorSubtract(this->m_Vector, x.m_Vector);
    }
    Vector3 operator+(const Vector3& x) {
        return dxm::XMVectorAdd(this->m_Vector, x.m_Vector);
    }
    Vector3 operator*(const Vector3& x) {
        return dxm::XMVectorMultiply(this->m_Vector, x.m_Vector);
    }
    Vector3 operator/(const Vector3& x) {
        return dxm::XMVectorDivide(this->m_Vector, x.m_Vector);
    }
    float GetX() { return dxm::XMVectorGetX(this->m_Vector); }
    float GetY() { return dxm::XMVectorGetY(this->m_Vector); }
    float GetZ() { return dxm::XMVectorGetZ(this->m_Vector); }
    float GetW() { return dxm::XMVectorGetW(this->m_Vector); }
    dxm::XMVECTOR GetRaw() const { return this->m_Vector; }
    void SetX(float x) { this->m_Vector = dxm::XMVectorSetX(this->m_Vector, x); }
    void SetY(float x) { this->m_Vector = dxm::XMVectorSetY(this->m_Vector, x); }
    void SetZ(float x) { this->m_Vector = dxm::XMVectorSetZ(this->m_Vector, x); }
    void SetW(float x) { this->m_Vector = dxm::XMVectorSetW(this->m_Vector, x); }
    bool IsGreaterThan(const Vector3& x) { return dxm::XMVector3Greater(this->m_Vector, x.m_Vector); }
    bool IsGreaterThanOrEqualTo(const Vector3& x){ return dxm::XMVector3GreaterOrEqual(this->m_Vector, x.m_Vector); }
    bool IsLessThan(const Vector3& x) { return dxm::XMVector3Less(this->m_Vector, x.m_Vector); }
    bool IsLessThanOrEqualTo(const Vector3& x) { return dxm::XMVector3LessOrEqual(this->m_Vector, x.m_Vector); }
    Vector3 Cross(const Vector3& x) { return dxm::XMVector3Cross(this->m_Vector, x.m_Vector); }
    Vector3 Dot(const Vector3& x) { return dxm::XMVector3Dot(this->m_Vector, x.m_Vector); }
    Vector3 Scale(float scale) { return dxm::XMVectorScale(this->m_Vector, scale); }
    Vector3 Normalize() { return dxm::XMVector3Normalize(this->m_Vector); }
};
class CCamera {
public:
    CCamera(dxm::XMVECTOR position) {
        this->CameraPosition = position;
    }
    dxm::XMMATRIX GetMatrix() { return this->ViewMatrix; }
    Vector3 GetPosition() { return this->CameraPosition; }
    //dxm::XMVECTOR GetDirection() { return this->cameraDirection; }
    void Update() {
        this->ViewMatrix = DirectX::XMMatrixLookAtLH(CameraPosition.GetRaw(), (CameraPosition + CameraFront).GetRaw(), {0.0,1.0,0.0});
        const float cameraSpeed = 0.005f;
        if (GetAsyncKeyState('W') & 0x8000) {
            CameraPosition = CameraPosition + (CameraFront.Scale(cameraSpeed)).GetRaw();
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            CameraPosition = CameraPosition - (CameraFront.Scale(cameraSpeed)).GetRaw();
        }
        if (GetAsyncKeyState('A') & 0x8000) {
            CameraPosition = CameraPosition + (CameraFront.Cross({ 0.0,1.0,0.0 }).Normalize().Scale(cameraSpeed));
        }
        if (GetAsyncKeyState('D') & 0x8000) {
            CameraPosition = CameraPosition - (CameraFront.Cross({ 0.0,1.0,0.0 }).Normalize().Scale(cameraSpeed));
        }
        ImGui::Begin("Camera Rotation");
        //yaw = wrap_angle(yaw + mouse.GetMouseX() * 0.005f); // 0.005f sensativity. change
        //pitch = std::clamp(pitch + mouse.GetMouseY() * 0.005f, -PI / 2.0f, PI / 2.0f); // 0.005f sensativity. change
        //ImGui::Text("Delta : (%f, %f)", mouse.GetMouseX(), mouse.GetMouseY());
        //ImGui::Text("X(Yaw): %f; Y(Pitch): %f", yaw, pitch);
        ImGui::SliderFloat("Yaw", &yaw, -360, 360);
        ImGui::SliderFloat("Pitch", &pitch, -360, 360);

        ImGui::End();

        if (pitch > 89.f) {
            pitch = 89.f;
        }
        if (pitch < -89.f) {
            pitch = -89.0f;
        }
        DirectX::XMVECTOR tempFront;
        tempFront.m128_f32[0] = cos(DirectX::XMConvertToRadians(yaw)) * cos(DirectX::XMConvertToRadians(pitch));//x
        tempFront.m128_f32[1] = sin(DirectX::XMConvertToRadians(pitch));//y
        tempFront.m128_f32[2] = sin(DirectX::XMConvertToRadians(yaw)) * cos(DirectX::XMConvertToRadians(pitch));//z
        CameraFront = DirectX::XMVector3Normalize(tempFront);
    }

private:
    Vector3 CameraPosition = { 0.f,0.f,3.0f };
    Vector3 CameraFront = { 0.0,0.0,-1.0f };
    DirectX::XMMATRIX ViewMatrix;
    float fov = 45.f;
    float min, max;
    float yaw = -00, pitch =0 ;

    /*
        DirectX::XMVECTOR cameraPosition = { 0.f,0.f,3.0f };
        DirectX::XMVECTOR cameraTarget = { 0.0f,0.0f,4.0f };
        DirectX::XMVECTOR cameraFront = { 0.0,0.0,-1.0f };
        DirectX::XMVECTOR cameraDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(cameraPosition, cameraTarget));
        DirectX::XMVECTOR UP = { 0.0,1.0,0.0 };
        DirectX::XMVECTOR cameraRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(UP, cameraDirection));
        DirectX::XMVECTOR cameraUp = { 0.0,1.0,0.0 };
        float yaw = -90.f;
        float pitch = 0.0f;
        float lastX = 800.f / 2.0f;
        float lastY = 600.f / 2.0f;
        float fov = 45.0f; // oh for fuck sake.
    */
};
template<typename T>
class grcVertexBuffer {
public:
    grcVertexBuffer(GDevice& device) {
        this->device = device;
    }
    void Init(const std::vector<T>& data) {
        stride = sizeof T;
        count = data.size();
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.ByteWidth = stride * count;
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = data.data();
        device.GetDevice()->CreateBuffer(&desc, &initData, &this->m_pGPUBuffer);
    }
    void Bind() {
        UINT offset = 0;
        device.GetContext()->IASetVertexBuffers(0,1,this->m_pGPUBuffer, &stride, &offset);
    }

private:
    GDevice& device;
    ID3D11Buffer* m_pGPUBuffer;
    UINT stride;
    UINT count;
};
class grcIndexBuffer {
public:
    grcIndexBuffer(GDevice& device) {
        this->device = device;
    }
    void Init(const std::vector<uint32_t>& data) {
        stride = sizeof uint32_t;
        count = data.size();
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER; 
        desc.ByteWidth = stride * count;
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = data.data();
        device.GetDevice()->CreateBuffer(&desc, &initData, &this->m_pGPUBuffer);
    }
    void Bind() {
        UINT offset = 0;
        device.GetContext()->IASetVertexBuffers(0, 1, this->m_pGPUBuffer, &stride, &offset);
    }

private:
    GDevice& device;
    ID3D11Buffer* m_pGPUBuffer;
    UINT stride;
    UINT count;
};
class Geometry {
public:
    void ShootMe(std::vector<Vertex>& crossbowmynuts, std::vector<uint32_t> indices) {
        this->m_pVertexBuffer->Init(crossbowmynuts);
        this->m_pIndexBuffer->Init(indices);
    }
private:
    grcVertexBuffer<Vertex>* m_pVertexBuffer; // @TODO: diff the memory pattern from the memory itself :)
    grcIndexBuffer* m_pIndexBuffer;
};
class fwMesh {
public:
    void Bind() {
        
    }
private:
    Geometry* m_pGeo;
    int numGeo;
};
class fwShader {
public:
    enum eShaderType {
        Vertex,
        Pixel
    };
    virtual void Bind() = 0;
    eShaderType GetShaderType() { return this->ShaderType; }
protected:
    fwShader(eShaderType type) {
        this->ShaderType = type;
    }
    eShaderType ShaderType;
};
class grcVertexShader : public fwShader{ // listen I've been just going back and forth I've decided just to write. And suffer if I can't make it. you gotta do it regardless. Just bite the bullet. 
public:
    grcVertexShader() : fwShader(Vertex){}
    void Bind() { 
    
    } // take a render context or get one from global scope you idiot.
private:
    ID3D11VertexShader* m_pRawShader;
};
class fwShaderGroup {
public:
    fwShaderGroup() {}
    void SetShader(fwShader* pShader) {
        switch (pShader->GetShaderType()) {
        case fwShader::Vertex:
            this->m_pVertexShader = pShader;
            break;
        case fwShader::Pixel:
            this->m_pPixelShader = pShader;
            break;
        }
    }
    fwShader* GetVertexShader() { return this->m_pVertexShader; }
    fwShader* GetPixelShader() { return this->m_pPixelShader; }
private:
    fwShader* m_pVertexShader = nullptr;
    fwShader* m_pPixelShader = nullptr;
};
class grcDrawable {
public:
    grcDrawable(fwMesh* msh, fwShaderGroup* pShader) {
        this->m_pMesh = msh;
        this->m_pShaders = pShader; // fuckshit
    } 
    void SetShader(fwShaderGroup* pShader) {
        this->m_pShaders = pShader; // i don't care if pShader is null :)
    }
    void SetupDraw() {
        
    }
private:
    fwRenderContext* m_pRenderContext;
    fwMesh* m_pMesh;
    fwShaderGroup* m_pShaders;
};

class fwRenderContext {
public:
    enum eFormat { // @TODO Add more data types as necessary. It's gonna be a pain at first cause its basically data transfusion. Maybe do a bit of automation?  
        FMT_R32_UINT,
    };
    virtual void SetInputAssembly(ID3D11InputLayout* pLayout) = 0; // @TODO Remove ID3D11InputLayout base implementation. 
    virtual void SetVertexBuffers(unsigned int StartSlot, unsigned int NumBuffers, void** ppVertexBuffers, const unsigned int* pStrides, const unsigned int* pOffsets) = 0;
    virtual void SetIndexBuffer(void* pIndexBuffer, eFormat Format, unsigned int offset) = 0; // abstraction yay!
    virtual void DrawIndexed(unsigned int indexcount, unsigned int startindexloc, int basevertexlocation) = 0;
};
template<typename ConversionType>
class fwFormatMappings {
public:
    virtual ConversionType TranslateFormat(fwRenderContext::eFormat format) = 0;
protected:
    std::map<fwRenderContext::eFormat, ConversionType> m_FormatMap;
};
class grcD3D11FormatMappings : public fwFormatMappings<DXGI_FORMAT> { 
public:
    void Init() {
        this->m_FormatMap.insert({ fwRenderContext::eFormat::FMT_R32_UINT, DXGI_FORMAT::DXGI_FORMAT_R32_UINT }); // this is a bunch of shit. regardless of your opinion :L.
    }
    DXGI_FORMAT TranslateFormat(fwRenderContext::eFormat format) {
        return this->m_FormatMap.at(format);
    }
    static grcD3D11FormatMappings& GetInstance() {
        assert(sm_pFormatMapperDXGI != nullptr);
        return *sm_pFormatMapperDXGI;
    }
private:
    grcD3D11FormatMappings() {
        this->Init();
    }
    static grcD3D11FormatMappings* sm_pFormatMapperDXGI;
};
grcD3D11FormatMappings* grcD3D11FormatMappings::sm_pFormatMapperDXGI = new grcD3D11FormatMappings();
class grcD3D11RenderContext : public fwRenderContext{
public:
    grcD3D11RenderContext() {}
    void SetInputAssembly(ID3D11InputLayout* pLayout) {
        this->GetBasisContext()->IASetInputLayout(pLayout);
    }  
    void SetVertexBuffers(unsigned int StartSlot, unsigned int NumBuffers, void* const * ppVertexBuffers, const unsigned int* pStrides, const unsigned int* pOffsets) {
        this->GetBasisContext()->IASetVertexBuffers(StartSlot, NumBuffers, (ID3D11Buffer* const *)ppVertexBuffers, pStrides, pOffsets); // nice beautiful dirty cast in the middle of the argument :|
    }
    void SetIndexBuffer(void* pIndexBuffer, eFormat Format, unsigned int offset) {
        this->GetBasisContext()->IASetIndexBuffer((ID3D11Buffer*)pIndexBuffer, grcD3D11FormatMappings::GetInstance().TranslateFormat(Format), offset);
    } 
    void DrawIndexed(unsigned int indexcount, unsigned int startindexloc, int basevertexlocation) override {
        this->GetBasisContext()->DrawIndexed(indexcount, startindexloc, basevertexlocation);
    }
    ID3D11DeviceContext* GetBasisContext() { return this->m_pDeviceContext; }
private:
    ID3D11DeviceContext* m_pDeviceContext;
};

class grcPass {
public:
    grcPass(std::vector<grcDrawable*>& ppDrawables) {
        this->m_ppDrawables = ppDrawables;
    }
    void Render() {
        for (auto* draw : m_ppDrawables) {
            draw->SetupDraw();
            m_pRenderContext->DrawIndexed();
        }
    }
private:
    fwRenderContext* m_pRenderContext;
    std::vector<grcDrawable*> m_ppDrawables;
};
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    int cmdcount = 0;    LPWSTR* cmdArgs = CommandLineToArgvW(pCmdLine, &cmdcount);
    const wchar_t CLASSNAME[] = L"Sample Window Class";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASSNAME;
    RegisterClass(&wc);
    HWND hwnd = CreateWindowExW(0, CLASSNAME, L"LEGIT ADVANCED GRAPHICS ENGINE(L.A.G.E)", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL) {
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);
    GDevice device = GDevice();
    device.Init(hwnd);
    
    HRESULT hr;
    //Depth Buffer
    D3D11_TEXTURE2D_DESC depthbufferdesc;
    device.GetFrameBuffer()->GetDesc(&depthbufferdesc);
    depthbufferdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthbufferdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    ID3D11Texture2D* depthBuffer;
    device.GetDevice()->CreateTexture2D(&depthbufferdesc, nullptr, &depthBuffer);
    ID3D11DepthStencilView* depthbufferDSV;
    device.GetDevice()->CreateDepthStencilView(depthBuffer, nullptr, &depthbufferDSV);
    //Vertex shader
    ID3DBlob* vertexshaderCSO = NULL;
    //COMPILE VERTEX SHADER
    D3DCompileFromFile(L"gpu.hlsl", nullptr, nullptr, "VertexShaderMain", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);
    ID3D11VertexShader* vertexShader;
    device.GetDevice()->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &vertexShader);
    //Input. This coresponds to fields in the shader. Think of these like bindings for the way shit interacts. basically POS is not world space its coordinate space. This corresponds to the model's stuff..
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 position
        { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 normal
        { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float2 texcoord
        { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 color im gonna default tne alignment
    };
    ID3D11InputLayout* inputLayout = NULL;
    hr = device.GetDevice()->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &inputLayout);
    assert(SUCCEEDED(hr));
    ID3DBlob* pixelshaderCSO;
    D3DCompileFromFile(L"gpu.hlsl", nullptr, nullptr, "PixelShaderMain", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);
    ID3D11PixelShader* pixelShader;
    device.GetDevice()->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &pixelShader);
    //Rasterization steps this allows for you to access wire-frames
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    ID3D11RasterizerState* rasterizerState;
    device.GetDevice()->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    //Texture Sampling settings
    D3D11_SAMPLER_DESC samplerdesc = {};
    samplerdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    ID3D11SamplerState* samplerstate; // The sampler object. (This gets passed to the shader) 
    device.GetDevice()->CreateSamplerState(&samplerdesc, &samplerstate);
    //Depth Stencil ( actually adding depth so stuff behind objects doesn't get drawn infront of other objects ) 
    D3D11_DEPTH_STENCIL_DESC depthstencildesc = {};
    depthstencildesc.DepthEnable = TRUE;
    depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS;
    ID3D11DepthStencilState* depthstencilstate;
    device.GetDevice()->CreateDepthStencilState(&depthstencildesc, &depthstencilstate);
     //Constant data
    struct Constants { DirectX::XMMATRIX transform, view, projection; DirectX::XMFLOAT3 lightvector; };
    D3D11_BUFFER_DESC constantBufferDesc = {};
    constantBufferDesc.ByteWidth = sizeof(Constants) + 0xf & 0xfffffff0;
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    grcBuffer constants = grcBuffer(&constantBufferDesc, nullptr, &device);
    //Texture.
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = TEXTURE_WIDTH;
    textureDesc.Height = TEXTURE_HEIGHT;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    D3D11_SUBRESOURCE_DATA textureSRD = {};
    textureSRD.pSysMem = texturedata; // raw texture data. so if we load texture data it goes here? then what goes in SysMemPitch?
    textureSRD.SysMemPitch = TEXTURE_WIDTH * sizeof(UINT); // The distance from the beginning of one line to the next line? MSDN. this represents the format? so like 0x00000000 means what color channels? since there is 8 numbers at play here this divides them?
    ID3D11Texture2D* texture;
    device.GetDevice()->CreateTexture2D(&textureDesc, &textureSRD, &texture); 
    ID3D11ShaderResourceView* textureSRV;
    device.GetDevice()->CreateShaderResourceView(texture, nullptr, &textureSRV);
    int width = 0, height = 0, channels = 0;
    
    //Putting the model in memory
    DirectX::XMFLOAT3 modelrotation = { 0.0f,0.0f,0.0f };
    DirectX::XMFLOAT3 modelscale = { 1.0f,1.0f,1.0f };
    DirectX::XMFLOAT3 modeltranslation = { 0.0f,0.0f,0.0f };
    std::vector<float> raw_vertices(std::begin(vertexdata), std::end(vertexdata));
    std::vector<Vertex> vertices(raw_vertices.size());
    size_t index = 0;
    for (int i = 0; i < raw_vertices.size()/11; i++) { // 11 numbers 3 + 2 + 3 + 3
        const float* ptr = &raw_vertices[i];
        Vertex vertex{};
        vertex.VERTEX_POS = { raw_vertices[index], raw_vertices[index + 1], raw_vertices[index + 2] };
        index += 3;
        vertex.NORMAL = { raw_vertices[index], raw_vertices[index + 1], raw_vertices[index + 2] };
        index += 3;
        vertex.UVCOORD = { raw_vertices[index], raw_vertices[index + 1] };
        index += 2;
        vertex.COLOR = { raw_vertices[index], raw_vertices[index + 1], raw_vertices[index + 2] };
        index += 3;
        vertices.push_back(vertex);
    }
    //cube vertices. 
    DirectX::XMFLOAT3 CUBEPOINTS[8] =
    {
        DirectX::XMFLOAT3(-1, -1, -1),
        DirectX::XMFLOAT3(1, -1, -1),
        DirectX::XMFLOAT3(1, 1, -1),
        DirectX::XMFLOAT3(-1, 1, -1),
        DirectX::XMFLOAT3(-1, -1, 1),
        DirectX::XMFLOAT3(1, -1, 1),
        DirectX::XMFLOAT3(1, 1, 1),
        DirectX::XMFLOAT3(-1, 1, 1)
    };
    DirectX::XMFLOAT2 texCoords[4] =
    {
        DirectX::XMFLOAT2(0, 0),
        DirectX::XMFLOAT2(1, 0),
        DirectX::XMFLOAT2(1, 1),
        DirectX::XMFLOAT2(0, 1)
    };
    //for (int i = 0; i < raw_vertices.size() / 11; i++) { // 11 numbers 3 + 2 + 3 + 3
    //    const float* ptr = &raw_vertices[i];
    //    Vertex vertex{};
    //    vertex.VERTEX_POS = { raw_vertices[index], raw_vertices[index + 1], raw_vertices[index + 2] };
    //    index += 3;
    //    vertex.NORMAL = { raw_vertices[index], raw_vertices[index + 1], raw_vertices[index + 2] };
    //    index += 3;
    //    vertex.UVCOORD = { raw_vertices[index], raw_vertices[index + 1] };
    //    index += 2;
    //    vertex.COLOR = { raw_vertices[index], raw_vertices[index + 1], raw_vertices[index + 2] };
    //    index += 3;
    //    vertices.push_back(vertex);
    //}
    CCamera camera = CCamera({0,0,0,0});
    Physical object1 = Physical(vertices, { modeltranslation, modelrotation, modelscale });
    modeltranslation = { -3.0,0,0 };
    Physical object2 = Physical(vertices, { modeltranslation, modelrotation, modelscale });
    modeltranslation = { -6.0,0,0 };
    auto* pixels = stbi_load("W:\\Engine\\RAGE\\RAGE\\tex\\mini_map_2d.png", &width, &height, &channels, STBI_rgb_alpha);
    TextureData tData = {};
    tData.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    tData.MIPLevel = 1;
    tData.textureData = pixels;
    tData.textureHeight = height;
    tData.textureWidth = width;
    object1.SetTexture(device.GetDevice(), tData);
    tData.textureData = texturedata;
    tData.textureHeight = TEXTURE_HEIGHT;
    tData.textureWidth = TEXTURE_WIDTH;
    object2.SetTexture(device.GetDevice(), tData);
    std::vector<int> indices = { std::begin(indexdata), std::end(indexdata) };
    object2.SetMesh(device.GetDevice(), vertices, indices);
    object1.SetMesh(device.GetDevice(), vertices, indices);

    /*Physical object3 = Physical(, {modeltranslation, modelrotation, modelscale});*/
    //Assimp::Importer imp;
    //auto model = imp.ReadFile("W:\\Engine\\D3D11Test\\D3D11Test\\Models\\teapot.obj", aiProcess_Triangulate | aiProcess_GenUVCoords);
    //const auto pMesh = model->mMeshes[0];
    //
    //std::vector<Vertex> rahhhhhWannn;
    //rahhhhhWannn.reserve((pMesh->mNumVertices));
    ////maybe data isn't the same stride when using this method? I don't quite know how to debug this one? Maybe just flood the vector with data?
    //int selectedTextureCoordinateLookup = -1;
    //for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++) {
    //    if (pMesh->HasTextureCoords(i)) {
    //        selectedTextureCoordinateLookup = i;
    //    }
    //}
    //if (selectedTextureCoordinateLookup == -1) {
    //    assert(false && "Selected Texture Coordinate is still -1! Shit!");
    //}
    const float scalef = 1.f;
    //for (int i = 0; i < pMesh->mNumVertices; i++) {
    //    Vertex vertex{};
    //    vertex.VERTEX_POS = { pMesh->mVertices[i].x * scalef, pMesh->mVertices[i].y * scalef, pMesh->mVertices[i].z * scalef, }; // 1.0f represents scale
    //    if (pMesh->mTextureCoords[0]) {
    //        vertex.UVCOORD = { pMesh->mTextureCoords[0][i].x,pMesh->mTextureCoords[0][i].y }; // 1.0f represents scale
    //    }
    //    int meshData = model->mNumTextures;
    //    int colorData = pMesh->GetNumColorChannels();
    //    
    //    //assert(false && meshData);
    //    if (pMesh->HasNormals()) {
    //        vertex.NORMAL = { pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z };
    //    }
    //    vertex.COLOR = { 0.5f,0.5f,0.5f };  // 1.0f represents scale
    //    rahhhhhWannn.push_back(vertex); // fuck me sideways.
    //    
    //}
    //std::vector<int> MODELINDICES(pMesh->mNumFaces * 3);
    //for (int i = 0; i < pMesh->mNumFaces; i++) {
    //    const auto& face = pMesh->mFaces[i];
    //    assert(face.mNumIndices == 3);
    //    MODELINDICES.push_back(face.mIndices[0]);
    //    MODELINDICES.push_back(face.mIndices[1]);
    //    MODELINDICES.push_back(face.mIndices[2]);
    //    
    //}
    ////vertex is filled with zeros and no matter what I do can't seem to get it to work? for loop isn't skipped? what is causing this discrepancy.
    //object2.SetMesh(device.GetDevice(), rahhhhhWannn, MODELINDICES);
    //object2.SetTexture(device.GetDevice(), tData);
    auto ret = CreateD3DBuffer(device.GetDevice(), vertexdata, sizeof(vertexdata) / sizeof(vertexdata[0]), indexdata, sizeof(indexdata) / sizeof(indexdata[0]));
    object1.ObjectMesh.gpuVertexBuffer = ret.first;
    object1.ObjectMesh.gpuIndexBuffer= ret.second;
    object2.ObjectMesh.gpuVertexBuffer = ret.first;
    object2.ObjectMesh.gpuIndexBuffer = ret.second;
    // are buffers being setup properly? because the gpu doesn't get data if so?
    std::vector<Physical*> Objects;
    Objects.push_back(&object1);
    Objects.push_back(&object2);
    //Clear color
    FLOAT clearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };

    UINT stride = 11 * sizeof(float); // vertex size (11 floats: float3 position, float3 normal, float2 texcoord, float3 color)
    UINT offset = 0;

    D3D11_VIEWPORT viewport = { 0.f,0.f,(float)device.GetSwapChainDescriptor()->BufferDesc.Width, (float)device.GetSwapChainDescriptor()->BufferDesc.Height, 0.f,1.0f};
    float w = viewport.Width / viewport.Height;//fucking ratio
    float h = 1.0f; // idk
    float n = 1.0f; // near plane
    float f = 20.f; // far plane


    auto lastTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    double elapsedTime = 0.0;
    const double targetInterval = 1.0; // Update FPS every 1 second
    double deltaTime = 0.0;
    DirectX::XMFLOAT3 actualPos = { 0.f,0.f,-5.0f };
    DirectX::XMFLOAT3 actualTarget = { 0.f,0.f,4.f };
    DirectX::XMFLOAT3 actualup = { 0.0f,1.0f, 0.0f };
    
    {//IMGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(device.GetDevice(), device.GetContext());
    }
    MSG msg = {};
    bool should_close = false;
    float fText = 0.0f;
    grcResourceCache::InitClass(&device);
    


    const float radius = 10.0f;
    ID3D11Texture2D*     anotherrendertarget;
    D3D11_TEXTURE2D_DESC testDesc = {};
    testDesc.Width = 2048;
    testDesc.Height = 2048;
    testDesc.MipLevels = 1;
    testDesc.ArraySize = 1;
    testDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    testDesc.SampleDesc.Count = 1;
    testDesc.Usage = D3D11_USAGE_DEFAULT;
    testDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
    grcResourceCache::GetInstance().CreateTexture2D(&testDesc, nullptr, &anotherrendertarget);
    D3D11_RENDER_TARGET_VIEW_DESC desc234{};
    desc234.Format = DXGI_FORMAT_R32_FLOAT;
    desc234.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    ID3D11RenderTargetView* anotherrendertargetview;
    device.GetDevice()->CreateRenderTargetView(anotherrendertarget, &desc234, &anotherrendertargetview);

    while (!should_close) {
        grcResourceCache::GetInstance().CreateTexture2D(&testDesc, nullptr, &anotherrendertarget);
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        { // Time Caluclations
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> frameTime = currentTime - lastTime;
            deltaTime = frameTime.count();

            lastTime = currentTime;
            // Update elapsed time and frame count
            elapsedTime += frameTime.count();
            frameCount++;
            double fps = 0;
            // Calculate FPS
            fps = frameCount / elapsedTime;
            // Reset counters
            elapsedTime = 0.0;
            frameCount = 0;
            ImGui::Begin("Performance");
            ImGui::Text("Time Data");
            ImGui::Text("Delta: %f", deltaTime);
            ImGui::Text("FPS: %lf", fps);
            ImGui::Text("Elapsed: %f", elapsedTime);
            ImGui::Text("Current time: %lf", frameTime);
            ImGui::End(); 
        }



        /**** handle user input and other window events ****/
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT) { break; }
        { // obj movement
            //modelrotation.x += 0.5f * deltaTime;
            //modelrotation.y += 0.9f * deltaTime;
            //modelrotation.z += 0.1f * deltaTime;
        }
        {
            //float camX = static_cast<float>(sin(GetGlfwTimer()) * radius);
            //float camZ = static_cast<float>(cos(GetGlfwTimer()) * radius);
            //if (ImGui::Begin("Name")) {
            //    ImGui::Text("%f, %f, %f: Camera Positional Data", camX, 0.0, camZ);
            //    float v23[3] = {modeltranslation.x, modeltranslation.y, modeltranslation.z};
            //    ImGui::InputFloat("Object Positional Data", &modeltranslation.x);
            //    ImGui::End();
            //}

            camera.Update();
        }
        {
            //mouse.ShowRawInputWindow();
        }
        { // basic input methods. Broken currently.

        }

        device.GetContext()->ClearRenderTargetView(device.GetRenderTarget(), clearColor);
        device.GetContext()->ClearDepthStencilView(depthbufferDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
        for(auto& obj : Objects)
        { /*** RENDER A FRAME ***/

            device.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            device.GetContext()->IASetInputLayout(inputLayout); 
            device.GetContext()->IASetVertexBuffers(0, 1, &obj->ObjectMesh.gpuVertexBuffer, &stride, &offset); //  i need buffers allocated on the gpu? not sure where to do that yet if it happens when we allocate stuff or something else.
            device.GetContext()->IASetIndexBuffer(obj->ObjectMesh.gpuIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

            device.GetContext()->VSSetShader(vertexShader, nullptr, 0);
            device.GetContext()->VSSetConstantBuffers(0, 1, constants.GetBufferPtr()); // since its supposed to be a collection it needs this I think :0

            device.GetContext()->RSSetViewports(1, &viewport);
            device.GetContext()->RSSetState(rasterizerState);

            device.GetContext()->PSSetShader(pixelShader, nullptr, 0);
            device.GetContext()->PSSetShaderResources(0, 1, &obj->ObjectMaterial.TextureSRV);
            device.GetContext()->PSSetSamplers(0, 1, &samplerstate);

            device.GetContext()->OMSetRenderTargets(1, device.GetRenderTargets(), depthbufferDSV);
            device.GetContext()->OMSetDepthStencilState(depthstencilstate, 0);
            device.GetContext()->OMSetBlendState(nullptr, nullptr, 0xffffffff);

            D3D11_MAPPED_SUBRESOURCE constantBufferMSR;
            device.GetContext()->Map(constants.GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &constantBufferMSR);
            {
                Constants* constants = (Constants*)constantBufferMSR.pData;
                
                constants->transform = obj->transform.GetMat4();
                constants->projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), viewport.Width / viewport.Height, 0.1, 100.f);

                constants->view = camera.GetMatrix();
                constants->lightvector = { 1.0f, -1.0f, 1.0f };
            }
            device.GetContext()->Unmap(constants.GetBuffer(), 0);

            device.GetContext()->DrawIndexed(obj->ObjectMesh.IndexBuffer.size(), 0, 0);


        } // end of frame
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        device.GetSwapChain()->Present(1, 0);



    } // end of main loop
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        RAWINPUTDEVICE rID;
        rID.usUsagePage = 0x01;
        rID.usUsage = 0x02;
        rID.dwFlags = 0;
        rID.hwndTarget = nullptr;
        if (!RegisterRawInputDevices(&rID, 1, sizeof(rID))) {
            throw std::runtime_error("Microsoftt!!!!!");
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        global_Size = true;
        break;
    case WM_GETMINMAXINFO: {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 800;
        lpMMI->ptMinTrackSize.y = 600;
        break;
    };
    case WM_MOUSEMOVE: {
        POINTS pt = MAKEPOINTS(lParam);

        break;
      }
                        
    case WM_INPUT: {
        UINT size;
        if (GetRawInputData(
            reinterpret_cast<HRAWINPUT>(lParam),
            RID_INPUT,
            nullptr,
            &size,
            sizeof RAWINPUTHEADER ) == -1) {
            //bail message shit fuck
            break;
        }
        rawBuffer.resize(size);
        if (GetRawInputData(
            reinterpret_cast<HRAWINPUT>(lParam),
            RID_INPUT,
            rawBuffer.data(),
            &size,
            sizeof RAWINPUTHEADER) != size) {
            //bail message shit fuck
            break;
        }
        auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
        if (ri.header.dwType == RIM_TYPEMOUSE && (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0)) {
            //mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
        }
        break;
    }
        
        return 0;
    }
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        return true;
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

matrix operator*(const matrix& m1, const matrix& m2) {
    return
    {
        m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0],
        m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1],
        m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2],
        m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3],
        m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0],
        m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1],
        m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2],
        m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3],
        m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0],
        m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1],
        m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2],
        m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3],
        m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0],
        m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1],
        m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2],
        m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3],
    };
}

