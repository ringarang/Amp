#pragma once

/*
note:
	Init will add a vected exception handler that we'll use to retrieve the correct address corresponding to the int3 breakpoint.
	The idea is to call GenerateSafeAddress and pass our c function's address in it so it returns the location of a int3 breakpoint in .text range corresponding to our c function, therefore
	bypassing callcheck.
*/

namespace Amp {
	class Callcheck
	{
	public:
		static void Init();
		static unsigned int GenerateSafeAddress(unsigned int addr);
	};
}

