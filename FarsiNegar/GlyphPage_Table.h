#pragma once

#include "GlyphListCtrl.h"

// CGlyphPage_Table dialog

class CGlyphPage_Table : public CPropertyPage
{
	DECLARE_DYNAMIC(CGlyphPage_Table)

	Public UnicodeGlyphInfoPtr List_GetItemData(int itemIndex)
	{
		return (UnicodeGlyphInfo*)m_wndList.GetItemData(itemIndex);
	}

	Public WCHAR List_GetGlyphCode(int itemIndex)
	{
		return List_GetItemData(itemIndex)->Code;
	}

public:
	CGlyphPage_Table();
	virtual ~CGlyphPage_Table();

// Dialog Data
	enum { IDD = IDD_GLYPHPAGE_TABLE };
	FontMapPtr GetData() { return pFontMap; }
	FontMapPtr pFontMap;
	CGlyphListCtrl m_wndList;
	
private:
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	void DoRecoverGlyphs();
	void UpdateFont(void);
	CImageList m_imageList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	HBITMAP GetGlyphBitmap(WCHAR wChar);
	void OnLvnItemDeleteItem(NMHDR *pNMHDR, LRESULT *pResult);
	void OnBnClickedCharmap();
	void OnLvnItemchangedListGlyphs(NMHDR *pNMHDR, LRESULT *pResult);
	void OnLvnItemchangingListGlyphs(NMHDR *pNMHDR, LRESULT *pResult);
	void OnBnClickedFont();
	void OnEnChangeEditData();
	static CString Convert_CharsToHexString(CString strChars);
	void OnEnChangeEditDatacode();
	bool m_bChangeEditLock;
private:
	void ShowCharsInEdit(CString strChars);
public:
	CRichEditCtrl m_richedit;
};
