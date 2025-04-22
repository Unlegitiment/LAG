#pragma once
#include "LAGFramework\renderer\device.h"
struct ID3D11Device;
class grcD3D11Device : public fwDevice{
public:
	grcD3D11Device(ID3D11Device* device) : m_pDevice(device){}
	eGraphicsResult CreateBuffer(const fwBufferDesc* desc, const fwInitData* init, void** outObj) override;
	ID3D11Device* GetRawDevice() { return this->m_pDevice; }
private:
	ID3D11Device* m_pDevice;
};