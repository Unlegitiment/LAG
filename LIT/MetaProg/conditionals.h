#pragma once
namespace lit {
	template<typename T, T val>
	struct Type {
		using ValueType = T;
		using Constant = Type;
		static constexpr T value = val;
	};
	template<bool val> using TypeConditional = Type<bool, val>; 
	using TrueConditional = TypeConditional<true>; 
	using FalseConditional = TypeConditional<false>;
}