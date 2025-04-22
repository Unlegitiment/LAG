#pragma once
template<typename Base, typename Derived> struct IsBaseOf {
private:
	static char Test(Base*);
	static int Test(void*);
public:
	static constexpr bool value = sizeof(Test(static_cast<Derived*>(nullptr))) == sizeof(char);
};
