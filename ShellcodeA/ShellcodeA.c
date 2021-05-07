#define WIN32_LEAN_AND_MEAN

#pragma warning( disable : 4201 ) // Disable warning about 'nameless struct/union'

#include "GetProcAddressWithHash.h"

#include <Windows.h>

/** NOTE: module hashes are computed using all-caps unicode strings */
#define LDRLOADDLL_HASH					0xbdbf9c13
#define LDRGETPROCADDRESS_HASH			0x5ed941b5

typedef int (WINAPI* MESSAGEBOXW)(HWND, LPCWSTR, LPCWSTR, UINT);
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
	LDRLOADDLL pLdrLoadDll = NULL;
	LDRGETPROCADDRESS pLdrGetProcAddress = NULL;
	MESSAGEBOXW pMessageBoxW = NULL;

	// General
	HANDLE hUser32;
	
	// String
	UNICODE_STRING uString = { 0 };
	STRING aString = { 0 };

	WCHAR sUser32[] = { 'u', 's' ,'e' ,'r' ,'3' ,'2' ,'.' ,'d' ,'l' ,'l' };

	BYTE sMessageBoxW[] = { 'M','e','s','s','a','g','e','B','o','x','W', 0 };

	WCHAR sMsgContent[] = { 'H','e','l','l','o', ' ', 'W','o','r','l','d','!', 0 };
	WCHAR sMsgTitle[] = { 'D','e','m','o','!', 0 };

	///
	// STEP 1: locate all the required functions
	///

	pLdrLoadDll = (LDRLOADDLL)GetProcAddressWithHash(LDRLOADDLL_HASH);
	pLdrGetProcAddress = (LDRGETPROCADDRESS)GetProcAddressWithHash(LDRGETPROCADDRESS_HASH);

	uString.Buffer = sUser32;
	uString.MaximumLength = sizeof(sUser32);
	uString.Length = sizeof(sUser32);

	pLdrLoadDll(NULL, 0, &uString, &hUser32);

	FILL_STRING_WITH_BUF(aString, sMessageBoxW);
	pLdrGetProcAddress(hUser32, &aString, 0, (PVOID*)&pMessageBoxW);

	///
	// STEP 2: pop messagebox
	///

	pMessageBoxW(NULL, sMsgContent, sMsgTitle, 0x00000000L);
}
