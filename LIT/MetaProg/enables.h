#pragma once
#include "conditionals.h"
namespace lit {
	template<bool Condition, typename T = void> struct EnableTypeIf { };
	template<typename T> struct EnableTypeIf<true, T> {
		using type = T;
	};
	template<bool Condition> struct EnableValueIf : public FalseConditional { };
	template<> struct EnableValueIf<true> : public TrueConditional { };
}