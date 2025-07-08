#pragma once
#include <d3d11.h>
#include "Buffer.h"
#include "LIT\types\integrals.h"
class grcIndexBuffer {
public:
	grcIndexBuffer(lit::u32* data, UINT size) {
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