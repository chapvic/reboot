/***************************************************************************

MIT License

Copyright (c) 2020 FoxTeam

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*****************************************************************************/

#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
#endif // UNICODE

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif
#endif // _UNICODE

#if defined(UNICODE) && defined(_MBCS)
#undef _MBCS
#endif

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#ifndef __GNUC__
#pragma comment(lib, "advapi32")
#endif

#ifndef MemAlloc
#define MemAlloc(s) HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(s))
#endif

#ifndef MemFree
#define MemFree(p) if(p){HeapFree(GetProcessHeap(),0,(p));(p)=NULL;}
#endif

int main(int argc, char * argv[]) {
	HANDLE hToken;
	TOKEN_PRIVILEGES * NewState = NULL;
	TCHAR lpErrorMessage[256] = {0};
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
	NewState = (TOKEN_PRIVILEGES *)MemAlloc(sizeof(TOKEN_PRIVILEGES) + sizeof(LUID_AND_ATTRIBUTES));
	NewState->PrivilegeCount = 1;
	if(!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &NewState->Privileges[0].Luid)) goto Error;
	NewState->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if(!AdjustTokenPrivileges(hToken, FALSE, NewState, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) goto Error;
	CloseHandle(hToken);
	if(InitiateSystemShutdown(NULL, _T("Reboot"), 0, TRUE, TRUE)) goto Exit;
Error:
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
		MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), lpErrorMessage, 255, NULL);
	_tprintf(_T("Error: %s\n"), lpErrorMessage);
Exit:
	MemFree(NewState);
	return 0;
}
