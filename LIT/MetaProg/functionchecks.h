#pragma once
#include "conditionals.h"
#include "valuesubsitution.h"
#include "enables.h"
#include "typeidentification.h"
namespace lit {
	template<typename T> struct IsCopyable {
	private:
		template<typename U, typename = decltype(U(DeclaredValue<const U&>()))> static TrueConditional Test(int);
		template<typename> static FalseConditional Test(...);
	public:
		static constexpr bool value = decltype(Test<T>(1))::value;
	};
	template<typename T> struct IsDefaultConstructable {
	private:
		template<typename U, typename = decltype(U())> static TrueConditional Test(int);
		template<typename> static FalseConditional Test(...);
	public:
		static constexpr bool value = decltype(Test<T>(1))::value;
	};
	enum eOperation {
		COMPARISON,
	};
	template<eOperation operation, typename T, typename = typename EnableTypeIf<!IsPrimitive<T>::value>::type> struct IsOperationSpecified {
		static constexpr bool value = false;
	};
	template<typename T> struct IsOperationSpecified<COMPARISON, T> {
	private:
		template<typename U, typename = decltype(DeclaredValue<U&&>().operator==(DeclaredValue<const U&>()))> static TrueConditional Test(int); //bool operator==(const U&) const {}
		template<typename> static FalseConditional Test(...);
	public:
		static constexpr bool value = decltype(Test<T>(1))::value;
	};
	template <typename T>
	constexpr bool HasComparisonOperatorV = IsOperationSpecified<COMPARISON, T>::value;
	template <typename T>
	static constexpr bool IsCopyableV = IsCopyable<T>::value;
}