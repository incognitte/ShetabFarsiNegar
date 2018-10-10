#pragma once

#include "global.h"
#include "resource.h"		// main symbols
extern HHOOK g_hKbdHook;
extern HWND g_hWndOwner;

// GlypherDLL
// See GlypherDLL.cpp for the implementation of this class
class GlypherDLL : public CAtlDllModuleT<GlypherDLL>
{
	Public HMODULE InstaceHandleField;
	Public GlypherDLL()
	{
		InstaceHandleField = NULL;
	}

	Public static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam , LPARAM lParam)
	{
		if (nCode<0)
			return CallNextHookEx(g_hKbdHook, nCode, wParam, lParam);

		bool bFarsiNegarWnd = GetProp(GetForegroundWindow(), _T("FarsiNegar"))==(HANDLE)1;
		if ( nCode==HC_ACTION && !bFarsiNegarWnd )
		{
			//bool bControl = _TOBOOL(GetKeyState(VK_CONTROL)&0x8000);
			bool bShift = _TOBOOL(GetKeyState(VK_SHIFT)&0x8000);
			bool bAlt = _TOBOOL(GetKeyState(VK_MENU)&0x8000);
			bool bPress = !_TOBOOL(lParam&0x80000000);

			//grab Convertor
			bool bFastConvert = wParam==VK_F12 && !bShift && bAlt && SendMessage(g_hWndOwner, g_uMessageGetData, needEnableFastConvertor, 0)!=0;
			if ( bFastConvert && bPress)
			{
				ATLTRACE(L"Pressed");
				SetTimer(NULL, 0, 100, TimerProc_ForShowConvertor);
				return 1;
			}

			//grab autoinsert
			bool bAutoInsert = false; //wParam==VK_F12 && !bControl && !bShift && !bAlt && SendMessage(g_hWndOwner, g_uMessageGetData, needEnableAI, 0)!=0;
			if ( bAutoInsert && bPress )
			{
				//clean clipboard
				m::Clipboard::CopyText(_T(""), true);

				//select all and copy
				Press_SelectAll();
				Press_Copy();

				SetTimer(NULL, 0, 100, TimerProc1);
				return 1;
			}

			//ignore if in autoInsert or FastConvert mode
			if (bAutoInsert || bFastConvert)
				return 1;
		}

		return CallNextHookEx(g_hKbdHook, nCode, wParam, lParam);
	}

	Private static VOID CALLBACK TimerProc1(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR idEvent, DWORD /*dwTime*/)
	{
		KillTimer (NULL, idEvent);
		//do autoinsert
		PostMessage(g_hWndOwner, g_uMessageGrab, (WPARAM)gmDoAutoInsert, 0);
	}

	Private static VOID CALLBACK TimerProc_ForShowConvertor1(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR idEvent, DWORD /*dwTime*/)
	{
		KillTimer (NULL, idEvent);

		//select all if clipboard is empty
		CString string;
		m::Clipboard::PasteText(string);
		if (string.IsEmpty())
		{
			//clean clipboard
			m::Clipboard::CopyText(_T(""), true);
			//select all and copy
			Press_SelectAll();
			Press_Copy();
		}

		//show convert menu
		SetTimer(NULL, 0, 100, TimerProc_ForShowConvertor2);
	}

	Private static VOID CALLBACK TimerProc_ForShowConvertor2(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR idEvent, DWORD /*dwTime*/)
	{
		KillTimer (NULL, idEvent);
		ShowMenu(gmNeedConvertorMenu, gmDoConvertor);
	}

	Private static VOID CALLBACK TimerProc_ForShowConvertor(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR idEvent, DWORD /*dwTime*/)
	{
		KillTimer (NULL, idEvent);
		ShowMenu(gmNeedConvertorMenu, gmDoConvertor);
	}

	Private static VOID CALLBACK TimerProc_ForCtrlRelease(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR idEvent, DWORD /*dwTime*/)
	{
		KillTimer (NULL, idEvent);
		bool bControl = _TOBOOL(GetKeyState(VK_CONTROL)&0x8000);
		if (bControl) keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	}

	Private static VOID CALLBACK TimerProc_ForGrabConvertorWin(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	Private static void ShowMenu(UINT uMsgNeedMenu, UINT uMsgProcessMenu)
	{
		//show convertor menu
		//get convertor menu handle form farsi negar
		LRESULT nResult = SendMessage(g_hWndOwner, g_uMessageGrab, (WPARAM)uMsgNeedMenu, 0);
		HMENU hMenu = (HMENU)nResult;

		//find position to show menu
		CPoint pt = CPoint(0,0);
		GetCursorPos(&pt); //use mouse position if caret not available

		//track menu
		int nMenuId = (int)TrackPopupMenu(GetSubMenu(hMenu, 0), TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, g_hWndOwner, 0);


		//post selected item to farsi negar
		PostMessage(g_hWndOwner, g_uMessageGrab, (WPARAM)uMsgProcessMenu, (LPARAM)nMenuId);
	}

	Private static void Press_SelectAll(void)
	{
		//do for photoshop
		CString strTitle = m::Wnd::GetWindowText(::GetForegroundWindow()).MakeLower();
		if (strTitle.Find(_T("photoshop"))>=0)
		{
			::PostMessage(::GetForegroundWindow(), WM_COMMAND, MAKEWPARAM(1017, 0), 0);
			return;
		}

		//do for others windows
		bool bControl = _TOBOOL(GetKeyState(VK_CONTROL)&0x8000);
		bool bShiftR = _TOBOOL(GetKeyState(VK_RSHIFT)&0x8000);
		bool bShiftL = _TOBOOL(GetKeyState(VK_LSHIFT)&0x8000);
		
		//Press Ctrl+Home
		if (!bControl) keybd_event(VK_CONTROL, 0, 0, 0);
		if (bShiftR) keybd_event(VK_RSHIFT, 0, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
		if (bShiftL) keybd_event(VK_LSHIFT, 0, KEYEVENTF_KEYUP, 0);
		keybd_event(VK_HOME, 0, KEYEVENTF_EXTENDEDKEY, 0);
		keybd_event(VK_HOME, 0, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);

		//Press Ctrl+Shift+End
		keybd_event(VK_SHIFT, 0, 0, 0);
		keybd_event(VK_END, 0, KEYEVENTF_EXTENDEDKEY, 0);
		keybd_event(VK_END, 0, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
		keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
		if (!bControl) keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	}

	Private static void Press_Copy(void)
	{
		//do for photoshop
		CString strTitle = m::Wnd::GetWindowText(::GetForegroundWindow()).MakeLower();
		if (strTitle.Find(_T("photoshop"))>=0)
		{
			::PostMessage(::GetForegroundWindow(), WM_COMMAND, MAKEWPARAM(104, 0), 0);
			return;
		}

		//do for others windows
		//press Ctrl+C
		bool bControl = _TOBOOL(GetKeyState(VK_CONTROL)&0x8000);
		if (!bControl) keybd_event(VK_CONTROL, 0, 0, 0);
		keybd_event('C', 0, 0, 0);
		keybd_event('C', 0, KEYEVENTF_KEYUP, 0);
		if (!bControl) SetTimer(NULL, 0, 100, TimerProc_ForCtrlRelease); //release control on idle to prevent some problem in photoshop
	}
};
