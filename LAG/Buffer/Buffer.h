#pragma once
#include "Buffer/Buffer.h"
#include <d3d11.h>
#include <device\device.h>
class grcBufferD3D {
public:
	grcBufferD3D(D3D11_BIND_FLAG bindType, void* data, UINT size) { // check val 
		D3D11_BUFFER_DESC verBuffDescr = { };
		verBuffDescr.ByteWidth = size; // this would just be the type?
		verBuffDescr.Usage = D3D11_USAGE_DYNAMIC;
		verBuffDescr.BindFlags = bindType;
		verBuffDescr.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		
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
