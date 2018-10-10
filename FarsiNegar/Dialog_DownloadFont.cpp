// Dialog_DownloadFont.cpp : implementation file
//

#include "stdafx.h"
#include "Glypher.h"
#include "Dialog_DownloadFont.h"
#include "dialog_downloadfont.h"


// CDialog_DownloadFont dialog

IMPLEMENT_DYNAMIC(CDialog_DownloadFont, CDialog)
CDialog_DownloadFont::CDialog_DownloadFont(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog_DownloadFont::IDD, pParent)
	, pFontMap(NULL)
{
	m_hLinkFont = NULL;
	m_hOpenFolderIcon = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_OPENFOLDER), IMAGE_ICON, 16, 16, 0);
}

CDialog_DownloadFont::~CDialog_DownloadFont()
{
	DeleteObject(m_hLinkFont);
	DestroyIcon(m_hOpenFolderIcon);
}

void CDialog_DownloadFont::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
		//set links for download
		CString strLink1 = pFontMap->DownloadFontUrl1;
		SetDlgItemText(IDC_STATIC_LINK1, strLink1);
		if ( strLink1.IsEmpty() ) ::ShowWindow( ::GetDlgItem(m_hWnd, IDC_STATIC_LINK1), SW_HIDE );

		CString strLink2 = pFontMap->DownloadFontUrl2;
		SetDlgItemText(IDC_STATIC_LINK2, strLink2);
		if ( strLink2.IsEmpty() ) ::ShowWindow( ::GetDlgItem(m_hWnd, IDC_STATIC_LINK2), SW_HIDE );

		CString strLink3 = pFontMap->DownloadFontUrl3;
		SetDlgItemText(IDC_STATIC_LINK3, strLink3);
		if ( strLink3.IsEmpty() ) ::ShowWindow( ::GetDlgItem(m_hWnd, IDC_STATIC_LINK3), SW_HIDE );

		//set note
		SetDlgItemText(IDC_EDIT_NOTE, pFontMap->Note);
		if ( pFontMap->Note.IsEmpty() ) ::ShowWindow( ::GetDlgItem(m_hWnd, IDC_EDIT_NOTE), SW_HIDE );

	}
}


BEGIN_MESSAGE_MAP(CDialog_DownloadFont, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_OPENFONTFOLDER, OnBnClickedOpenfontfolder)
END_MESSAGE_MAP()


// CDialog_DownloadFont message handlers

BOOL CDialog_DownloadFont::OnInitDialog()
{
	ASSERT( pFontMap!=NULL );
	CDialog::OnInitDialog();
	// Add extra initialization here

	CString strText = m::Wnd::GetDlgItemText(*this, IDC_STATIC_TEXT);
	strText.Replace(_T("%1"), pFontMap->Title );
	SetDlgItemText(IDC_STATIC_TEXT, strText);

	LOGFONT lf;
	GetFont()->GetLogFont(&lf);
	lf.lfUnderline = TRUE;
	m_hLinkFont = CreateFontIndirect(&lf);

	//set open folder icon
	GetDlgItem(IDC_OPENFONTFOLDER)->SendMessage(BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)m_hOpenFolderIcon);
	return TRUE;
}

HBRUSH CDialog_DownloadFont::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	//Change any attributes of the DC here
	switch(pWnd->GetDlgCtrlID()){
	case IDC_STATIC_LINK1:
	case IDC_STATIC_LINK2:
	case IDC_STATIC_LINK3:
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SelectObject( CFont::FromHandle(m_hLinkFont) );
		break;

	case IDC_STATIC_FRAME2:
	case IDC_STATIC_ICON:
		return (HBRUSH)GetStockObject(GRAY_BRUSH);

	case IDC_STATIC_TIP1:
	case IDC_STATIC_TIP2:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(WHITE_BRUSH);
	}

	//Return a different brush if the default is not desired
	return hbr;
}

BOOL CDialog_DownloadFont::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint pt = GetCurrentMessage()->pt;
	if ( PtInControl(IDC_STATIC_LINK1, pt) || 
		PtInControl(IDC_STATIC_LINK2, pt) || 
		PtInControl(IDC_STATIC_LINK3, pt))
	{
		SetCursor( LoadCursor(NULL, IDC_HAND) );
		return 1;
	}

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

bool CDialog_DownloadFont::PtInControl(UINT nControlID, CPoint point)
{
	CWnd* pWnd = GetDlgItem(nControlID);
	if (pWnd==NULL)
		return false;

	if (!pWnd->IsWindowVisible())
		return false;

	CRect rc;
	pWnd->GetWindowRect(rc);
	return rc.PtInRect(point)!=FALSE;
}

void CDialog_DownloadFont::OnLButtonDown(UINT nFlags, CPoint point)
{
	//Add your message handler code here and/or call default
	CPoint pt = point;
	ClientToScreen(&pt);

	if ( PtInControl(IDC_STATIC_LINK1, pt) )
	{
		Path url(m::Wnd::GetDlgItemText(*this, IDC_STATIC_LINK1));
		m::ShellExecute( url );
		return;
	}

	if ( PtInControl(IDC_STATIC_LINK2, pt) )
	{
		Path url(m::Wnd::GetDlgItemText(*this, IDC_STATIC_LINK2));
		m::ShellExecute( url );
		return;
	}

	if ( PtInControl(IDC_STATIC_LINK3, pt) )
	{
		Path url(m::Wnd::GetDlgItemText(*this, IDC_STATIC_LINK3));
		m::ShellExecute( url );
		return;
	}

	__super::OnLButtonDown(nFlags, point);
}

void CDialog_DownloadFont::OnBnClickedOpenfontfolder()
{
	Path folder = GetAppOptions()->FontFolderField;
	m::ShellExecute(folder);
}
