#pragma once

#include <cstdint>

namespace Amp {
	class Retcheck
	{
	public:
		static uintptr_t Unprotect(uintptr_t addr);
	};
}

