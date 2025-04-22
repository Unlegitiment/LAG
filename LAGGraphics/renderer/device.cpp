#include "device.h"
#include <d3d11.h>
#include "LAGFramework\core\exception.h"
std::string GetExcept(eAPIDesc desc) {
	switch (desc){
		case D3D11:
			return "D3D11";
		case OPENGL:
			return "OPENGL";
		case VULKAN:
			return "VULKAN";
	}
}
eGraphicsResult grcD3D11Device::CreateBuffer(const fwBufferDesc* desc, const fwInitData* init, void** outObj) {
	if (desc->StateTest != D3D11) {
		throw LagException("[Fatal] Invalid Graphics API. Expected D3D11 got " + GetExcept(desc->StateTest));
	}
	ID3D11Buffer* outbuf;
	m_pDevice->CreateBuffer((D3D11_BUFFER_DESC*)desc->nativeDesc, (D3D11_SUBRESOURCE_DATA*)init, &outbuf); 
	*outObj = outbuf;
	return GRAPHICS_OK;
}
