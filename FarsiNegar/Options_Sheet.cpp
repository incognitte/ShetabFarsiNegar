// Options_Sheet.cpp : implementation file
//

#include "stdafx.h"
#include "Glypher.h"
#include "Options_Sheet.h"


// COptions_Sheet

IMPLEMENT_DYNAMIC(COptions_Sheet, CPropertySheetFarsi)
COptions_Sheet::COptions_Sheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheetFarsi(nIDCaption, pParentWnd, iSelectPage)
{
}

COptions_Sheet::COptions_Sheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheetFarsi(pszCaption, pParentWnd, iSelectPage)
{
	Constructor();
}

COptions_Sheet::~COptions_Sheet()
{
	Constructor();
}


BEGIN_MESSAGE_MAP(COptions_Sheet, CPropertySheetFarsi)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// COptions_Sheet message handlers

void COptions_Sheet::Constructor(void)
{
	//AddPage( &m_pageGeneral );
	AddPage( &m_pageEditor );
	AddPage( &m_pagePreview );
	AddPage( &m_pageFontLoader );
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}
