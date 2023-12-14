#pragma once

#include <Windows.h>
#include <stdio.h>

inline VOID HexDump(const PVOID data, SIZE_T size) {
	char ascii[17];
	SIZE_T i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		wprintf(L"%02X ", ((PBYTE)data)[i]);
		if (((PBYTE)data)[i] >= ' ' && ((PBYTE)data)[i] <= '~') {
			ascii[i % 16] = ((PBYTE)data)[i];
		}
		else {
			ascii[i % 16] = '.';
		}
		if ((i + 1) % 8 == 0 || i + 1 == size) {
			wprintf(L" ");
			if ((i + 1) % 16 == 0) {
				wprintf(L"|  %hs \n", ascii);
			}
			else if (i + 1 == size) {
				ascii[(i + 1) % 16] = '\0';
				if ((i + 1) % 16 <= 8) {
					wprintf(L" ");
				}
				for (j = (i + 1) % 16; j < 16; ++j) {
					wprintf(L"   ");
				}
				wprintf(L"|  %hs \n", ascii);
			}
		}
	}
	wprintf(L"\n");
}
