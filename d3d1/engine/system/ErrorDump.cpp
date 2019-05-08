#include "ErrorDump.h"
#include "Window.h"
#include "system/Logging.h"
#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>

LPTOP_LEVEL_EXCEPTION_FILTER oldFilter;

LONG WINAPI UnhandledExceptionHandler(PEXCEPTION_POINTERS exceptionPointers)
{
	BOOL bMiniDumpSuccessful;
	WCHAR szPath[1024];
	WCHAR szFileName[1024];
	const WCHAR* szAppName = L"AppName";
	const WCHAR* szVersion = L"v1.0";
	DWORD dwBufferSize = 1024;
	HANDLE hDumpFile;
	SYSTEMTIME stLocalTime;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;

	GetLocalTime(&stLocalTime);
	GetCurrentDirectory(dwBufferSize, szPath);
	
	wsprintf(szFileName, L"%s%s", szPath, szAppName);
	CreateDirectory(szFileName, NULL);
	wsprintf(szFileName, L"%s\\crash-dump-%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp",
		szPath, szVersion,
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
		GetCurrentProcessId(), GetCurrentThreadId());
	hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	ExpParam.ThreadId = GetCurrentThreadId();
	ExpParam.ExceptionPointers = exceptionPointers;
	ExpParam.ClientPointers = TRUE;

	bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
		hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);

	return EXCEPTION_EXECUTE_HANDLER;
}

void ErrorDump::setup() {
	// To debug handler uncomment:
	/*__try {
		int *pBadPtr = NULL;
		*pBadPtr = 0;
	} __except (UnhandledExceptionHandler(GetExceptionInformation())) {}*/

	oldFilter = SetUnhandledExceptionFilter(UnhandledExceptionHandler);
}

void ErrorDump::close() {
	SetUnhandledExceptionFilter(oldFilter);
}