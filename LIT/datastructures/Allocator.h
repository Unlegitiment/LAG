#pragma once // my fucking allocation implementation with placement new got fucking wiped. are you FUCKING JOKING BRUH. thanks microshit. 
#include "LIT\MetaProg\enables.h"
#include "LIT\MetaProg\typeidentification.h"
namespace lit {
	
	template<typename T, typename __SizeType>
	class Allocator {
	public:
		using Type = T;
		using Ptr = T*;
		using Ref = T&; // might have to strip lmao
		using SizeType = __SizeType;
		static_assert(IsIntegralConstant<SizeType>::value && "Allocator<T, ???> SizeType is not an integral constant. This means it is not a char, short, int, or long long!");
		Ptr Allocate(SizeType Amount) {
			return (Ptr)::operator new(sizeof(Type)*Amount); // using new like this is just weird lmao.
		}
		// @Todo: Micro Optimizations Available: Perfect Forwarding. 
		template<typename... Args>
		void Construct(Ptr pPtr, Args&&... args) {
			new (pPtr)  args...; // placement new. - Allows for the construction of a preexisting allocation blk. 
		}
		void Deconstruct(Ptr pPtr) {
			pPtr->~T();
		}
		void Deallocate(Ptr pPtr) {
			delete pPtr; // I think I don't have to use the arr type here because its not technically an arr.
		}
	};
}