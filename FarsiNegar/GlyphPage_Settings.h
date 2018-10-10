#pragma once

// CGlyphPage_Settings dialog
class CGlyphPage_Settings : public CPropertyPage
{
	DECLARE_DYNAMIC(CGlyphPage_Settings)

public:
	CGlyphPage_Settings();
	virtual ~CGlyphPage_Settings();

// Dialog Data
	enum { IDD = IDD_GLYPHPAGE_SETTINGS };
	FontMapPtr pFontMap;
	FontMapPtr GetData() { return pFontMap; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void OnBnClickedCheckEng();
	void OnBnClickedCheckEngcaps();
};
