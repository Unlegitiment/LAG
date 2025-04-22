#include "result_d3d11.h"
#include "d3d11.h"
#define ins(res, hres) this->RESULT_MAPPING.insert({res,hres})
/*
	this is boring probably automate this shit out of here? copy from msdn and write smth to push them out of here. 
	find first _ for MSDN and replace upto that point with GRAPHICS for enum

*/
void grcD3D11Result::Init() {
	sm_pInst = this;
	ins(GRAPHICS_OK, S_OK);
	ins(GRAPHICS_FILE_NOT_FOUND, D3D11_ERROR_FILE_NOT_FOUND);
	ins(GRAPHICS_INVALID_ARG, E_INVALIDARG);
	ins(GRAPHICS_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD, D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD); // baselevel stuff idk
	ins(GRAPHICS_FALSE, S_FALSE);
	ins(GRAPHICS_ERROR_OUT_OF_MEMORY, E_OUTOFMEMORY);
	ins(GRAPHICS_NOTIMPL, E_NOTIMPL);
	ins(GRAPHICS_FAIL, E_FAIL);
}
