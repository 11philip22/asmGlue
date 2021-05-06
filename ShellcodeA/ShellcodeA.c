#define WIN32_LEAN_AND_MEAN

#pragma warning( disable : 4201 ) // Disable warning about 'nameless struct/union'

#include "GetProcAddressWithHash.h"

#include <Windows.h>
#include <winternl.h>
#include <intrin.h>

#define DEREF( name )*(UINT_PTR *)(name)
#define DEREF_64( name )*(DWORD64 *)(name)
#define DEREF_32( name )*(DWORD *)(name)
#define DEREF_16( name )*(WORD *)(name)
#define DEREF_8( name )*(BYTE *)(name)

/** NOTE: module hashes are computed using all-caps unicode strings */
#define LDRLOADDLL_HASH					0xbdbf9c13
#define LDRGETPROCADDRESS_HASH			0x5ed941b5

typedef int (WINAPI* MESSAGEBOXA)(HWND, LPSTR, LPSTR, UINT);
typedef NTSTATUS(WINAPI* LDRLOADDLL)(PWCHAR, ULONG, PUNICODE_STRING, PHANDLE);
typedef NTSTATUS(WINAPI* LDRGETPROCADDRESS)(HMODULE, PANSI_STRING, WORD, PVOID*);

#pragma warning( push )
#pragma warning( disable : 4214 ) // nonstandard extension
typedef struct
{
	WORD	offset : 12;
	WORD	type : 4;
} IMAGE_RELOC, * PIMAGE_RELOC;
#pragma warning(pop)

static inline size_t
AlignValueUp(size_t value, size_t alignment) {
	return (value + alignment - 1) & ~(alignment - 1);
}
static inline size_t
_strlen(char* s) {
	size_t i;
	for (i = 0; s[i] != '\0'; i++);
	return i;
}

static inline size_t
_wcslen(wchar_t* s) {
	size_t i;
	for (i = 0; s[i] != '\0'; i++);
	return i;
}

#define RVA(type, base, rva) (type)((ULONG_PTR) base + rva)

#define FILL_STRING(string, buffer) \
	string.Length = (USHORT)_strlen(buffer); \
	string.MaximumLength = string.Length; \
	string.Buffer = buffer

#define FILL_UNI_STRING(string, buffer) \
	string.Length = (USHORT)_wcslen(buffer); \
	string.MaximumLength = string.Length; \
	string.Buffer = buffer

#define FILL_STRING_WITH_BUF(string, buffer) \
	string.Length = sizeof(buffer); \
	string.MaximumLength = string.Length; \
	string.Buffer = (PCHAR)buffer

VOID Run()
{
#pragma warning( push )
#pragma warning( disable : 4055 ) // Ignore cast warnings

	// Function pointers
	LDRLOADDLL pLdrLoadDll;
	LDRGETPROCADDRESS pLdrGetProcAddress;
	MESSAGEBOXA pMessageBoxA = NULL;

	// General
	HANDLE library;
	
	// String
	UNICODE_STRING uString = { 0 };
	STRING aString = { 0 };

	WCHAR sKernel32[] = { 'k', 'e', 'r', 'n', 'e', 'l', '3', '2', '.', 'd', 'l', 'l' };
	CHAR msg[2] = { 'a','\0'};
	
	// At a certain length (15ish), the compiler with screw with inline
	// strings declared as CHAR. No idea why, use BYTE to get around it.
	BYTE sMessageBoxA[] = { 'M', 'e', 's', 's', 'a', 'g', 'e', 'B', 'o', 'x', 'A' };

	///
	// STEP 1: locate all the required functions
	///

	pLdrLoadDll = (LDRLOADDLL)GetProcAddressWithHash(LDRLOADDLL_HASH);
	pLdrGetProcAddress = (LDRGETPROCADDRESS)GetProcAddressWithHash(LDRGETPROCADDRESS_HASH);

	uString.Buffer = sKernel32;
	uString.MaximumLength = sizeof(sKernel32);
	uString.Length = sizeof(sKernel32);

	pLdrLoadDll(NULL, 0, &uString, &library);

	FILL_STRING_WITH_BUF(aString, sMessageBoxA);
	pLdrGetProcAddress(library, &aString, 0, (PVOID*)&pMessageBoxA);

	pMessageBoxA(NULL, msg, msg, 0x00004000L);
}