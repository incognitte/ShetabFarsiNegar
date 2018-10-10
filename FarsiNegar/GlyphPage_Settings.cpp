// GlyphPage_Settings.cpp : implementation file
//

#include "stdafx.h"
#include "Glypher.h"
#include "GlyphPage_Settings.h"
#include "glyphpage_settings.h"


// CGlyphPage_Settings dialog

IMPLEMENT_DYNAMIC(CGlyphPage_Settings, CPropertyPage)
CGlyphPage_Settings::CGlyphPage_Settings()
	: CPropertyPage(CGlyphPage_Settings::IDD)
{
	pFontMap = NULL;
}

CGlyphPage_Settings::~CGlyphPage_Settings()
{
}

void CGlyphPage_Settings::DoDataExchange(CDataExchange* pDX)
{
	ASSERT( pFontMap!=NULL );  //not initialized at yet
	CPropertyPage::DoDataExchange(pDX);
	m::DDX::CheckFlag(pDX->m_bSaveAndValidate, *GetDlgItem(IDC_CHECK_ENG), FontMap::FlagHasEngGlyphs, pFontMap->Flags);
	m::DDX::CheckFlag(pDX->m_bSaveAndValidate, *GetDlgItem(IDC_CHECK_ENGCAPS), FontMap::FlagHasEngCapGlyphs, pFontMap->Flags);
	m::DDX::CheckFlag(pDX->m_bSaveAndValidate, *GetDlgItem(IDC_CHECK_UNICODERANGE), FontMap::FlagUnicodeRange, pFontMap->Flags);
	m::DDX::CheckFlag(pDX->m_bSaveAndValidate, *GetDlgItem(IDC_CHECK_INVERTGLYPH), FontMap::FlagInvertGlyphs, pFontMap->Flags);
	m::DDX::CheckFlag(pDX->m_bSaveAndValidate, *GetDlgItem(IDC_CHECK_UNICODECOPYNT), FontMap::FlagUseUnicodeClipboard, pFontMap->Flags);
	m::DDX::CheckFlag(pDX->m_bSaveAndValidate, *GetDlgItem(IDC_CHECK_EXISTINVIEW), FontMap::FlagShowInViewAsDefault, pFontMap->Flags);
}


BEGIN_MESSAGE_MAP(CGlyphPage_Settings, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_ENG, OnBnClickedCheckEng)
	ON_BN_CLICKED(IDC_CHECK_ENGCAPS, OnBnClickedCheckEngcaps)
END_MESSAGE_MAP()


// CGlyphPage_Settings message handlers

void CGlyphPage_Settings::OnBnClickedCheckEng()
{
	BOOL bValue = m::Button_GetCheck(*this, IDC_CHECK_ENG);
	if ( AfxMessageBox(_L(IDS_MSG_DOUWANTCHANGEGLYPH), MB_YESNO | MB_ICONQUESTION)==IDYES ) 
		pFontMap->SetEngLetters( bValue!=FALSE );
	else
		m::Button_SetCheck(*this, IDC_CHECK_ENG, _TOBOOL(pFontMap->Flags&FontMap::FlagHasEngGlyphs));
}

void CGlyphPage_Settings::OnBnClickedCheckEngcaps()
{
	BOOL bValue = m::Button_GetCheck(*this, IDC_CHECK_ENGCAPS);
	if ( AfxMessageBox(_L(IDS_MSG_DOUWANTCHANGEGLYPH), MB_YESNO | MB_ICONQUESTION)==IDYES ) 
		pFontMap->SetEngCapsLetters( bValue!=FALSE );
	else
		m::Button_SetCheck(*this, IDC_CHECK_ENGCAPS, _TOBOOL(pFontMap->Flags&FontMap::FlagHasEngCapGlyphs));
}
