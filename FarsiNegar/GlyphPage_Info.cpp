// GlyphPage_Info.cpp : implementation file
//

#include "stdafx.h"
#include "Glypher.h"
#include "GlyphPage_Info.h"


// CGlyphPage_Info dialog

IMPLEMENT_DYNAMIC(CGlyphPage_Info, CPropertyPage)
CGlyphPage_Info::CGlyphPage_Info()
	: CPropertyPage(CGlyphPage_Info::IDD)
{
}

CGlyphPage_Info::~CGlyphPage_Info()
{
}

void CGlyphPage_Info::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LINK1, GetData()->DownloadFontUrl1);
	DDX_Text(pDX, IDC_EDIT_LINK2, GetData()->DownloadFontUrl2);
	DDX_Text(pDX, IDC_EDIT_LINK3, GetData()->DownloadFontUrl3);
	DDX_Text(pDX, IDC_EDIT_NOTE, GetData()->Note);
}


BEGIN_MESSAGE_MAP(CGlyphPage_Info, CPropertyPage)
END_MESSAGE_MAP()


// CGlyphPage_Info message handlers
