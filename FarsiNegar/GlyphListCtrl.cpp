// GlyphListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Glypher.h"
#include "GlyphListCtrl.h"


// CGlyphListCtrl

IMPLEMENT_DYNAMIC(CGlyphListCtrl, CListCtrl)
CGlyphListCtrl::CGlyphListCtrl()
{
}

CGlyphListCtrl::~CGlyphListCtrl()
{
}


BEGIN_MESSAGE_MAP(CGlyphListCtrl, CListCtrl)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()



// CGlyphListCtrl message handlers


void CGlyphListCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
}
