/* Quick explaination:
    Lib I wrote for what i call "object obfuscation", allows me to hide a int, float, number, any number type variables
    with xor and hiden behind a pointer
*/
#pragma once
#include <cstdint>
#include "Amp.hpp"

namespace Amp {
	class ObjectObfuscation
	{
    public:
        template <typename t>
        static uintptr_t New(t* val, unsigned int modKey = Amp::seed) {
            *val ^= modKey;
            t* p = val;
            uintptr_t* p0 = new uintptr_t((uintptr_t)p ^ modKey);
            *val ^= (uintptr_t)p0 ^ modKey;
            return (uintptr_t)p0 ^ modKey;
        }

        template <typename t>
        static t Get(uintptr_t pa, unsigned int modKey = Amp::seed) {
            uintptr_t* p0 = (uintptr_t*)(pa ^ modKey);
            t* p = (t*)(*p0 ^ modKey);
            return ((*p) ^ modKey) ^ pa;
        }

        template <typename t>
        static void Set(uintptr_t pa, t nval, unsigned int modKey = Amp::seed) {
            uintptr_t* p0 = (uintptr_t*)(pa ^ modKey);
            t* p = (t*)(*p0 ^ modKey);
            *p = (nval ^ modKey) ^ pa;
        }

        template <typename t>
        static void Delete(uintptr_t pa, unsigned int modKey = Amp::seed) {
            uintptr_t* p0 = (uintptr_t*)(pa ^ modKey);
            t* p = (t*)(*p0 ^ modKey);
            delete p0;
            delete p;
        }
	};
}