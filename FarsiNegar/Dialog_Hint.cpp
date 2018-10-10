// Dialog_Hint.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "Dialog_Hint.h"
#include "dialog_hint.h"
#include "Core\GlypherUtil.h"


// CDialog_Hint dialog

IMPLEMENT_DYNAMIC(CDialog_Hint, CDialog)
CDialog_Hint::CDialog_Hint(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog_Hint::IDD, pParent)
	, m_bDontShow(FALSE)
    , m_strText(_T(""))
	, m_strTitle(_T(""))
{
	m_fontTitle.Attach( mOld::Font_Create(_T("Name=Tahoma; Size=8; Bold=1;")) );
}

CDialog_Hint::~CDialog_Hint()
{
}

void CDialog_Hint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_DONTSHOW, m_bDontShow);
	DDX_Text(pDX, IDC_STATIC_MESSAGE, m_strText);
	DDX_Control(pDX, IDC_STATIC_ICON, m_wndImage);
	DDX_Text(pDX, IDC_STATIC_TITLE, m_strTitle);
}


BEGIN_MESSAGE_MAP(CDialog_Hint, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDialog_Hint message handlers

BOOL CDialog_Hint::OnInitDialog()
{
	//find first line
	int nFind = m_strText.Find(_T('\n') );
	if (nFind!=-1)
	{
		m_strTitle = m_strText.Left(nFind);
		m_strText = m_strText.Mid(nFind+1);
	}
	
	CDialog::OnInitDialog();

	// Add extra initialization here
	GetDlgItem(IDC_STATIC_TITLE)->SetFont( &m_fontTitle );
	return TRUE;
}

HBRUSH CDialog_Hint::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// Change any attributes of the DC here
	switch(pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC_TITLE:
	case IDC_STATIC_MESSAGE:
		pDC->SetBkColor(RGB(255,255,255));
		return (HBRUSH)GetStockObject(WHITE_BRUSH);

	case IDC_STATIC_ICON:
		return (HBRUSH)GetStockObject(GRAY_BRUSH);
	};

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
