// GlypherDoc.cpp : implementation of the CGlypherDoc class
//

#include "stdafx.h"
#include "Glypher.h"

#include "GlypherDoc.h"
#include "MainFrm.h"
#include "GlypherEditView.h"
#include "glypherdoc.h"

// CGlypherDoc

IMPLEMENT_DYNCREATE(CGlypherDoc, CDocument)

BEGIN_MESSAGE_MAP(CGlypherDoc, CDocument)
END_MESSAGE_MAP()


// CGlypherDoc construction/destruction

CGlypherDoc::CGlypherDoc()
{
	// TODO: add one-time construction code here
}

CGlypherDoc::~CGlypherDoc()
{
}

BOOL CGlypherDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
    
	SetInputText(L"");
	return TRUE;
}


BOOL CGlypherDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	//Add your specialized code here and/or call the base class
	try
	{
		CString text = m::Wnd::GetWindowText(*GetEditConrol());
		m::String_SaveToFile(text, Path(lpszPathName), CP_WINUNICODE, true);
		SetModifiedFlag(FALSE);
		return TRUE;
	}
	catch(CFileException* e)
	{
		e->ReportError(MB_ICONSTOP);
		e->Delete();
		return FALSE;
	}
}

BOOL CGlypherDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	try
	{
		CString text;
		m::String_LoadFromFile(text, Path(lpszPathName), 1256);
		SetInputText(text);
		return true;
	}
	catch (_com_error& /*e*/)
	{
		return false;
	}
}

CGlypherEditView* CGlypherDoc::GetGlypherEditView(void)
{
	POSITION pos = GetFirstViewPosition();
	CGlypherEditView* pView = (CGlypherEditView*)GetNextView(pos);
	return pView;
}

BOOL CGlypherDoc::SaveModified()
{
	if ( _TOBOOL(GetAppOptions()->Flags & GlypherOptions::flagPromptOnExit) )
		return __super::SaveModified();
	else
		return TRUE;
}
