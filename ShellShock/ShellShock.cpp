#include "Windows.h"

int main ()
{
	LPSTR rdiShellcode;
	DWORD rdiShellcodeLength;

	if (rdiShellcode == NULL || rdiShellcodeLength == 0) return 0;

	BYTE bootstrap[49] = { 0 };
	DWORD i = 0;

	// call next instruction (Pushes next instruction address to stack)
	bootstrap[i++] = 0xe8;
	bootstrap[i++] = 0x00;
	bootstrap[i++] = 0x00;
	bootstrap[i++] = 0x00;
	bootstrap[i++] = 0x00;

	// pop eax - Capture our current location in memory
	bootstrap[i++] = 0x58;

	// push ebp
	bootstrap[i++] = 0x55;

	// move ebp, esp
	bootstrap[i++] = 0x89;
	bootstrap[i++] = 0xe5;

	// call - Transfer execution to the RDI
	bootstrap[i++] = 0xe8;
	bootstrap[i++] = sizeof(bootstrap) - i - 4; // Skip the remainder of instructions
	bootstrap[i++] = 0x00;
	bootstrap[i++] = 0x00;
	bootstrap[i++] = 0x00;

	// add esp, 0x14 - clean up stack from args (cdecl)
	bootstrap[i++] = 0x83;
	bootstrap[i++] = 0xc4;
	bootstrap[i++] = 0x14;

	// leave
	bootstrap[i++] = 0xc9;

	// ret - return to caller
	bootstrap[i++] = 0xc3;

	// Ends up looking like this in memory:
	// Bootstrap shellcode
	// RDI shellcode
	DWORD outLength = rdiShellcodeLength + sizeof(bootstrap);
	LPSTR outBytes = (LPSTR)malloc(outLength);
	MoveMemory(outBytes, bootstrap, sizeof(bootstrap));
	MoveMemory(outBytes + sizeof(bootstrap), rdiShellcode, rdiShellcodeLength);
}