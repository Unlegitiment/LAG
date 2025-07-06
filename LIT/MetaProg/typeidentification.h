#pragma once
#include "LIT\MetaProg\conditionals.h"
namespace lit {
	template<typename T> struct IsPointer : public FalseConditional {};
	template<typename T> struct IsPointer<T*> : public TrueConditional {};
	template<typename T> struct IsIntegralConstant : public FalseConditional {};
	template<> struct IsIntegralConstant<char> : public TrueConditional {  };
	template<> struct IsIntegralConstant<short> : public TrueConditional {  };
	template<> struct IsIntegralConstant<int> : public TrueConditional {  };
	template<> struct IsIntegralConstant<long long> : public TrueConditional {  };
	template<typename T, bool b = IsIntegralConstant<T>::value || IsPointer<T>::value> struct IsPrimitive : public FalseConditional {}; // A Pointer is technically a primitive, bc every pointer is a hex-value under the hood and this hex-value is technically an unsigned long long. An R-Value is also *technically* a pointer but I have to do more research on whether that is viable as a mem address or not. 
	template<typename T> struct IsPrimitive<T, true> : public TrueConditional{};

	//template<> struct IsPrimitive<int> : public TrueConditional {};
}