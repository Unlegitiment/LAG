#pragma once
#include "typeidentification.h"
#include "functionchecks.h"
namespace lit {
    template <typename T, bool = IsPrimitive<T>::value>
    struct DefaultComparison
    { // for customs
        static bool Compare(T& a, T& b)
        {
            static_assert(HasComparisonOperatorV<T> && "Operator T does not have valid comparison");
            return a == b;
        }
    };
    template <typename T>
    struct DefaultComparison<T*, false> // pointer specification. so that raw_pointers can too be included inside of this since they technically aren't full primitives. 
    { // for customs
        static bool Compare(T*& a, T*& b)
        {
            //static_assert(HasComparisonOperatorV<T> && "Operator T does not have valid comparison");
            return a == b;
        }
    };
    template <typename T>
    struct DefaultComparison<T, true>
    {
        static bool Compare(T& a, T& b)
        {
            return a == b;
        }
    };
}