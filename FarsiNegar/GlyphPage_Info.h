#pragma once

// CGlyphPage_Info dialog

class CGlyphPage_Info : public CPropertyPage
{
	DECLARE_DYNAMIC(CGlyphPage_Info)

public:
	CGlyphPage_Info();
	virtual ~CGlyphPage_Info();

// Dialog Data
	enum { IDD = IDD_GLYPHPAGE_INFO };
	FontMapPtr pFontMap;
	FontMapPtr GetData() { return pFontMap; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
