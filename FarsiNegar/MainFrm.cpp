// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Glypher.h"

#include "mainfrm.h"
#include "GlypherView.h"
#include "GlypherEditView.h"
#include "Options_Sheet.h"
#include "GlypherDoc.h"
#include "Dialog_Hint.h"

// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_REGISTERED_MESSAGE(g_uMessageGrab, OnAIGrab)
	ON_REGISTERED_MESSAGE(g_uMessageGetData, OnAINeedData)
	ON_REGISTERED_MESSAGE(g_uMessageShow, OnAppShow)
	ON_UPDATE_COMMAND_UI(ID_LANG_FARSI, OnUpdateLangFarsi)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_COMMAND(ID_VIEW_KASHIDA, OnViewKashida)
	ON_UPDATE_COMMAND_UI(ID_VIEW_KASHIDA, OnUpdateViewKashida)
	ON_COMMAND(ID_VIEW_KASHIDAFULL, OnViewKashidafull)
	ON_UPDATE_COMMAND_UI(ID_VIEW_KASHIDAFULL, OnUpdateViewKashidafull)
	ON_COMMAND(ID_VIEW_WRAP, OnViewWrap)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WRAP, OnUpdateViewWrap)
	ON_COMMAND(ID_VIEW_SINGLELINE, OnViewSingleline)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SINGLELINE, OnUpdateViewSingleline)
	ON_COMMAND(ID_EDITOR_REMOVEENTER, OnEditorRemoveenter)
	ON_COMMAND(ID_EDITOR_REMOVEKASHIDA, OnEditorRemovekashida)
	ON_COMMAND(ID_EDIT_EDITORFONT, OnEditEditorfont)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITORFONT, OnUpdateEditEditorfont)
	ON_COMMAND(ID_HELP_WEBSITE, OnHelpWebsite)
	ON_COMMAND(ID_HELP_README, OnHelpReadme)
	ON_COMMAND(ID_EDIT_ANSIMODE, OnEditAnsimode)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ANSIMODE, OnUpdateEditAnsimode)
	ON_COMMAND(ID_CopyAndMinimize, OnCopyAndMinimize)
	ON_UPDATE_COMMAND_UI(ID_CopyAndMinimize, OnUpdateCopyAndMinimize)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(messageTray, OnTray)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_VIEW_MAINWND, OnViewMainwnd)
	ON_COMMAND(ID_LANG_FARSI, OnLangFarsi)
	ON_UPDATE_COMMAND_UI(ID_LANG_FARSI, OnUpdateLangFarsi)
	ON_COMMAND(ID_LANG_ENGLISH, OnLangEnglish)
	ON_UPDATE_COMMAND_UI(ID_LANG_ENGLISH, OnUpdateLangEnglish)
	ON_COMMAND_RANGE(MENUID_COPY_FIRST, MENUID_COPY_LAST, OnCopyForFont)
	ON_UPDATE_COMMAND_UI_RANGE(MENUID_COPY_FIRST, MENUID_COPY_LAST, OnUpdateCopyForFont)
	ON_COMMAND_RANGE(MENUID_FONT_FIRST, MENUID_FONT_LAST, OnSetFont)
	ON_COMMAND_RANGE(MENUID_CUT_FIRST, MENUID_CUT_LAST, OnCutForFont)
	ON_UPDATE_COMMAND_UI_RANGE(MENUID_CUT_FIRST, MENUID_CUT_LAST, OnUpdateCutForFont)
	ON_UPDATE_COMMAND_UI_RANGE(MENUID_FONT_FIRST, MENUID_FONT_LAST, OnUpdateSetFont)
	ON_COMMAND_RANGE(MENUID_PASTE_FIRST, MENUID_PASTE_LAST, OnPasteForFont)
	ON_UPDATE_COMMAND_UI_RANGE(MENUID_PASTE_FIRST, MENUID_PASTE_LAST, OnUpdatePasteForFont)
	ON_COMMAND_RANGE(MENUID_DWFONT_FIRST, MENUID_DWFONT_LAST, OnDWFont)
	ON_UPDATE_COMMAND_UI_RANGE(MENUID_DWFONT_FIRST, MENUID_DWFONT_LAST, OnUpdateDWFont)
	ON_COMMAND(ID_EDIT_MYCOPY, OnEditCopyQuick)
	ON_COMMAND(ID_EDIT_MYCUT, OnEditCutQuick)
	ON_COMMAND(ID_EDIT_MYPASTE, OnEditPasteQuick)
	ON_COMMAND(ID_EDIT_MYFONT, OnEditFontQuick) //OnJustEnableCommand

	//ON_COMMAND(ID_EDIT_CopyQuick, OnEditCopyQuick)
	//ON_COMMAND(ID_EDIT_CutQuick, OnEditCutQuick)
	//ON_COMMAND(ID_EDIT_PasteQuick, OnEditPasteQuick)
	//ON_COMMAND(ID_EDIT_FontQuick, OnEditFontQuick)
	ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};


// CMainFrame construction/destruction
CMainFrame::CMainFrame()
	: m_bIsTrayIcon(false)
	, m_hAIModule(NULL)
	, AdverBarField(this, L"side")
	, AdverBar2Field(this, L"top")
{
	m_hIcon = GetApp()->LoadIcon(IDR_MAINFRAME);
}

CMainFrame::~CMainFrame()
{
	DestroyIcon(m_hIcon);
}

void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	// Add your message handler code here
	Tray(L"", true);
}

int CMainFrame::Tray(CString strTip, bool bRemove)
{
	UINT Message;

	//find message to send
	if (bRemove)
	{
		if (!m_bIsTrayIcon)
			return 0;
		Message = NIM_DELETE;
		m_bIsTrayIcon = false;
	}
	else if (m_bIsTrayIcon)
	{
		Message = NIM_MODIFY;
		m_bIsTrayIcon = true;
	}
	else
	{
		Message = NIM_ADD;
		m_bIsTrayIcon = true;
	}

	NOTIFYICONDATA t;
	t.cbSize=sizeof t;
	t.hWnd=m_hWnd;
	t.uID=1;
	t.uFlags=NIF_ICON | NIF_TIP | NIF_MESSAGE;
	t.uCallbackMessage = messageTray;
	StrCpy(t.szTip, strTip);
	t.hIcon = m_hIcon;
	return Shell_NotifyIcon(Message, &t);
}

LRESULT CMainFrame::OnTray(WPARAM /*wParam*/, LPARAM lParam)
{
	//UINT uID = (UINT)wParam;     
	CMenu menu, *pMenu;
	ATLVERIFY( menu.LoadMenu(IDM_CONTEXT_TRAY) );
	pMenu = menu.GetSubMenu(0);
	pMenu->SetDefaultItem(ID_VIEW_MAINWND);

	CPoint point;
	ATLVERIFY( GetCursorPos(&point) );
	
	switch(lParam) {
	case WM_RBUTTONUP:
		SetForegroundWindow();
		pMenu->TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, this);
		PostMessage(WM_NULL, 0, 0); 
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		SetForegroundWindow();
		ActivateWindow();
		break;
	}
	return 0;
}

void CMainFrame::ActivateWindow(void)
{
	ShowWindow(SW_SHOW);

	// Get the thread ID for the cursor owner.
    DWORD dwThreadID = GetWindowThreadProcessId(m_hWndAIForground, NULL);
    // Get the thread ID for the current thread
    DWORD dwCurrentThreadID = GetCurrentThreadId();
	AttachThreadInput(dwThreadID, dwCurrentThreadID, TRUE);
	//set active
	SetActiveWindow();
	//restore window if minimized
	if ( IsIconic() )
		ShowWindow( SW_RESTORE );
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)mOld::Font_Create(_T("Name=Tahoma; Size=20; Bold=1;")), false);
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//set window title
	SetTitle( GetApp()->m_pszAppName );

	CBitmap map;
	map.LoadBitmap(IDR_MAINFRAME);
	HBITMAP hBitmap = (HBITMAP)map.Detach();

	if (!m_wndToolBar1.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		| 0*CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar1.LoadToolBar(IDR_MAINFRAME1)) //Load mock image because MFC could not load 24bit bitmap
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar1.SetBitmap(hBitmap); //fix mock image
	
	if (!m_wndToolBar2.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		| 0*CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar2.LoadToolBar(IDR_MAINFRAME1)) //Load mock image because MFC could not load 24bit bitmap
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar2.SetBitmap(hBitmap); //fix mock image


	TBBUTTONINFO tbi;

	tbi.dwMask= TBIF_STYLE;
	tbi.cbSize= sizeof(TBBUTTONINFO);
	m_wndToolBar1.GetToolBarCtrl().GetButtonInfo(ID_EDIT_MYCUT, &tbi);
	tbi.fsStyle |= TBSTYLE_DROPDOWN | 0x0080;
	m_wndToolBar1.GetToolBarCtrl().SetButtonInfo(ID_EDIT_MYCUT, &tbi);

	tbi.dwMask= TBIF_STYLE;
	tbi.cbSize= sizeof(TBBUTTONINFO);
	m_wndToolBar1.GetToolBarCtrl().GetButtonInfo(ID_EDIT_MYPASTE, &tbi);
	tbi.fsStyle |= TBSTYLE_DROPDOWN | 0x0080;
	m_wndToolBar1.GetToolBarCtrl().SetButtonInfo(ID_EDIT_MYPASTE, &tbi);

	tbi.dwMask= TBIF_STYLE;
	tbi.cbSize= sizeof(TBBUTTONINFO);
	m_wndToolBar1.GetToolBarCtrl().GetButtonInfo(ID_EDIT_MYCOPY, &tbi);
	tbi.fsStyle |= TBSTYLE_DROPDOWN | 0x0080;
	m_wndToolBar1.GetToolBarCtrl().SetButtonInfo(ID_EDIT_MYCOPY, &tbi);

	tbi.dwMask= TBIF_STYLE;
	tbi.cbSize= sizeof(TBBUTTONINFO);
	m_wndToolBar1.GetToolBarCtrl().GetButtonInfo(ID_EDIT_MYFONT, &tbi);
	tbi.fsStyle |= TBSTYLE_DROPDOWN | 0x0080;
	m_wndToolBar1.GetToolBarCtrl().SetButtonInfo(ID_EDIT_MYFONT, &tbi);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	SetDefKeyboard();

	//add combo box
	QuickCombo_Create(&m_wndQuick1, &m_wndToolBar1);
	QuickCombo_Create(&m_wndQuick2, &m_wndToolBar2);

	//Initialize
	InitMenus();
	InitQuickCombo();
 
	//start monitor
	SetProp(m_hWnd, _T("FarsiNegar"), (HANDLE)1);
	if (GetAppOptions()->m_bEnableFastConvertor)
		AI_Start();

	//show toolbars
	ShowToolBar();

	//Creating docing state after toolbar create
	EnableDocking(CBRS_ALIGN_ANY);

	//create adver bar
	AdverBarField.Create(this, IDD_AdverBar, CBRS_ALIGN_RIGHT |CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC, IDD_AdverBar);
	AdverBarField.EnableDocking(CBRS_ALIGN_ANY);
	ShowControlBar(&AdverBarField, false, false);
	AdverBarField.AdverInitField = true;

	//create adver bar
	AdverBar2Field.Create(this, IDD_AdverBar, CBRS_ALIGN_TOP |CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC, IDD_AdverBar+1);
	AdverBar2Field.EnableDocking(CBRS_ALIGN_ANY);
	ShowControlBar(&AdverBar2Field, false, false);
	AdverBar2Field.AdverInitField = true;


	//start tray
	Tray(AfxGetAppName(), !_TOBOOL(GetAppOptions()->Flags & GlypherOptions::flagShowSysTray));
	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 2, 1))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CGlypherEditView), CSize(500, 200), pContext) ||
		!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CGlypherView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}
	m_wndSplitter.SetRowInfo(0, GetAppOptions()->EditorHeight, 0);

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	//rtl toolbar
	if ( IsAppRTL() )
	{
		cs.dwExStyle |= WS_EX_LAYOUTRTL;
	}

	// Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return TRUE;
}


// CMainFrame message handler
void CMainFrame::SetDefKeyboard(void)
{
	//activate keyboard layout
	HKL phkl[20];
	int nCount = GetKeyboardLayoutList(20, phkl);
	for (int i=0; i<nCount; i++)
	{
		LANGID wLangId = LOWORD(phkl[i]);

		//check for arabic language
		if ( PRIMARYLANGID(wLangId) ==LANG_ARABIC )
		{
			ActivateKeyboardLayout(phkl[i], 0);
		}

		//check for farsi language
		if ( PRIMARYLANGID(wLangId) ==LANG_FARSI )
		{
			ActivateKeyboardLayout(phkl[i], 0);
			break;
		}
	}
}

void CMainFrame::InitMenus(void)
{
	pMenuManager = new MenuManager();
	ATLVERIFY( pMenuManager->Main.LoadMenu(IDR_MAINFRAME) );
	ATLVERIFY( pMenuManager->ContextView.LoadMenu(IDM_CONTEXT_VIEW) );
	ATLVERIFY( pMenuManager->ToolCut.LoadMenu(IDM_CONTEXT_CUT) );
	ATLVERIFY( pMenuManager->toolCopy.LoadMenu(IDM_CONTEXT_COPY) );
	ATLVERIFY( pMenuManager->toolPaste.LoadMenu(IDM_CONTEXT_PASTE) );
	ATLVERIFY( pMenuManager->toolFont.LoadMenu(IDM_CONTEXT_FONT) );
	ATLVERIFY( pMenuManager->Convert.LoadMenu(IDM_CONTEXT_CONVERT) );
	ATLVERIFY( pMenuManager->ShMenu[MenuManager::ShMenuCopy2].LoadMenu(IDM_CONTEXT_COPY2) );
	ATLVERIFY( pMenuManager->ShMenu[MenuManager::ShMenuCut2].LoadMenu(IDM_CONTEXT_CUT2) );
	ATLVERIFY( pMenuManager->ShMenu[MenuManager::ShMenuPaste2].LoadMenu(IDM_CONTEXT_PASTE2) );

	CString name;
	FontMapsPtr pfontMaps = GetManager()->FontMapsGet();
	CMenu* pEditMenu = pMenuManager->Main.GetSubMenu(1);

	//find copy position
	int nPos = MenuManager::FindPosById(*pEditMenu, ID_STARTCOPYMENU);
	if ( nPos==-1) return;
	CMenu* pMenuCut = pEditMenu->GetSubMenu(nPos+1);
	CMenu* pMenuCopy = pEditMenu->GetSubMenu(nPos+2);
	CMenu* pMenuPaste = pEditMenu->GetSubMenu(nPos+3);
	CMenu* pMenuFont = pEditMenu->GetSubMenu(nPos+5);
	CMenu* pMenuDWFont = pMenuManager->Main.GetSubMenu(3);
	CMenu* pMenuCopyContext = pMenuManager->ContextView.GetSubMenu(0); 
	CMenu* pToolMenuCut = pMenuManager->ToolCut.GetSubMenu(0);
	CMenu* pToolMenuCopy = pMenuManager->toolCopy.GetSubMenu(0);
	CMenu* pToolMenuPaste = pMenuManager->toolPaste.GetSubMenu(0);
	CMenu* pToolMenuFont = pMenuManager->toolFont.GetSubMenu(0);
	//CMenu* pToolMenuAI = pMenuManager->toolAI.GetSubMenu(0);
	CMenu* pMenuConvertCopyFromWin = pMenuManager->Convert.GetSubMenu(0)->GetSubMenu(0);
	CMenu* pMenuConvertCopyToWin = pMenuManager->Convert.GetSubMenu(0)->GetSubMenu(1);
	CMenu* pMenuConvertPasteFromWin = pMenuManager->Convert.GetSubMenu(0)->GetSubMenu(3);
	CMenu* pMenuConvertPasteToWin = pMenuManager->Convert.GetSubMenu(0)->GetSubMenu(4);
	CMenu* pMenuConvert = pMenuManager->Convert.GetSubMenu(0)->GetSubMenu(6);

	//remove indicator menus
	pEditMenu->RemoveMenu(nPos, MF_BYPOSITION);
	pMenuDWFont->RemoveMenu(0, MF_BYPOSITION);
	pMenuCopyContext->RemoveMenu(0, MF_BYPOSITION);

	//remove first item from convertorWin
	pMenuConvertCopyToWin->RemoveMenu(0, MF_BYPOSITION);
	pMenuConvertCopyFromWin->RemoveMenu(0, MF_BYPOSITION);
	pMenuConvertPasteToWin->RemoveMenu(0, MF_BYPOSITION);
	pMenuConvertPasteFromWin->RemoveMenu(0, MF_BYPOSITION);
	pMenuConvert->RemoveMenu(0, MF_BYPOSITION);

	// add windows codepage to convertor menu
	InitConvertorMenus(pMenuConvert, 0, NULL); 
	
	//check menu
	if ( pMenuCopy==NULL && pMenuPaste==NULL )
	{
        ASSERT( false );
		return;
	}

	POSITION pos = pfontMaps->GetHeadPosition();
	int i = -1;
	while (pos!=NULL)
	{
		i++;
		FontMapPtr pFontMap = pfontMaps->GetNext(pos);
		if (!GetAppOptions()->IsFontMapEnabled(pFontMap))
			continue;

		//copy menu
		CMenuItem& menuCopy = pMenuManager->Copy[i];
		menuCopy.m_nCommand = CMenuItem::cmdCopy;
		menuCopy.m_string = _L(IDS_COPYFORFONT);
		menuCopy.m_string.Replace(_T("%1"), pFontMap->Title);
		menuCopy.pSrcFontMap = pFontMap;
		if ( i<4 ) menuCopy.m_string += _T("\tF")+Convert::ToString(i+5);
		pMenuManager->InsertItem(pMenuCopy, &menuCopy, i + MENUID_COPY_FIRST);
		pMenuManager->InsertItem(pToolMenuCopy, &menuCopy, i + MENUID_COPY_FIRST);

		//copy context menu
		menuCopy.m_nCommand = CMenuItem::cmdCopy;
		menuCopy.m_string = _L(IDS_COPYFORFONT2);
		menuCopy.m_string.Replace(_T("%1"), pFontMap->Title);
    	pMenuManager->InsertItem(pMenuCopyContext, &menuCopy, i + MENUID_COPY_FIRST);
		pMenuManager->InsertItem(pMenuManager->GetShMenu(MenuManager::ShMenuCopy2), &menuCopy, i + MENUID_COPY_FIRST);

		//cut menu
		CMenuItem& menuCut = pMenuManager->Cut[i];
		menuCut.m_nCommand = CMenuItem::cmdCut;
		menuCut.m_string = _L(IDS_CUTFORFONT);
		menuCut.m_string.Replace(_T("%1"), pFontMap->Title);
		menuCut.pSrcFontMap = pFontMap;
		if ( i<4 ) menuCut.m_string += _T("\tShift+F")+Convert::ToString(i+5);
		pMenuManager->InsertItem(pMenuCut, &menuCut, i + MENUID_CUT_FIRST);
		pMenuManager->InsertItem(pToolMenuCut, &menuCut, i + MENUID_CUT_FIRST);
		menuCut.m_string = _L(IDS_CUTFORFONT2);
		menuCut.m_string.Replace(_T("%1"), pFontMap->Title);
    	pMenuManager->InsertItem(pMenuManager->GetShMenu(MenuManager::ShMenuCut2), &menuCut, i + MENUID_CUT_FIRST);
		

		//paste menu
		CMenuItem& menuPaste = pMenuManager->Paste[i];
		menuPaste.m_nCommand = CMenuItem::cmdPaste;
		menuPaste.m_string = _L(IDS_PASTEFORFONT);
		menuPaste.m_string.Replace(_T("%1"), pFontMap->Title);
		menuPaste.pSrcFontMap = pFontMap;
		if ( i<4 ) menuPaste.m_string += _T("\tCtrl+F")+Convert::ToString(i+5);
    	pMenuManager->InsertItem(pMenuPaste, &menuPaste, i + MENUID_PASTE_FIRST);
		pMenuManager->InsertItem(pToolMenuPaste, &menuPaste, i + MENUID_PASTE_FIRST);
		menuPaste.m_string = _L(IDS_PASTEFORFONT2);
		menuPaste.m_string.Replace(_T("%1"), pFontMap->Title);
		pMenuManager->InsertItem(pMenuManager->GetShMenu(MenuManager::ShMenuPaste2), &menuPaste, i + MENUID_PASTE_FIRST);

		//font menu
		CMenuItem& menuFont = pMenuManager->Font[pMenuManager->FontIndex];
		menuFont.m_nCommand = CMenuItem::cmdFont;
		menuFont.m_string = _T("%1");
		menuFont.m_string.Replace(_T("%1"), pFontMap->Title);
		menuFont.pSrcFontMap = pFontMap;
    	pMenuManager->InsertItem(pMenuFont, &menuFont, pMenuManager->FontIndex + MENUID_FONT_FIRST);
		pMenuManager->InsertItem(pToolMenuFont, &menuFont, pMenuManager->FontIndex + MENUID_FONT_FIRST);
		pMenuManager->FontIndex++;
		

		//AutoInsert menu
		//CMenuItem& menuAI = pMenuManager->AI[AIIndex];
		//menuAI.m_nCommand = CMenuItem::cmdAI;
		//menuAI.m_string = _T("%1");
		//menuAI.m_string.Replace(_T("%1"), pFontMap->Name);
		//menuAI.pSrcFontMap = pFontMap->m_nId;
		//pMenuManager->InsertItem(pMenuAI, &menuAI, AIIndex + MENUID_AI_FIRST);
		//pMenuManager->InsertItem(pToolMenuAI, &menuAI, AIIndex + MENUID_AI_FIRST);
		//AIIndex++;

		//download fonts
		CMenuItem& menuDown = pMenuManager->DWFont[pMenuManager->DWFontIndex];
		menuDown.m_nCommand = CMenuItem::cmdDownloadFont;
		menuDown.pSrcFontMap = pFontMap;
		menuDown.m_string = _T("%1");
		menuDown.m_string.Replace(_T("%1"), pFontMap->Title);
		pMenuManager->InsertItem(pMenuDWFont, &menuDown, pMenuManager->DWFontIndex + MENUID_DWFONT_FIRST);
		pMenuManager->DWFontIndex++;

		//Windows convertor menu (CopyToWin)
		CMenuItem& menuConvertCopyToWin = pMenuManager->ConvertItem[pMenuManager->ConvertIndex];
		menuConvertCopyToWin.m_nCommand = CMenuItem::cmdConvertCopyToWin;
		menuConvertCopyToWin.pSrcFontMap = pFontMap;
		menuConvertCopyToWin.m_string = _T("%1");
		menuConvertCopyToWin.m_string.Replace(_T("%1"), pFontMap->Title);
		pMenuManager->InsertItem(pMenuConvertCopyToWin, &menuConvertCopyToWin, pMenuManager->ConvertIndex + MENUID_CONVERT_FIRST);
		pMenuManager->ConvertIndex++;

		//Windows convertor menu (CopyFromWin)
		CMenuItem& menuConvertCopyFromWin = pMenuManager->ConvertItem[pMenuManager->ConvertIndex];
		menuConvertCopyFromWin.m_nCommand = CMenuItem::cmdConvertCopyFromWin;
		menuConvertCopyFromWin.pSrcFontMap = NULL;
		menuConvertCopyFromWin.pDesFontMap = pFontMap;
		menuConvertCopyFromWin.m_string = _T("%1");
		menuConvertCopyFromWin.m_string.Replace(_T("%1"), pFontMap->Title);
		pMenuManager->InsertItem(pMenuConvertCopyFromWin, &menuConvertCopyFromWin, pMenuManager->ConvertIndex + MENUID_CONVERT_FIRST);
		pMenuManager->ConvertIndex++;

		//Windows convertor menu (PasteToWin)
		CMenuItem& menuConvertPasteToWin = pMenuManager->ConvertItem[pMenuManager->ConvertIndex];
		menuConvertPasteToWin.m_nCommand = CMenuItem::cmdConvertPasteToWin;
		menuConvertPasteToWin.pSrcFontMap = pFontMap;
		menuConvertPasteToWin.pDesFontMap = NULL;
		menuConvertPasteToWin.m_string = _T("%1");
		menuConvertPasteToWin.m_string.Replace(_T("%1"), pFontMap->Title);
		pMenuManager->InsertItem(pMenuConvertPasteToWin, &menuConvertPasteToWin, pMenuManager->ConvertIndex + MENUID_CONVERT_FIRST);
		pMenuManager->ConvertIndex++;

		//Windows convertor menu (PasteFromWin)
		CMenuItem& menuConvertPasteFromWin = pMenuManager->ConvertItem[pMenuManager->ConvertIndex];
		menuConvertPasteFromWin.m_nCommand = CMenuItem::cmdConvertPasteFromWin;
		menuConvertPasteFromWin.pSrcFontMap = NULL;
		menuConvertPasteFromWin.pDesFontMap = pFontMap;
		menuConvertPasteFromWin.m_string = _T("%1");
		menuConvertPasteFromWin.m_string.Replace(_T("%1"), pFontMap->Title);
		pMenuManager->InsertItem(pMenuConvertPasteFromWin, &menuConvertPasteFromWin, pMenuManager->ConvertIndex + MENUID_CONVERT_FIRST);
		pMenuManager->ConvertIndex++;

		//convert menu
		InitConvertorMenus(pMenuConvert, i+1, pFontMap); 
	}

	m_hMenuDefault = NULL;
	SetMenu(&pMenuManager->Main);
}

void CMainFrame::InitConvertorMenus(CMenu* pMenuConvert, int nMenuPos, FontMapPtr pfontMap)
{
	HMENU hSubMenu = CreateMenu();
	CString name = (pfontMap!=NULL) ? pfontMap->Title : _L(IDS_WINDOWS_CODEPAGE);
	pMenuConvert->InsertMenu(nMenuPos, MF_POPUP | MF_BYPOSITION, (UINT_PTR)hSubMenu, name);
    
	//insert windows codepage for destinatition
	if (pfontMap!=NULL)
	{
		CMenuItem& menuConvertItem = pMenuManager->ConvertItem[pMenuManager->ConvertIndex];
		menuConvertItem.m_nCommand = CMenuItem::cmdConvert;
		menuConvertItem.pSrcFontMap = pfontMap;
		menuConvertItem.pDesFontMap = NULL;
		menuConvertItem.m_string = _L(IDS_WINDOWS_CODEPAGE);
		pMenuManager->InsertItem(CMenu::FromHandle(hSubMenu), &menuConvertItem, pMenuManager->ConvertIndex+MENUID_CONVERT_FIRST);
		pMenuManager->ConvertIndex++;
	}
	
	//insert fontmap for destination
	FontMapsPtr pfontMaps = GetManager()->FontMapsGet();
	POSITION pos = pfontMaps->GetHeadPosition();
	while (pos!=NULL)
	{
		FontMapPtr pfontMap2 = pfontMaps->GetNext(pos);
		if (pfontMap!=NULL && pfontMap==pfontMap2)
			continue; //do not add itself
		
		CMenuItem& menuConvertItem = pMenuManager->ConvertItem[pMenuManager->ConvertIndex];
		menuConvertItem.m_nCommand = CMenuItem::cmdConvert;
		menuConvertItem.pSrcFontMap = pfontMap;
		menuConvertItem.pDesFontMap = pfontMap2;
		CString desName = (pfontMap2!=NULL) ? pfontMap2->Title : _L(IDS_WINDOWS_CODEPAGE);
		menuConvertItem.m_string = desName;
		pMenuManager->InsertItem(CMenu::FromHandle(hSubMenu), &menuConvertItem, pMenuManager->ConvertIndex+MENUID_CONVERT_FIRST);
		pMenuManager->ConvertIndex++;
	}
}

void CMainFrame::OnClose()
{
	int min;
	m_wndSplitter.GetRowInfo(0, GetAppOptions()->EditorHeight, min);
	GetAppOptions()->Save(GetApp()->GetAppRegistryKey(), L"Settings" );
	AI_Stop();
	__super::OnClose();
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// Add your message handler code here and/or call default
	if (nID==SC_CLOSE && _TOBOOL(GetAppOptions()->Flags & GlypherOptions::flagShowSysTray))
	{
		ShowWindow(SW_HIDE);
		return;
	}

	CFrameWnd::OnSysCommand(nID, lParam);
}

void CMainFrame::OnOptions()
{
	COptions_Sheet sheet(_L(IDS_CAPTION_OPTIONS), this);
	sheet.pOptions = GetAppOptions()->Clone();
	if (sheet.DoModal()==IDOK)
	{
		ApplyOptions(sheet.pOptions);
		InitMenus();
		InitQuickCombo();
	}
}

void CMainFrame::ApplyOptions(GlypherOptionsPtr pOptions)
{
	GetApp()->m_pAppOptions = pOptions;
	GetApp()->ReInitFontLoader();
	
	//options apply now
	GetGlypherDoc()->UpdateAllViews(NULL);
	Tray(AfxGetAppName(), !_TOBOOL(GetAppOptions()->Flags & GlypherOptions::flagShowSysTray));
}

void CMainFrame::OnViewKashida()
{
	GetAppOptions()->AlignMode = GlypherManager::AlignKashida;
	GetGlypherDoc()->UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateViewKashida(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck( GetAppOptions()->AlignMode==GlypherManager::AlignKashida );
}

void CMainFrame::OnViewKashidafull()
{
    GetAppOptions()->AlignMode=GlypherManager::AlignKashidaFull;
	GetGlypherDoc()->UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateViewKashidafull(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck( GetAppOptions()->AlignMode==GlypherManager::AlignKashidaFull );
}

void CMainFrame::OnViewWrap()
{
    GetAppOptions()->AlignMode=GlypherManager::AlignWrap;
	GetGlypherDoc()->UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateViewWrap(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck( GetAppOptions()->AlignMode==GlypherManager::AlignWrap );
}

void CMainFrame::OnViewSingleline()
{
    GetAppOptions()->AlignMode=GlypherManager::AlignNone;
	GetGlypherDoc()->UpdateAllViews(NULL);
}

void CMainFrame::OnUpdateViewSingleline(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck( GetAppOptions()->AlignMode==GlypherManager::AlignNone );
}

CGlypherDoc* CMainFrame::GetGlypherDoc(void)
{
	CGlypherDoc* pDoc = (CGlypherDoc*)GetActiveDocument();
	ASSERT( pDoc->IsKindOf(RUNTIME_CLASS(CGlypherDoc)) );
	return pDoc;
}

void CMainFrame::OnEditorRemoveenter()
{
	CString text = GetInputText();
	GlypherUtil::String_RemoveEnter(text);
	SetInputText(text);
}

void CMainFrame::OnEditorRemovekashida()
{
	CString text = GetInputText();
	text.Remove( 0x640 );
	SetInputText(text);
}

BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	//int idCtrl = (int)wParam;
	LPNMHDR pnmh = (LPNMHDR) lParam;
	if ( pnmh->code==TBN_DROPDOWN)
	{
		NMTOOLBAR* pNMTB = (NMTOOLBAR*)pnmh;
		CMenu menu;
		CMenu* pPopup = NULL;
		if (pNMTB->iItem==ID_EDIT_MYCUT) pPopup = pMenuManager->ToolCut.GetSubMenu(0);
		if (pNMTB->iItem==ID_EDIT_MYCOPY) pPopup = pMenuManager->toolCopy.GetSubMenu(0);
		if (pNMTB->iItem==ID_EDIT_MYPASTE) pPopup = pMenuManager->toolPaste.GetSubMenu(0);
		if (pNMTB->iItem==ID_EDIT_MYFONT) pPopup = pMenuManager->toolFont.GetSubMenu(0);
		if ( pPopup!=NULL )
		{
			CRect rcItem;
			m_wndToolBar1.GetItemRect( m_wndToolBar1.CommandToIndex(pNMTB->iItem), rcItem);
			m_wndToolBar1.ClientToScreen(rcItem);
			if ( IsAppRTL() )
			{
				pPopup->TrackPopupMenu(TPM_RIGHTBUTTON, rcItem.right, rcItem.bottom, this);
			}
			else
			{
				pPopup->TrackPopupMenu(TPM_RIGHTBUTTON, 
					rcItem.left, rcItem.bottom, this);
			}
		}
	}

	return CFrameWnd::OnNotify(wParam, lParam, pResult);
}

void CMainFrame::OnHelpWebsite()
{
	m::ShellExecute(Path(L"http://www.shetab.com"));
}

void CMainFrame::OnHelpReadme()
{
	m::ShellExecute( GetApp()->GetPath(CGlypherApp::pathReadmeFile) );
}

void CMainFrame::OnEditAnsimode()
{
	if ( _TOBOOL(GetAppOptions()->Flags & GlypherOptions::flagNoUnicodeCopy) )
	{
		GetAppOptions()->Flags &= ~GlypherOptions::flagNoUnicodeCopy;
	}
	else
	{
		//show hint
		if ( !_TOBOOL(GetAppOptions()->m_dwHints & GlypherOptions::hintAnsiMode) )
		{
			CDialog_Hint dlg(this);
			dlg.m_strText = _L(IDS_MSG_ANSIMODEHINT);
			dlg.DoModal();
			if (dlg.m_bDontShow)
				GetAppOptions()->m_dwHints |= GlypherOptions::hintAnsiMode;
		}

		GetAppOptions()->Flags |= GlypherOptions::flagNoUnicodeCopy;
	}
}

void CMainFrame::OnUpdateEditAnsimode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( true );
	pCmdUI->SetCheck( _TOBOOL(GetAppOptions()->Flags & GlypherOptions::flagNoUnicodeCopy) );
}

bool CMainFrame::AI_IsStarted(void)
{
	typedef BOOL (CALLBACK *ISMONITORSTARTED)();
	ISMONITORSTARTED pIsMonitorStarted = (ISMONITORSTARTED)GetProcAddress(m_hAIModule, "IsMonitorStarted");
	if ( pIsMonitorStarted!=NULL )
	{
		return pIsMonitorStarted()!=FALSE;
	}
	return false;
}

void CMainFrame::AI_Start(void)
{
	//return if AI already started
	if (AI_IsStarted())
		return;

	//start AI
	typedef BOOL (CALLBACK *STARTMONITOR)(HWND);
	if (m_hAIModule==NULL)
		m_hAIModule = LoadLibrary( theApp.GetPath(CGlypherApp::pathAppDllFile) );
	STARTMONITOR pStartMonitor = (STARTMONITOR)GetProcAddress(m_hAIModule, "StartMonitor");
	if ( pStartMonitor!=NULL )
		pStartMonitor(GetSafeHwnd());
}

void CMainFrame::AI_Stop(void)
{
	typedef BOOL (CALLBACK *STOPMONITOR)();
	STOPMONITOR pStopMonitor = (STOPMONITOR)GetProcAddress(m_hAIModule, "StopMonitor");
	if ( pStopMonitor!=NULL )
		pStopMonitor();
}

LRESULT CMainFrame::OnAIGrab(WPARAM wParam, LPARAM lParam)
{
	//process autoinsert if wParam is 1
	//.if (wParam==gmDoAutoInsert)
	//{
	//	//get focus window
	//	m_hWndAIForground = ::GetForegroundWindow();
	//	m_hWndAIFocus =	::GetFocus();
	//	
	//	//paste to editor if text is not empty
	//	CString strClip;
	//	m::Clipboard::PasteText(strClip);
	//	if (!strClip.IsEmpty())
	//	{
	//		GetGlypherDoc()->SetText(L"");
	//		GetGlypherDoc()->UpdateAllViews(NULL);
	//		GetGlypherDoc()->GetGlypherEditView()->Edit_PasteForFont(MENUID_PASTE_FIRST+GetAppOptions()->m_nAIMode);
	//	}

	//	// Activate Glypher Editor
	//	ActivateWindow();
	//}

	//return convertor menu handle
	if (wParam==gmNeedConvertorMenu)
	{
		bool bAnsiMode = _TOBOOL(GetAppOptions()->Flags & GlypherOptions::flagNoUnicodeCopy);
		CheckMenuItem(pMenuManager->Convert, ID_EDIT_ANSIMODE, MF_BYCOMMAND |  (bAnsiMode) ? MF_CHECKED : MF_UNCHECKED);
		EnableMenuItem(pMenuManager->Convert, ID_EDIT_ANSIMODE, MF_BYCOMMAND | MF_ENABLED);
		return (LRESULT)pMenuManager->Convert.m_hMenu;
	}

	//process convertors
	if (wParam==gmDoConvertor)
	{
		UINT itemId = (UINT)lParam;

		//change ansi mode
		if (itemId==ID_EDIT_ANSIMODE)
		{
			if ( _TOBOOL(GetAppOptions()->Flags & GlypherOptions::flagNoUnicodeCopy) )
                GetAppOptions()->Flags &= ~GlypherOptions::flagNoUnicodeCopy;
			else
				GetAppOptions()->Flags |= GlypherOptions::flagNoUnicodeCopy;
		}
		else
		{
			DoConvert(pMenuManager->Convert, itemId);
		}
	}

	return 0;
}

//void CMainFrame::AI_SendText(void)
//{
//	//return if AI is disabled
//	if (!GetAppOptions()->m_bEnableAI)
//		return;
//
//	//check is target window still exists
//	if ( !IsWindow(m_hWndAIForground) )
//		return;
//
//	//copy text to clipboard
//	if ( GetGlypherDoc()->Edit_CanCopyForFont() )
//	{
//        GetGlypherDoc()->Edit_CopyForFont(MENUID_COPY_FIRST + GetAppOptions()->m_nAIMode);
//	}
//
//
//	//set active
//	::SetActiveWindow(m_hWndAIForground);
//	::SetForegroundWindow(m_hWndAIForground);
//	::SetFocus(m_hWndAIFocus);
//
//	//Press Ctrl+V
//	Press_Paste();
//}

void CMainFrame::QuickCombo_Create(CComboBox* pwndCombo, CToolBar* pToolBar)
{
	CRect rc;
	pToolBar->GetItemRect( pToolBar->GetToolBarCtrl().GetButtonCount()-1, rc );
	rc.SetRect(rc.right+5, rc.top+6, rc.right+200, rc.bottom+1600);
	pwndCombo->Create(WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, rc, pToolBar, IDC_QUICK_COMBO);
	pwndCombo->SetFont( pToolBar->GetFont() );
}

void CMainFrame::InitQuickCombo()
{
	InitQuickCombo(&m_wndQuick1);
	InitQuickCombo(&m_wndQuick2);
}

void CMainFrame::InitQuickCombo(CComboBox* pwndCombo)
{
	pwndCombo->ResetContent();

	//add general font map
	int iAdd = pwndCombo->AddString(_L(IDS_QUICK_ALLFONTMAP));
	pwndCombo->SetItemData(iAdd, NULL);

	//add fontmaps
	POSITION pos = GetManager()->FontMapsGet()->GetHeadPosition();
	while (pos!=NULL)
	{
		FontMapPtr pfontMap = GetManager()->FontMapsGet()->GetNext(pos);
		if (GetAppOptions()->IsFontMapEnabled(pfontMap))
		{
			iAdd = pwndCombo->AddString( pfontMap->Title );
			pwndCombo->SetItemData(iAdd, (DWORD_PTR)(FontMap*)pfontMap);
		}
	}

}

void CMainFrame::OnQuickComboChange(CComboBox* pwndCombo)
{
	if ( pwndCombo->GetCurSel()!=-1 )
	{
		FontMapPtr pfontMap = (FontMap*)pwndCombo->GetItemData( pwndCombo->GetCurSel() );
		GetAppOptions()->QuickFontMapName = pfontMap!=NULL ? pfontMap->NameGet() : CString();
	}
	ShowToolBar();

	//optimize fonts if m_nQuickMode is not -1
	//if (GetAppOptions()->m_nQuickMode!=-1)
	//{
	//	FontMapPtr pFontMap = GetApp()->FontMapsField[GetAppOptions()->m_nQuickMode];
	//	GetGlypherDoc()->Font_OptimizeStdFontMap(pFontMap);
	//}

	// update all view
	GetGlypherDoc()->UpdateAllViews(NULL);
	
	// set focus to editor after combo changed
	if ( GetGlypherDoc()->GetGlypherEditView()!=NULL )
		GetGlypherDoc()->GetGlypherEditView()->SetFocus();
}

void CMainFrame::OnViewToolbar()
{
	if ( m_wndToolBar1.IsVisible() || m_wndToolBar2.IsVisible() )
	{
		ShowControlBar( &m_wndToolBar1, FALSE, FALSE );
		ShowControlBar( &m_wndToolBar2, FALSE, FALSE );
	}
	else
	{
		ShowToolBar();
	}
}

void CMainFrame::OnUpdateViewToolbar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_wndToolBar1.IsVisible() || m_wndToolBar2.IsVisible() );
}

void CMainFrame::ShowToolBar(void)
{
	//select item in combo box
	for (int i=0; i<m_wndQuick1.GetCount(); i++)
	{
		FontMapPtr pfontMap = (FontMap*)m_wndQuick1.GetItemData(i);
		if (pfontMap!=NULL && pfontMap->NameGet()==GetAppOptions()->QuickFontMapName)
		{
			m_wndQuick1.SetCurSel( i );
			m_wndQuick2.SetCurSel( i );
			break;
		}
	}
	if (GetAppOptions()->QuickFontMapName.IsEmpty())
		m_wndQuick1.SetCurSel( 0 );

	//show control bar
	if ( GetAppOptions()->QuickFontMapName.IsEmpty() )
	{
		ShowControlBar( &m_wndToolBar2, FALSE, FALSE );
		ShowControlBar( &m_wndToolBar1, TRUE, FALSE );
	}
	else
	{
		ShowControlBar( &m_wndToolBar2, TRUE, FALSE );
		ShowControlBar( &m_wndToolBar1, FALSE, FALSE );
	}
}

LRESULT CMainFrame::OnSetText(WPARAM wParam, LPARAM lParam)
{
	CString text = GetApp()->m_pszAppName;
	lParam = (LPARAM)(LPCTSTR)text;
	return DefWindowProc(WM_SETTEXT, wParam, lParam);
}

void CMainFrame::DoConvert(HMENU menuHandle, UINT id)
{
	ULONG_PTR dwItemData=0;
	pMenuManager->GetItemData(menuHandle, id, dwItemData);
	CMenuItem* pMenuItem = (CMenuItem*)dwItemData;
	if (pMenuItem==NULL)
		return;

	//store origianl clipboard data
	CStringA orgClipboardA;
	CStringW orgClipboardW;
	m::Clipboard::PasteText(orgClipboardA, orgClipboardW);

	bool bCopy = 
		pMenuItem->m_nCommand==CMenuItem::cmdConvertCopyToWin ||
		pMenuItem->m_nCommand==CMenuItem::cmdConvertCopyFromWin ||
		pMenuItem->m_nCommand==CMenuItem::cmdConvert;

	bool bPaste = 
		pMenuItem->m_nCommand==CMenuItem::cmdConvertPasteFromWin ||
		pMenuItem->m_nCommand==CMenuItem::cmdConvertPasteToWin ||
		pMenuItem->m_nCommand==CMenuItem::cmdConvert;

	bool bRestoreClipboard = 
		pMenuItem->m_nCommand==CMenuItem::cmdConvertPasteFromWin ||
		pMenuItem->m_nCommand==CMenuItem::cmdConvertPasteToWin ||
		pMenuItem->m_nCommand==CMenuItem::cmdConvert;


	// press copy (Ctrl+C) if needed
	if (bCopy)
	{
		m::Clipboard::CopyText(_T(""), true); //empty text if not any item selected
		Press_Copy();
	}

	FontMapPtr psrcMap = pMenuItem->pSrcFontMap;
	FontMapPtr pdesMap = pMenuItem->pDesFontMap;
	CString srcName = psrcMap!=NULL ? psrcMap->NameGet() : CString();
	CString desName = pdesMap!=NULL ? pdesMap->NameGet() : CString();
	bool ansiMode = _TOBOOL(GetAppOptions()->Flags&GlypherOptions::flagNoUnicodeCopy);

	CString text = GetManager()->PasteForFont(srcName, IsAppRTL(), ansiMode);
	GetManager()->CopyForFont(text, desName, NULL, 0, GlypherManager::AlignNone, IsAppRTL(), ansiMode);

	if (bPaste)
	{
		//send WM_ACTIVATE to photoshop refresh its clipboard
		CWnd* pWnd = GetForegroundWindow();
		pWnd->SendMessage(WM_ACTIVATEAPP, TRUE);
	
		//press Ctrl+V
		Press_Paste();

		//restore clipboard if needed
		if (bRestoreClipboard)
			m::Clipboard::CopyText(orgClipboardA, orgClipboardW, false);
	}
}

void CMainFrame::Press_Paste()
{
	//do for photoshop
	CString strTitle = m::Wnd::GetWindowText(::GetForegroundWindow()).MakeLower();
	if (strTitle.Find(_T("photoshop"))>=0)
	{
		::SendMessage(::GetForegroundWindow(), WM_COMMAND, MAKEWPARAM(105, 0), 0);
		WaitForInputIdleWindow( ::GetForegroundWindow() );
		return;
	}
	
	//do for other application
	//press Ctrl+V
	bool bControl = _TOBOOL(GetAsyncKeyState(VK_CONTROL)&0x8000);
	if (!bControl) keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event('V', 0, 0, 0);
	keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
	if (!bControl) keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	WaitForInputIdleWindow( ::GetForegroundWindow() );
}

void CMainFrame::Press_Copy(void)
{
	//do for photoshop
	CString strTitle = m::Wnd::GetWindowText(::GetForegroundWindow()).MakeLower();
	if (strTitle.Find(_T("photoshop"))>=0)
	{
		::SendMessage(::GetForegroundWindow(), WM_COMMAND, MAKEWPARAM(104, 0), 0);
		WaitForInputIdleWindow(::GetForegroundWindow());
		return;
	}

	//do for others windows
	//press Ctrl+C
	bool bControl = _TOBOOL(GetKeyState(VK_CONTROL)&0x8000);
	if (!bControl) keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event('C', 0, 0, 0);
	keybd_event('C', 0, KEYEVENTF_KEYUP, 0);
	if (!bControl) keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	WaitForInputIdleWindow(::GetForegroundWindow());
}

// wait until specified process of window go to idle 
void CMainFrame::WaitForInputIdleWindow(HWND hWnd, DWORD dwMilliseconds)
{
	bool bError = true;

	//wait
	DWORD dwProcess=0;
	::GetWindowThreadProcessId(hWnd, &dwProcess);
	if (dwProcess!=0)
	{
		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcess);
		::PostMessage( hWnd, RegisterWindowMessage(_T("Wait_ForInputIdle_Window")), 0, 0); //ensure application has a message to process
		Sleep(500); //let remote process begin
		DWORD res = ::WaitForInputIdle(hProcess, dwMilliseconds);
		::CloseHandle(hProcess);
		bError = res==WAIT_FAILED;
	}

	//wait if error occured and WaitForInputIdle not waited
	if (bError)
	{
		Sleep(dwMilliseconds);
	}
}

void CMainFrame::OnLangFarsi()
{
	if (GetAppOptions()->m_wLangId!=MAKELANGID(LANG_FARSI, SUBLANG_DEFAULT))
	{
		GetAppOptions()->m_wLangId = MAKELANGID(LANG_FARSI, SUBLANG_DEFAULT);
		AfxMessageBox(_L(IDS_MSG_LANGCHANGENEEDRESTART), MB_ICONINFORMATION);
	}
}

void CMainFrame::OnUpdateLangFarsi(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetAppOptions()->m_wLangId==MAKELANGID(LANG_FARSI, SUBLANG_DEFAULT));
}

void CMainFrame::OnLangEnglish()
{
	if (GetAppOptions()->m_wLangId!=MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US))
	{
		GetAppOptions()->m_wLangId = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
		AfxMessageBox(_L(IDS_MSG_LANGCHANGENEEDRESTART), MB_ICONINFORMATION);
	}
}

void CMainFrame::OnUpdateLangEnglish(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetAppOptions()->m_wLangId==MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
}

LRESULT CMainFrame::OnAINeedData(WPARAM wParam, LPARAM /*lParam*/)
{
	if (wParam==needEnableFastConvertor) return (LRESULT)GetAppOptions()->m_bEnableFastConvertor;
	return 0;
}


void CMainFrame::OnCopyAndMinimize()
{
	FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
	Edit_CopyForFont(pfontMap);
	ShowWindow(SW_MINIMIZE);
}

void CMainFrame::OnUpdateCopyAndMinimize(CCmdUI *pCmdUI)
{
	FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
	pCmdUI->Enable(Edit_CanCopyForFont(pfontMap));
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	RecalcLayout(); //let rearrange advers
}

LRESULT CMainFrame::OnAppShow(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	ShowWindow(IsIconic() ? SW_RESTORE : SW_SHOW);
	SetForegroundWindow();
	return 0;
}

void CMainFrame::OnViewMainwnd()
{
	// Add your command handler code here
	SetForegroundWindow();
	ActivateWindow();
}
