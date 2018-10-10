// PropertySheetFarsi.cpp : implementation file
//

#include "stdafx.h"
#include "Glypher.h"
#include "PropertySheetFarsi.h"


// CPropertySheetFarsi

IMPLEMENT_DYNAMIC(CPropertySheetFarsi, CPropertySheet)
CPropertySheetFarsi::CPropertySheetFarsi(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPropertySheetFarsi::CPropertySheetFarsi(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CPropertySheetFarsi::~CPropertySheetFarsi()
{
}


BEGIN_MESSAGE_MAP(CPropertySheetFarsi, CPropertySheet)
END_MESSAGE_MAP()


// CPropertySheetFarsi message handlers

BOOL CPropertySheetFarsi::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	// Add your specialized code here

	if (IsAppRTL())
	{
		GetTabControl()->ModifyStyleEx(0, WS_EX_LAYOUTRTL);
		ModifyStyleEx(0, WS_EX_RIGHT | WS_EX_RTLREADING);
	}

	SetDlgItemText(IDOK, _L(IDS_OK));
	SetDlgItemText(IDCANCEL, _L(IDS_CANCEL));
	SetDlgItemText(ID_APPLY_NOW, _L(IDS_APPLY_NOW));

	return bResult;
}
