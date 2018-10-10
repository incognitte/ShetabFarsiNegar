#pragma once
#include "afxcmn.h"


// CPage_General dialog
class COptions_Sheet;
class GlypherOptions;
class CPage_General : public CPropertyPage
{
	DECLARE_DYNAMIC(CPage_General)

public:
	CPage_General();
	virtual ~CPage_General();

// Dialog Data
	enum { IDD = IDD_PAGE_GENERAL };
	COptions_Sheet* GetParent() { return (COptions_Sheet*)CPropertyPage::GetParent(); }
	GlypherOptionsPtr GetData();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnApply();
};


// CPage_Editor dialog

class CPage_Editor : public CPropertyPage
{
	DECLARE_DYNAMIC(CPage_Editor)

public:
	CPage_Editor();
	virtual ~CPage_Editor();

// Dialog Data
	enum { IDD = IDD_PAGE_EDITOR };
	COptions_Sheet* GetParent() { return (COptions_Sheet*)CPropertyPage::GetParent(); }
	GlypherOptionsPtr GetData();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnApply();
};
#pragma once


// CPage_FontMaps dialog

class CPage_FontMaps : public CPropertyPage
{
	DECLARE_DYNAMIC(CPage_FontMaps)

public:
	CPage_FontMaps();
	virtual ~CPage_FontMaps();

	HFONT CloneFont(HFONT fontHandle)
	{
		LOGFONT lf;
		TESTLASTERR( mOld::Font_GetLogfont(fontHandle, &lf)!=0 );
		return CreateFontIndirect(&lf);
	}

// Dialog Data
	enum { IDD = IDD_PAGE_FONTMAP };
	COptions_Sheet* GetParent() { return (COptions_Sheet*)CPropertyPage::GetParent(); }
	GlypherOptionsPtr GetData();
	void OnBnClickedUp();
	void OnBnClickedDown();
	void OnBnClickedDefine();
	void OnBnClickedAdd();
	void OnBnClickedAddfromfile();
	void OnBnClickedRemove();
	void EnableButtons(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_wndList;
	void ListFill(void);
private:
	FontMapPtr GetFontMapByItem(UINT uItemId);
public:
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
};
