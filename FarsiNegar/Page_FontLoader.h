#pragma once
#include "afxcmn.h"


//class CPage_FontLoader
class COptions_Sheet;
class CPage_FontLoader : public CPropertyPage
{
	DECLARE_DYNAMIC(CPage_FontLoader)

public:
	void List_Fill(void);
	CPage_FontLoader();
	virtual ~CPage_FontLoader();

// Dialog Data
	enum { IDD = IDD_PAGE_FONTLOADER };
	COptions_Sheet* GetParent() { return (COptions_Sheet*)CPropertyPage::GetParent(); }
	GlypherOptionsPtr GetData();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_wndList;
	virtual BOOL OnInitDialog();
	void OnBnClickedBrowse();
	void OnLvnDeleteitemFontlist(NMHDR *pNMHDR, LRESULT *pResult);
};
