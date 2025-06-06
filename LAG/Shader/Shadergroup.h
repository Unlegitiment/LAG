#pragma once
#include "Shader.h"
#include <unordered_map>
#include "Buffer\Buffer.h"
class grcCBuffer {
public:
	grcCBuffer(void* data, size_t size) {
		m_pBuffer = new grcBufferD3D(D3D11_BIND_CONSTANT_BUFFER, data, (UINT)size);
	}
	void Update(void* data, size_t size) {
		D3D11_MAPPED_SUBRESOURCE mapped = {};
		HRESULT hr = grcDeviced3d::Get()->context->Map(m_pBuffer->GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		assert(SUCCEEDED(hr));
		memcpy(mapped.pData, data, size);
		grcDeviced3d::Get()->context->Unmap(m_pBuffer->GetBuffer(), 0); // :p
	};
	grcBufferD3D* GetBuffer() {
		return m_pBuffer;
	}
	ID3D11Buffer* GetBufferPtr() {
		return m_pBuffer->GetBuffer();
	}
	~grcCBuffer() {
		delete m_pBuffer;
		m_pBuffer = nullptr;
	}
private:
	grcBufferD3D* m_pBuffer = nullptr;
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

		for (auto& [slot, buf] : m_pBuffers) {
			grcDeviced3d::Get()->context->VSSetConstantBuffers(slot, 1, buf->GetBuffer()->GetAddrOf());
			grcDeviced3d::Get()->context->PSSetConstantBuffers(slot, 1, buf->GetBuffer()->GetAddrOf()); 
		}
	}
	void AppendShaderConstantBuffer(int reg, grcCBuffer* bufferData) {
		this->m_pBuffers.insert({reg, bufferData});
	}
	~grcShaderGroup() {
		delete VertexShader;
		VertexShader = nullptr;
		delete PixelShader;
		PixelShader = nullptr;
	}
private:
	std::unordered_map<int, grcCBuffer*> m_pBuffers;
	grcShaderVertex* VertexShader = nullptr;
	grcShaderPixel* PixelShader = nullptr;
};