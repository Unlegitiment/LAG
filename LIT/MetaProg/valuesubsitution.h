#pragma once
#include "TypeAdditions.h"
namespace lit {
	template<typename T> typename AddRef<T>::type DeclaredValue(); // This basically means like the value WILL exist but right now we are in compiler so the value doesn't.
}