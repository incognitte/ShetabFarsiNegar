#pragma once


// CDialog_DownloadFont dialog

class CDialog_DownloadFont : public CDialog
{
	DECLARE_DYNAMIC(CDialog_DownloadFont)

public:
	CDialog_DownloadFont(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialog_DownloadFont();

// Dialog Data
	enum { IDD = IDD_DOWNLOADFONT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	FontMapPtr pFontMap;
	virtual BOOL OnInitDialog();
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	// @param point position relative to screen coordinate
	bool PtInControl(UINT nControlID, CPoint point);
	HFONT m_hLinkFont;
	void OnLButtonDown(UINT nFlags, CPoint point);

private:
	HICON m_hOpenFolderIcon;
public:
	void OnBnClickedOpenfontfolder();
};
