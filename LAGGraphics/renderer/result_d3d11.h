#pragma once
#include "lagFramework\renderer\result.h"
typedef long HRESULT;
class grcD3D11Result : public fwGraphicsResultMap<HRESULT> {
	void Init();
};
#undef ins
