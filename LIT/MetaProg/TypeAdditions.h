#pragma once
namespace lit {
	// @ -- Append R-Value Modifications to Type. 
	template<typename T> struct AddRef {
		using Type = T&&;
	};
	template<typename T> struct AddRef<T&> {
		using Type = T&;
	};
	template<> struct AddRef<void> {
		using Type = void;
	};
	template<typename T> struct AddPtr {
		using Type = T*;
	};
	template<typename T> struct AddPtr<T&> {
		using Type = T*&;
	};
	
}