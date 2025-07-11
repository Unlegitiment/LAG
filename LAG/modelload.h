#pragma once

#include "LIT\types\integrals.h"
#include <vector>

class CGameModel {
public:
private:

};
template<typename ImporterType, typename SceneType, typename MeshType> struct ImporterParams {
	using ImporterBase = ImporterType;
	using ImporterScene = SceneType;
	using Mesh = MeshType;
};
using NullParams = ImporterParams<void, void, void>;
template<typename ImpParams> // This is required for the template declarations
class CGameModelLoader {
public:
private:

};
