#pragma once
#include "LAGFramework\renderer\rendercontext.h"
#include "LAGFramework\renderer\device.h"
class CGraphics {
public:
	static void Init(unsigned int mode); 
	static void Shutdown();
	static fwRenderContext* GetContext() { return sm_pImmediateContext; }
	static fwDevice* GetDevice() { return sm_pDevice; }
	static eAPIDesc WhatApiAmIUsing() { return sm_API;  }
private:
	static constexpr eAPIDesc sm_API = D3D11;
	static fwRenderContext* sm_pImmediateContext;
	static fwDevice* sm_pDevice;
};