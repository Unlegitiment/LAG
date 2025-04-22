#pragma once
#include "buffer.h"
#include <vector>
#pragma region IMPL_MV
#include <d3d11.h>
#include "LAGFramework\renderer\device.h"
#pragma endregion IMPL_MV
class grcIndexBuffer {
public:
	grcIndexBuffer() = default;
	void Init(ID3D11Device* dev, unsigned int* pIndex, unsigned int iCount) { // @TODO: RM: D3D ADD: fwDev*,  REM: fwDev* -> grcBaseDevice 
		this->m_iCount = iCount;
		D3D11_BUFFER_DESC indexBufferDesc{};
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.ByteWidth = sizeof(int) * iCount;

		D3D11_SUBRESOURCE_DATA indexBufferData{};
		indexBufferData.pSysMem = pIndex;
		HRESULT H = dev->CreateBuffer(&indexBufferDesc, &indexBufferData, &this->m_pBuffer);
		if (H != S_OK) {
			DebugBreak();
		}
		return;
	}
	void Destroy() {
		this->m_pBuffer->Release();
	}

	//Buffer Boilerplate

	ID3D11Buffer* Get() const { return this->m_pBuffer; }
	ID3D11Buffer** GetAddressOf() const { return &this->m_pBuffer; }
	unsigned long GetCount() const { return this->m_iCount; }
	unsigned long* GetAddressOfCount() const { return &this->m_iCount; }
private:
	ID3D11Buffer* m_pBuffer;
	unsigned long m_iCount;
};