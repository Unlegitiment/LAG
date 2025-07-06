#pragma once
namespace lit {
	template<typename T> struct StripPointer {
		using Type = T;
		//using NoPointerType = T;
	};
	template<typename T> struct StripPointer<T*> {
		using Type = T; // check?
		//using NoPointerType = typename StripPointer<T>::NoPointerType; // OHH ITS RECURSION THATS FUCKING FIRE DUDE!
	};
	template<typename T> struct StripAllPointers {
		using Type = T;
	};
	template<typename T> struct StripAllPointers {
		using Type = typename StripPointer<T>::Type;
	};
	template<typename T> struct StripRef {
		using Type = T;
	};
	template<typename T> struct StripRef<T&> {
		using Type = T;
	};
	template<typename T> struct StripRef<T&&> {
		using Type = T;
	};

}