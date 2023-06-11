#pragma once

#include "Utils.hpp"
#include "Retcheck.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

#define NEW_SUBROUTINE(name, addr, cc, ret, ...) __declspec(selectany) auto name = (ret(cc*)(__VA_ARGS__))Amp::Retcheck::Unprotect(Amp::Utils::Rebase(addr))
#define NEW_OFFSET(name, ofs) __declspec(selectany) unsigned int name = ofs
#define NEW_CONST_OFFSET(name, ofs) const unsigned int name = ofs
#define NEW_ADDRESS(name, addr) __declspec(selectany) unsigned int name = Amp::Utils::Rebase(addr)