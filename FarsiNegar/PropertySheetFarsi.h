#pragma once



// CPropertySheetFarsi

class CPropertySheetFarsi : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropertySheetFarsi)

public:
	CPropertySheetFarsi(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPropertySheetFarsi(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CPropertySheetFarsi();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};


