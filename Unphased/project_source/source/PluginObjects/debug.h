#include <cstdarg>
#include <windows.h>
#include <stdio.h>

inline void debugOut(const char* fmt, ...)
{
	 va_list a= nullptr;
	 char szDebugString[1024];
	 if(lstrlenA(fmt)==0)return;
	 va_start(a,fmt);
	 vsprintf(szDebugString,fmt,a);
	 va_end(a);
	 if(lstrlenA(szDebugString)==0)return;
	 OutputDebugString(szDebugString);
}