#pragma once
#include <d3dcompiler.h>
#include <cassert>
#include "device\device.h"
typedef wchar_t wchar;
enum eShaderType {
	INVALID,
	VERTEX,
	PIXEL
};
class grcShaderBase {
public:
	grcShaderBase(eShaderType shader) : m_ShaderType(shader) {

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
class grcShaderVertex : public grcShaderBase {
public:
	grcShaderVertex(const wchar* path, const char* entry) : grcShaderBase(VERTEX) {
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