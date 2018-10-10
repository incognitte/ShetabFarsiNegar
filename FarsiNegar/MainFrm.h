// MainFrm.h : interface of the CMainFrame class
//
#pragma once
#include "mytoolbar.h"
#include "GlypherOptions.h"
#include "MenuManager.h"
#include "Dialog_DownloadFont.h"
#include "AdverBar.h"
#define WM_IDLEUPDATECMDUI  0x0363  // wParam == bDisableIfNoHandler

class CGlypherDoc;
class CGlypherEditView;
class CMainFrame : public CFrameWnd
{
	Public static int const messageTray = WM_APP+1;
	Public MenuManagerPtr pMenuManager;

	Private void OnJustEnableCommand()
	{
	}

	Public bool Edit_CanCopyForFont(FontMapPtr pfontMap)
	{
		CString name = pfontMap!=NULL ? pfontMap->NameGet() : CString();
		return GetManager()->CanCopyForFont(GetInputText(), name);
	}

	Public void Edit_CopyForFont(FontMapPtr pfontMap)
	{
		HFONT fontHandle = pfontMap!=NULL ? GetAppOptions()->GetFontForFontMap(pfontMap)->HandleGet() : NULL;
		bool ansiMode = _TOBOOL(GetAppOptions()->Flags&GlypherOptions::flagNoUnicodeCopy);
		CString name = pfontMap!=NULL ? pfontMap->NameGet() : CString();
		GetManager()->CopyForFont(GetInputText(), name,  fontHandle, GetAppOptions()->AlignSize, GetAppOptions()->AlignMode, GetInputRtl(), ansiMode);
	}

	Public bool Edit_CanCutForFont(FontMapPtr pfontMap)
	{
		return Edit_CanCopyForFont(pfontMap);
	}

	Public void Edit_CutForFont(FontMapPtr pfontMap)
	{
		Edit_CopyForFont(pfontMap);
		if (GetEditConrol()!=NULL)
		{
			int startChar;
			int endChar;
			GetEditConrol()->GetSel(startChar, endChar);
			if (startChar<endChar)
			{
				GetEditConrol()->ReplaceSel(L"", TRUE);
			}
			else
			{
				SetInputText(L"");
			}
		}
	}

	Public void Edit_PasteForFont(FontMapPtr pfontMap)
	{
		if (GetEditConrol()==NULL)
			return;

		CString name = pfontMap!=NULL ? pfontMap->NameGet() : CString();
		bool ansiMode = _TOBOOL(GetAppOptions()->Flags&GlypherOptions::flagNoUnicodeCopy);
		CString text = GetManager()->PasteForFont(name, GetInputRtl(), ansiMode);

		//remove kashida
		if ( _TOBOOL(GetAppOptions()->Flags & GlypherOptions::flagNoPasteKashida) )
			text.Remove(0x640);

		//remove enter
		if ( _TOBOOL(GetAppOptions()->Flags & GlypherOptions::flagNoPasteEnter) )
			GlypherUtil::String_RemoveEnter(text);

		//clear editor
		if ( _TOBOOL(GetAppOptions()->Flags & GlypherOptions::flagClearBeforePaste) )
		{
			SetInputText(text);
		}
		else
		{
			GetEditConrol()->ReplaceSel(text, TRUE);
		}
	}

	Public bool Edit_CanPasteForFont(FontMapPtr pfontMap)
	{
		CString name = pfontMap!=NULL ? pfontMap->NameGet() : CString();
		bool ansiMode = _TOBOOL(GetAppOptions()->Flags&GlypherOptions::flagNoUnicodeCopy);
		return GetManager()->CanPasteForFont(name, ansiMode);
	}

	Public void Edit_SetFont(FontMapPtr pfontMap)
	{
		if (pfontMap==NULL)
			return Edit_SetFontForEditor();

		//retreive old log font
		LOGFONT lf = {0};
		LOGFONT* plf = NULL;
		FontHandleClassPtr pfont = GetAppOptions()->GetFontForFontMap(pfontMap);
		if (pfont!=NULL)
		{
			mOld::Font_GetLogfont(*pfont, &lf);
			plf =  &lf;
		}

		CFontDialog dlg(plf);
		if ( dlg.DoModal()==IDOK )
		{
			FontHandleClassPtr pnewFont = new FontHandleClass(CreateFontIndirect(dlg.m_cf.lpLogFont));
			GetAppOptions()->SetFontForFontMap(pfontMap, pnewFont);
			((CDocument*)GetGlypherDoc())->UpdateAllViews(NULL);
		}
	}

	Public bool Edit_CanSetFont(FontMapPtr /*pfontMap*/)
	{
		return true;
	}

	Public void Edit_DownloadFont(FontMapPtr pfontMap)
	{
		if (pfontMap==NULL)
			return;

		CDialog_DownloadFont dlg;
		dlg.pFontMap = pfontMap;
		dlg.DoModal();
	}

	Public bool Edit_CanDownloadFont(FontMapPtr pfontMap)
	{
		return pfontMap!=NULL && pfontMap->HasDownloadFontUrl();
	}

	Public void Edit_SetFontForEditor()
	{
		HFONT fontHandle = (HFONT)GetEditConrol()->SendMessage(WM_GETFONT);
		
		LOGFONT lf={0};
		LOGFONT* plf = NULL;
		if (fontHandle!=NULL)
		{
			ATLVERIFY(mOld::Font_GetLogfont(fontHandle, &lf));
			plf = &lf;
		}

		CFontDialog dlg(plf);
		if ( dlg.DoModal()==IDOK )
		{
			GetAppOptions()->m_strEditorFont = m::LogFont_ToString(dlg.m_cf.lpLogFont);
			DeleteObject(fontHandle);
			fontHandle = CreateFontIndirect(dlg.m_cf.lpLogFont);
			GetEditConrol()->SendMessage(WM_SETFONT, (WPARAM)fontHandle, TRUE);
		}
	}

	Private void OnCopyForFont(UINT nId)
	{
		CMenuItem* pmenuItem = pMenuManager->GetCopyItem(nId);
		Edit_CopyForFont(pmenuItem!=NULL ? pmenuItem->pSrcFontMap : NULL);
	}

	Private void OnUpdateCopyForFont(CCmdUI* pCmdUI)
	{
		CMenuItem* pmenuItem = pMenuManager->GetCopyItem(pCmdUI->m_nID);
		pCmdUI->Enable(Edit_CanCopyForFont(pmenuItem!=NULL ? pmenuItem->pSrcFontMap : NULL));
	}

	Private void OnCutForFont(UINT nId)
	{
		CMenuItem* pmenuItem = pMenuManager->GetCutItem(nId);
		Edit_CutForFont(pmenuItem!=NULL ? pmenuItem->pSrcFontMap : NULL);
	}

	Private void OnUpdateCutForFont(CCmdUI *pCmdUI)
	{
		CMenuItem* pmenuItem = pMenuManager->GetCutItem(pCmdUI->m_nID);
		pCmdUI->Enable(Edit_CanCutForFont(pmenuItem!=NULL ? pmenuItem->pSrcFontMap : NULL));
	}

	Private void OnPasteForFont(UINT nId)
	{
		CMenuItem* pmenuItem = pMenuManager->GetPasteItem(nId);
		Edit_PasteForFont(pmenuItem!=NULL ? pmenuItem->pSrcFontMap : NULL);
	}

	Private void OnUpdatePasteForFont(CCmdUI *pCmdUI)
	{
		CMenuItem* pmenuItem = pMenuManager->GetPasteItem(pCmdUI->m_nID);
		pCmdUI->Enable(Edit_CanPasteForFont(pmenuItem!=NULL ? pmenuItem->pSrcFontMap : NULL));
	}

	Private void OnSetFont(UINT nId)
	{
		CMenuItem* pmenuItem = pMenuManager->GetFontItem(nId);
		Edit_SetFont(pmenuItem!=NULL ? pmenuItem->pSrcFontMap : NULL);
	}

	Private void OnUpdateSetFont(CCmdUI *pCmdUI)
	{
		CMenuItem* pmenuItem = pMenuManager->GetFontItem(pCmdUI->m_nID);
		pCmdUI->Enable(Edit_CanSetFont(pmenuItem!=NULL ? pmenuItem->pSrcFontMap : NULL));
	}

	Private void OnDWFont(UINT nId)
	{
		CMenuItem* pmenuItem = pMenuManager->GetDWFontItem(nId);
		Edit_DownloadFont(pmenuItem!=NULL ? pmenuItem->pSrcFontMap : NULL);
	}

	Private void OnUpdateDWFont(CCmdUI *pCmdUI)
	{
		CMenuItem* pmenuItem = pMenuManager->GetDWFontItem(pCmdUI->m_nID);
		pCmdUI->Enable(Edit_CanDownloadFont(pmenuItem!=NULL ? pmenuItem->pSrcFontMap : NULL));
	}

	Private void OnEditEditorfont()
	{
		Edit_SetFont(NULL);
	}

	Private void OnUpdateEditEditorfont(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable(TRUE);
	}

	Private void OnEditCopyQuick()
	{
		FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
		Edit_CopyForFont(pfontMap);
	}
	
	Private void OnUpdateEditCopyQuick(CCmdUI *pCmdUI)
	{
		FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
		pCmdUI->Enable(Edit_CanCopyForFont(pfontMap));
	}

	Private void OnEditCutQuick()
	{
		FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
		Edit_CutForFont(pfontMap);
	}

	Private void OnUpdateEditCutQuick(CCmdUI *pCmdUI)
	{
		FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
		pCmdUI->Enable(Edit_CanCutForFont(pfontMap));
	}

	Private void OnEditPasteQuick()
	{
		FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
		Edit_PasteForFont(pfontMap);
	}

	Private void OnUpdateEditPasteQuick(CCmdUI *pCmdUI)
	{
		FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
		pCmdUI->Enable(Edit_CanPasteForFont(pfontMap));
	}

	Private void OnEditFontQuick()
	{
		FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
		Edit_SetFont(pfontMap);
	}

	Public void ShowAdverWindow(AdverBar* padverBar, bool show, DWORD dockBarId)
	{
		if (!padverBar->AdverInitField)
			return;

		//if (show==padverBar->AdverStateShow && dockBarId==padverBar->AdverStateDockBardId)
			//return;

		padverBar->AdverStateShow = show;
		padverBar->AdverStateDockBardId = dockBarId;
		DockControlBar(padverBar, dockBarId);
		ShowControlBar(padverBar, show, FALSE);
		RecalcLayout();
	}

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
	CSplitterWnd m_wndSplitter;

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();

protected:  // control bar embedded members
	AdverBar AdverBarField;
	AdverBar AdverBar2Field;
	CStatusBar	m_wndStatusBar;
	MyToolBar	m_wndToolBar1;
	MyToolBar	m_wndToolBar2;

private:
	LRESULT OnSetText(WPARAM /*wParam*/, LPARAM lParam);
	LRESULT OnTray(WPARAM wParam, LPARAM lParam);
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnViewMainwnd();
	int Tray(CString strTip, bool bRemove);
	void ActivateWindow(void);
	void InitConvertorMenus(CMenu* pMenuConvert, int nMenuPos, FontMapPtr pfontMap);
	void InitMenus(void);
	void InitQuickCombo(void);
	void InitQuickCombo(CComboBox* pWndCombo);
	void QuickCombo_Create(CComboBox* pWndCombo, CToolBar* pToolBar);
	HWND m_hWndAIForground;
	HWND m_hWndAIFocus;
	HMODULE m_hAIModule;
	bool m_bIsTrayIcon;
	Public CGlypherDoc* GetGlypherDoc(void);
	DECLARE_MESSAGE_MAP()

public:
	void SetDefKeyboard(void);
	void OnClose();
	void OnOptions();
	void ApplyOptions(GlypherOptionsPtr pOptions);
	void OnViewKashida();
	void OnUpdateViewKashida(CCmdUI *pCmdUI);
	void OnViewKashidafull();
	void OnUpdateViewKashidafull(CCmdUI *pCmdUI);
	void OnViewWrap();
	void OnUpdateViewWrap(CCmdUI *pCmdUI);
	void OnViewSingleline();
	void OnUpdateViewSingleline(CCmdUI *pCmdUI);
	void OnEditorRemoveenter();
	void OnEditorRemovekashida();
	void OnHelpWebsite();
	void OnHelpReadme();
	void OnEditAnsimode();
	void OnUpdateEditAnsimode(CCmdUI *pCmdUI);
	// @param wParam is one of the GrabMessage
	LRESULT OnAIGrab(WPARAM wParam, LPARAM lParam);
	LRESULT OnAINeedData(WPARAM wParam, LPARAM lParam);
	bool AI_IsStarted(void);
	void AI_Start(void);
	void AI_Stop(void);
	CComboBox m_wndQuick1;
	CComboBox m_wndQuick2;
	void OnQuickComboChange(CComboBox* pWndCombo);
	void OnCopyAndMinimize();
	void OnUpdateCopyAndMinimize(CCmdUI *pCmdUI);
	void OnViewToolbar();
	void OnUpdateViewToolbar(CCmdUI *pCmdUI);
	void ShowToolBar(void);
	HICON m_hIcon;
	void DoConvert(HMENU hMenu, UINT nId);
	static void Press_Paste();
	static void Press_Copy();
	// wait until specified process of window go to idle 
	void static WaitForInputIdleWindow(HWND hWnd, DWORD dwMilliseconds=3000);
	void OnLangFarsi();
	void OnUpdateLangFarsi(CCmdUI *pCmdUI);
	void OnLangEnglish();
	void OnUpdateLangEnglish(CCmdUI *pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnAppShow(WPARAM wParam, LPARAM lParam);
	void OnSysCommand(UINT nID, LPARAM lParam);


};


