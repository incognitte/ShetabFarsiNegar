// GlypherEditView.cpp : implementation file
//

#include "stdafx.h"
#include "Glypher.h"

#include "GlypherDoc.h"
#include "MainFrm.h"
#include "dialog_hint.h"
#include "glyphereditview.h"


// CGlypherEditView
CEdit* pEditControl;

IMPLEMENT_DYNCREATE(CGlypherEditView, CEditView)

CGlypherEditView::CGlypherEditView()
{
}

CGlypherEditView::~CGlypherEditView()
{
}

BEGIN_MESSAGE_MAP(CGlypherEditView, CEditView)
	ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
	ON_WM_CREATE()
	ON_WM_STYLECHANGED()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_COPY2, OnEditCopy2)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY2, OnUpdateEditCopy2)
	ON_COMMAND(ID_EDIT_CUT2, OnEditCut2)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT2, OnUpdateEditCut2)
	ON_COMMAND(ID_EDIT_PASTE2, OnEditPaste2)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE2, OnUpdateEditPaste2)
END_MESSAGE_MAP()


// CGlypherEditView diagnostics

CGlypherDoc* CGlypherEditView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGlypherDoc)));
	return (CGlypherDoc*)m_pDocument;
}


void CGlypherEditView::OnEnChange()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEditView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	GetDocument()->SetModifiedFlag( m::Wnd::GetWindowText(*this)!=L"" );
	GetDocument()->UpdateAllViews(NULL);
}

BOOL CGlypherEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL ret = CEditView::PreCreateWindow(cs);;
	if (!IsAppRTL())
		cs.dwExStyle |= WS_EX_RIGHT | WS_EX_RTLREADING | WS_EX_LEFTSCROLLBAR;

	//remove editor wrap
	if ( _TOBOOL(GetAppOptions()->Flags & GlypherOptions::flagWrapEditor) )
	{
		cs.style &= ~WS_HSCROLL;
		cs.style &= ~ES_AUTOHSCROLL;
	}
	return ret;
}


int CGlypherEditView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEditView::OnCreate(lpCreateStruct) == -1)
		return -1;

	pEditControl = &GetEditCtrl();
	CString fontString = GetAppOptions()->m_strEditorFont;
	if ( !fontString.IsEmpty() )
	{
		//create new font
		LOGFONT lf = m::LogFont_FromString(fontString);
		CFont font;
		font.CreateFontIndirect( &lf );
		SetFont(&font);
		font.Detach();
	}

	return 0;
}


void CGlypherEditView::OnEditCopy()
{
	int nStart,nEnd;
	GetEditCtrl().GetSel(nStart, nEnd);

	//select all text
	if (nStart == nEnd)
		GetEditCtrl().SetSel(0, -1, TRUE);

	//copy
	__super::OnEditCopy();
	
	//restore select
	if (nStart == nEnd)
		GetEditCtrl().SetSel(nStart, nEnd, TRUE);
}


void CGlypherEditView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	CString text;
	GetWindowText(text);
	pCmdUI->Enable(!text.IsEmpty());
}

void CGlypherEditView::OnEditCut()
{
	int nStart,nEnd;
	GetEditCtrl().GetSel(nStart, nEnd);

	//select all text
	if (nStart == nEnd)
		GetEditCtrl().SetSel(0, -1, TRUE);
	
	CEditView::OnEditCut();
}

void CGlypherEditView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	CString text;
	GetWindowText(text);
	pCmdUI->Enable(!text.IsEmpty());
}


void CGlypherEditView::OnEditCopy2()
{
	//show menu for general mode
	if (GetAppOptions()->QuickFontMapName.IsEmpty())
	{
		CPoint pt = GetShMenuPos();
		UINT flags = !_TOBOOL(GetMainWnd()->GetExStyle()&WS_EX_LAYOUTRTL)  ? TPM_RIGHTALIGN  : 0;
		GetMainWnd()->pMenuManager->GetShMenu(MenuManager::ShMenuCopy2)->TrackPopupMenu( flags, pt.x, pt.y, GetMainWnd());
	}
	else
	{
		FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
		GetMainWnd()->Edit_CopyForFont(pfontMap);
	}
}

void CGlypherEditView::OnUpdateEditCopy2(CCmdUI *pCmdUI)
{
	FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
	pCmdUI->Enable(GetMainWnd()->Edit_CanCopyForFont(pfontMap));
}

void CGlypherEditView::OnEditCut2()
{
	//show menu for general mode
	if (GetAppOptions()->QuickFontMapName.IsEmpty())
	{
		CPoint pt = GetShMenuPos();
		UINT flags = !_TOBOOL(GetMainWnd()->GetExStyle()&WS_EX_LAYOUTRTL)  ? TPM_RIGHTALIGN  : 0;
		GetMainWnd()->pMenuManager->GetShMenu(MenuManager::ShMenuCut2)->TrackPopupMenu( flags, pt.x, pt.y, GetMainWnd());
	}
	else
	{
		FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
		GetMainWnd()->Edit_CutForFont(pfontMap);
	}
}

void CGlypherEditView::OnUpdateEditCut2(CCmdUI *pCmdUI)
{
	FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
	pCmdUI->Enable(GetMainWnd()->Edit_CanCutForFont(pfontMap));
}

void CGlypherEditView::OnEditPaste2()
{
	//show menu for general mode
	if (GetAppOptions()->QuickFontMapName.IsEmpty())
	{
		CPoint pt = GetShMenuPos();
		UINT flags = !_TOBOOL(GetMainWnd()->GetExStyle()&WS_EX_LAYOUTRTL)  ? TPM_RIGHTALIGN  : 0;
		GetMainWnd()->pMenuManager->GetShMenu(MenuManager::ShMenuPaste2)->TrackPopupMenu( flags, pt.x, pt.y, GetMainWnd());
	}
	else
	{
		FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
		GetMainWnd()->Edit_PasteForFont(pfontMap);
	}
}

void CGlypherEditView::OnUpdateEditPaste2(CCmdUI *pCmdUI)
{
	FontMapPtr pfontMap = GetManager()->FontMapsGet()->FindRefByName(GetAppOptions()->QuickFontMapName);
	pCmdUI->Enable(GetMainWnd()->Edit_CanPasteForFont(pfontMap));
}

bool CGlypherEditView::IsRTL(void)
{
	return _TOBOOL( GetExStyle()&WS_EX_RTLREADING);
}

void CGlypherEditView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	if (m_pDocument!=NULL)
		GetDocument()->UpdateAllViews(this);
	CEditView::OnStyleChanged(nStyleType, lpStyleStruct);
}

CPoint CGlypherEditView::GetShMenuPos()
{
	CPoint pt = GetCaretPos();
	ClientToScreen(&pt);
	return pt;
}
