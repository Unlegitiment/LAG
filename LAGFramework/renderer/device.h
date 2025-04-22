#pragma once
#include <map>
/*
*	Auth: Unlegitiment
	Purpose:	Framework Device is meant to abstractly represent a Graphical Device. 
				It more follows D3D11's style of creation resources.
				However it can represent any device as its just a abstract class.
				Can represent a Vulkan device. Or a D3D11 Device. Or any type of device OpenGL whatever. 
*/
#include "Result.h"
enum eAPIDesc {
	D3D11,
	VULKAN,
	OPENGL,
};
struct fwBufferDesc {
	eAPIDesc StateTest;
	const void* nativeDesc;
};
struct fwInitData {
	const void* pSysMem;
	unsigned int		SysMemPitch;
	unsigned int		SysMemSlicePitch;
};
class fwDevice {
public:
	virtual eGraphicsResult CreateBuffer(const fwBufferDesc* desc, const fwInitData* init, void** outObj) = 0; 
private:

};