#pragma once
#include <d3d11.h>
#include "Buffer.h"
#include <device\device.h>
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