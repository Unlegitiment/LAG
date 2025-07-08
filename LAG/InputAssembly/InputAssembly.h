#pragma once
#include <d3d11.h>
#include "LAG\Shader\Shadergroup.h"
#include <LAG\device\device.h>
class grcInputLayout {
public:
	grcInputLayout(grcShaderVertex* shader) {
		D3D11_INPUT_ELEMENT_DESC inputEleDesc[] = {
			{"POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			//{"COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TXTC",0,DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		HRESULT hr = grcDeviced3d::Get()->outDevice->CreateInputLayout(inputEleDesc, ARRAYSIZE(inputEleDesc), shader->GetBlob()->GetBufferPointer(), shader->GetBlob()->GetBufferSize(), &pLayout);
		assert(SUCCEEDED(hr));
	}
	void Bind() {
		grcDeviced3d::Get()->context->IASetInputLayout(pLayout);
	}
	~grcInputLayout() {
		if (pLayout) pLayout->Release();
	}
	ID3D11InputLayout* GetLayout() { return pLayout; }
private:
	ID3D11InputLayout* pLayout = nullptr;
};