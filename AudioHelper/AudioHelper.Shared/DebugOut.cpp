#include "pch.h"

using namespace Platform;

void DebugOut(LPTSTR pszFormat, ...)
{
	TCHAR szUser[2048], szBuf[4096];
	va_list argptr;
	va_start(argptr, pszFormat);

	StringCbVPrintf(szUser, sizeof(szUser), pszFormat, argptr);

	StringCbPrintf(szBuf, sizeof(szBuf), L"[%d:%d] - ", GetCurrentProcessId(), GetCurrentThreadId());

	StringCbCat(szBuf, sizeof(szBuf), szUser);
	StringCbCat(szBuf, sizeof(szBuf), L"\r\n");
	OutputDebugString(szBuf);
} // DebugOut

void DumpRefcount(Object^ o)
{
	IUnknown *punk = (IUnknown *) o;
	punk->AddRef();
	auto refcount = punk->Release();
	DebugOut(L"Refcount = %d", refcount);
} // DumpRefCount