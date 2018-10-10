#pragma once

#include "PropertySheetFarsi.h"
#include "GlypherOptions.h"
#include "page_general.h"
#include "page_fontloader.h"

// COptions_Sheet

class COptions_Sheet : public CPropertySheetFarsi
{
	DECLARE_DYNAMIC(COptions_Sheet)

public:
	COptions_Sheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptions_Sheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~COptions_Sheet();

    CPage_FontLoader m_pageFontLoader;
    CPage_General m_pageGeneral;
    CPage_Editor m_pageEditor;
	CPage_FontMaps m_pagePreview;
	GlypherOptionsPtr pOptions;

protected:
	DECLARE_MESSAGE_MAP()

private:
	void Constructor(void);
};


