#pragma once
#include <d3d11.h>
/*
* No template. Why? Templating locks us to a vertex formula, this allows for theroetical dynamic vertex information 
* Right now stride reps what basically it would look like before every new line is done. Without a template, freeing of the issue.
* {x,y,norx,nory}\n stride specifies a 4 stride element big enough for 4 floats else you'd have weird overlap like: {x,y,norx,nory,x} <-- WE DO NOT WANT THIS!.
* 
*/
class grcVertexBuffer {
public:
	void Init(ID3D11Device* dev, unsigned long iStride, void* data, unsigned long numVertices) { // @TODO: RM: D3D ADD: fwDevice*,  REM: fwDevice -> grcBaseDevice
		this->m_iCount = numVertices;
		this->m_iStride = iStride;
		D3D11_BUFFER_DESC vertexBufferDesc{};
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.ByteWidth = iStride * numVertices;

		D3D11_SUBRESOURCE_DATA vertexBufferData{};
		vertexBufferData.pSysMem = data;
		HRESULT H = dev->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &this->m_pBuffer); 
		if (H != S_OK) {
			DebugBreak();
		}
		return;
	}
	void Destroy() {
		this->m_pBuffer->Release();
	}
	ID3D11Buffer* Get() const { return this->m_pBuffer; }
	ID3D11Buffer** GetAddressOf() { return &this->m_pBuffer; }
	unsigned long GetStride() const { return this->m_iStride; }
	unsigned long* GetAddressOfStride() { return &this->m_iStride; }
	unsigned long GetCount() const { return this->m_iCount; }
	unsigned long* GetAddressOfCount() { return &this->m_iCount;  }
private:
	ID3D11Buffer* m_pBuffer;
	unsigned long m_iStride;
	unsigned long m_iCount;
};
