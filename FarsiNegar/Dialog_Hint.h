#pragma once
#include "afxwin.h"


// CDialog_Hint dialog

class CDialog_Hint : public CDialog
{
	DECLARE_DYNAMIC(CDialog_Hint)

public:
	CDialog_Hint(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialog_Hint();

// Dialog Data
	enum { IDD = IDD_HINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bDontShow;
	CString m_strText;
	virtual BOOL OnInitDialog();
	CStatic m_wndImage;
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

private:
	CString m_strTitle;
	CFont m_fontTitle;
};
