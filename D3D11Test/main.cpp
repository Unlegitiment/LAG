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
/*
    Ok jesus christ. We've finally figured out textures. which means we've basically done looked into everything in terms of how stuff is working
    Views are just different metadata for buffers/textures. 
    Textures and Buffers are synonomous one just on the surface means an Image whilst the other means a Buffer of data. 
    Views allow you to have multiple inputs and outputs to textures. which explains that we need a rendertargetview to the backbuffer.
*/

class grcDevice {
public:
    enum ShaderStages {
        Vertex,
        Hull,
        Compute,
        Pixel,
    };
    void Init();
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
enum eShaderType {
    VERTEX,
    PIXEL,
    MAX_SHADER_TYPES
};

class fwShader {
public:
    fwShader();
    eShaderType GetShaderType() { return this->m_iShaderType; }
    virtual void Bind() = 0; // 
protected:
    eShaderType m_iShaderType;
};
class fwVertexShader : public fwShader{
public:
    fwVertexShader(ID3D11VertexShader* shader) {
        this->m_ShaderInstance = shader;
        this->m_iShaderType = VERTEX;
    }
    void Bind() {
        GRCDEVICE->GetContext()->VSSetShader(this->m_ShaderInstance, nullptr, 0);
    }
private:
    ID3D11VertexShader* m_ShaderInstance;
};
class fwPixelShader : public fwShader {
public:
    fwPixelShader(ID3D11PixelShader* shader) {
        this->m_ShaderInstance = shader;
        this->m_iShaderType = PIXEL
    }
    void Bind() {
        GRCDEVICE->GetContext()->PSSetShader(this->m_ShaderInstance, nullptr, 0);
    }
private:
    ID3D11PixelShader* m_ShaderInstance;
};
template<typename T> class SingletonDef : protected T{
private:
    static T* sm_pInstance;
    SingletonDef() : T() {}
public:
    static T* GetInstancePtr() { assert(sm_pInstance != NULL) return this->sm_pInstance; }
    static T& GetInstance() { assert(sm_pInstance != NULL) return *this->sm_pInstance; }
    static bool DoesInstanceExist() { return this->sm_pInstance != nullptr; }
};
/*
    A Pass can have a lot of things that encompass it. Its basically an entire render context.
    Shaders,
    Input Assemblers, 
    Vertex Info ( I think idk );

*/
class grcPass {
public:
    void SetShader(fwShader* shader) {
        m_Shaders[shader->GetShaderType()] = shader;
    }
private:
    fwShader* m_Shaders[MAX_SHADER_TYPES];
};

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
#include "device.h"
using namespace lage;
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
        { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 color
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
    ID3D11Buffer* constantBuffer;
    device.GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
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
    float fov = 45.0f;
    DirectX::XMMATRIX VIEW;
    VIEW = DirectX::XMMatrixLookAtLH(cameraPosition, DirectX::XMVectorAdd(cameraPosition, cameraFront), cameraUp);
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
    device.GetDevice()->CreateTexture2D(&testDesc, nullptr, &anotherrendertarget);
    D3D11_RENDER_TARGET_VIEW_DESC desc234{};
    desc234.Format = DXGI_FORMAT_R32_FLOAT;
    desc234.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    ID3D11RenderTargetView* anotherrendertargetview;
    device.GetDevice()->CreateRenderTargetView(anotherrendertarget, &desc234, &anotherrendertargetview);

    while (!should_close) {
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

            VIEW = DirectX::XMMatrixLookAtLH(cameraPosition, DirectX::XMVectorAdd(cameraPosition, cameraFront), cameraUp);
        }
        {
            //mouse.ShowRawInputWindow();
        }
        { // basic input methods. Broken currently.
            const float cameraSpeed = 0.005f;
            if (GetAsyncKeyState('W') & 0x8000) {
                cameraPosition = DirectX::XMVectorAdd(cameraPosition, DirectX::XMVectorScale(cameraFront, cameraSpeed));
            }
            if (GetAsyncKeyState('S') & 0x8000) {
                cameraPosition = DirectX::XMVectorSubtract(cameraPosition, DirectX::XMVectorScale(cameraFront, cameraSpeed));
            }
            if (GetAsyncKeyState('A') & 0x8000) {
                cameraPosition = DirectX::XMVectorAdd(cameraPosition, DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMVector3Cross(cameraFront, cameraUp)), cameraSpeed));
            }
            if (GetAsyncKeyState('D') & 0x8000) {
                cameraPosition = DirectX::XMVectorSubtract(cameraPosition, DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMVector3Cross(cameraFront, cameraUp)), cameraSpeed));
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
            cameraFront = DirectX::XMVector3Normalize(tempFront);
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
            device.GetContext()->VSSetConstantBuffers(0, 1, &constantBuffer);

            device.GetContext()->RSSetViewports(1, &viewport);
            device.GetContext()->RSSetState(rasterizerState);

            device.GetContext()->PSSetShader(pixelShader, nullptr, 0);
            device.GetContext()->PSSetShaderResources(0, 1, &obj->ObjectMaterial.TextureSRV);
            device.GetContext()->PSSetSamplers(0, 1, &samplerstate);

            device.GetContext()->OMSetRenderTargets(1, device.GetRenderTargets(), depthbufferDSV);
            device.GetContext()->OMSetDepthStencilState(depthstencilstate, 0);
            device.GetContext()->OMSetBlendState(nullptr, nullptr, 0xffffffff);

            D3D11_MAPPED_SUBRESOURCE constantBufferMSR;
            device.GetContext()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantBufferMSR);
            {
                Constants* constants = (Constants*)constantBufferMSR.pData;
                
                constants->transform = obj->transform.GetMat4();
                constants->projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), viewport.Width / viewport.Height, 0.1, 100.f);

                constants->view = VIEW;
                constants->lightvector = { 1.0f, -1.0f, 1.0f };
            }
            device.GetContext()->Unmap(constantBuffer, 0);

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

