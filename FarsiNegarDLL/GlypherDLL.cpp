// GlypherDLL.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "glypherdll.h"

// The one and only GlypherDLL object
GlypherDLL theApp;
#pragma data_seg(".shared")
	HHOOK g_hKbdHook = NULL;
	HWND g_hWndOwner = NULL;
#pragma data_seg() 
#pragma comment(linker, "/SECTION:.shared,RWS")


BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID /*lpReserved*/)
{
	theApp.InstaceHandleField = hModule;
	return theApp.DllMain(ul_reason_for_call, 0);
}

extern "C" BOOL CALLBACK IsMonitorStarted()
{
	return IsWindow(g_hWndOwner);

}
 
extern "C" BOOL CALLBACK StartMonitor(HWND hWnd)
{
	if ( IsMonitorStarted() )
		return FALSE; //already started

	g_hKbdHook = SetWindowsHookEx(WH_KEYBOARD, GlypherDLL::KeyboardProc, theApp.InstaceHandleField, 0);
	if (g_hKbdHook!=NULL)
	{
		g_hWndOwner = hWnd;
		return TRUE;
	}
	else
	{
        return FALSE;
	}
}

extern "C" BOOL CALLBACK StopMonitor()
{
	BOOL ret = UnhookWindowsHookEx( g_hKbdHook );
	if (ret!=FALSE)
	{
		g_hWndOwner = NULL;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}