#pragma once
#include <map>
enum eGraphicsResult {
#pragma region D3D11_SPECIFIC
	GRAPHICS_ERROR_OUT_OF_MEMORY,
	GRAPHICS_INVALID_CALL,
	GRAPHICS_FILE_NOT_FOUND,
	GRAPHICS_TOO_MANY_STATE_OBJECTS,
	GRAPHICS_TOO_MANY_UNIQUE_VIEW_OBJECTS,
	GRAPHICS_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD,
	GRAPHICS_WAS_STILL_DRAWING,
	GRAPHICS_FAIL,
	GRAPHICS_INVALID_ARG,
	GRAPHICS_NOTIMPL,
	GRAPHICS_FALSE,
	GRAPHICS_OK,
#pragma endregion D3D11_SPECIFIC
#pragma region LAG_ENGINE
	LAG_GRAPHICS_INVALID_API_MISSMATCH,
	LAG_GRAPHICS_ERR_MAX
#pragma endregion LAG_ENGINE
};

template<typename RESULTTYPE>
class fwGraphicsResultMap {
public:
	virtual void Init() =0;
	virtual eGraphicsResult Translate(RESULTTYPE v) = 0;
	static fwGraphicsResultMap& GetInst() { return *sm_pInst;  }
	 template<typename T> static T& GetInst() { return (T&)*sm_pInst; }
protected:
	fwGraphicsResultMap() = default;
	std::map<eGraphicsResult, RESULTTYPE> RESULT_MAPPING;
	static fwGraphicsResultMap* sm_pInst;
};