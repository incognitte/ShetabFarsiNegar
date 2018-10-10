// GlyphPage_Table.cpp : implementation file
//

#include "stdafx.h"
#include "Glypher.h"
#include "GlyphPage_Table.h"
#include "glyphpage_table.h"

#define LVS_EX_BORDERSELECT     0x00008000 // border selection style instead of highlight

// CGlyphPage_Table dialog

IMPLEMENT_DYNAMIC(CGlyphPage_Table, CPropertyPage)
CGlyphPage_Table::CGlyphPage_Table()
	: CPropertyPage(CGlyphPage_Table::IDD)
	, m_bChangeEditLock(false)
{
	pFontMap = NULL;
	ATLVERIFY( m_imageList.Create(50, 50, m::GetSystemBitCount(), 500, 5) );
}

CGlyphPage_Table::~CGlyphPage_Table()
{
}

void CGlyphPage_Table::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, pFontMap->Title);
	DDX_Control(pDX, IDC_LIST_GLYPHS, m_wndList);

	if (pDX->m_bSaveAndValidate)
	{
		//save glyph data
		int iSelect = m::List_GetCurSelection( m_wndList );
		if (iSelect>=0)
		{
			WCHAR glyphCode = List_GetGlyphCode(iSelect);
			CString hexString = m::Wnd::GetDlgItemText(*this, IDC_EDIT_DATACODE);
			CString fonyGlyphs = UnicodeGlyphInfo::CodeToData(hexString);
			bool inverse = m::Button_GetCheck(*this, IDC_CHECK_REVERT)!=0;
			UnicodeGlyphMapPtr pgm = new UnicodeGlyphMap(fonyGlyphs, inverse);
			pFontMap->UnicodeGlyphMapsGet()->SetAt(glyphCode, pgm);
		}
	}
	else
	{
		Path charMapFile = m::GetSystemDirectory() + Path(L"charmap.exe");
		GetDlgItem(IDC_CHARMAP)->EnableWindow( charMapFile.IsFile() );

		//load glyph data
		int iSelect = m::List_GetCurSelection( m_wndList );
		if (iSelect>=0)
		{
			WCHAR glyphCode = List_GetGlyphCode(iSelect);
			CString fontGlyphs;
			UnicodeGlyphMapPtr pgm = pFontMap->FindUnicodeGlyphMapByUnicodeGlyph(glyphCode);
			if (pgm!=NULL)
				fontGlyphs = pgm->FontGlyphs;
			SetDlgItemText(IDC_EDIT_DATACODE, UnicodeGlyphInfo::DataToCode(fontGlyphs) );
			ShowCharsInEdit(fontGlyphs);
			m::Button_SetCheck(*this, IDC_CHECK_REVERT, pgm!=NULL && pgm->CanInverse);
		}
	}
}


BEGIN_MESSAGE_MAP(CGlyphPage_Table, CPropertyPage)
	ON_BN_CLICKED(IDC_CHARMAP, OnBnClickedCharmap)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GLYPHS, OnLvnItemchangedListGlyphs)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_GLYPHS, OnLvnItemchangingListGlyphs)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_GLYPHS, OnLvnItemDeleteItem)
	ON_BN_CLICKED(IDC_FONT, OnBnClickedFont)
	ON_EN_CHANGE(IDC_EDIT_DATA, OnEnChangeEditData)
	ON_EN_CHANGE(IDC_EDIT_DATACODE, OnEnChangeEditDatacode)
END_MESSAGE_MAP()


// CGlyphPage_Table message handlers

BOOL CGlyphPage_Table::OnInitDialog()
{
	ASSERT( pFontMap!=NULL ); //not initialized correctly

	CRect rcEdit;
	CWnd* pWnd = GetDlgItem(IDC_EDIT_DATA);
	pWnd->GetWindowRect(rcEdit);
	ScreenToClient(rcEdit);
	pWnd->DestroyWindow();
	m_richedit.CreateEx(0x204, WS_CHILD | WS_BORDER | WS_VISIBLE, rcEdit, this, IDC_EDIT_DATA);
	m_richedit.SetEventMask( m_richedit.GetEventMask() | ENM_CHANGE );
	CPropertyPage::OnInitDialog();
	//Add extra initialization here


	UpdateFont();

	ListView_SetExtendedListViewStyleEx(m_wndList, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_BORDERSELECT, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_BORDERSELECT);
	CRect rc;
	m_wndList.GetClientRect(rc);
	rc.DeflateRect(20,20);
	m_wndList.InsertColumn(0, _L(IDS_COLUMN_LETTERS), LVCFMT_CENTER, rc.Width()*1/6);
	m_wndList.InsertColumn(1, _L(IDS_COLUMN_LETTERS), LVCFMT_CENTER, rc.Width()*1/6);
	m_wndList.InsertColumn(2, _L(IDS_COLUMN_Code), LVCFMT_CENTER, rc.Width()*1/6);
	m_wndList.InsertColumn(3, _L(IDS_COLUMN_Desc), LVCFMT_CENTER, rc.Width()*3/6);
	m_wndList.SetImageList(&m_imageList, LVSIL_SMALL);
	m_wndList.SetItemCount((int)GetGlyphInfos()->GetCount());

	//insert glyphs to table
	POSITION pos = GetGlyphInfos()->GetStartPosition();
	while (pos!=NULL)
	{
		UnicodeGlyphInfoPtr pgi = GetGlyphInfos()->GetNextValue(pos);
		bool invisible = _TOBOOL( pgi->Flags & UnicodeGlyphInfo::FlagInvisible);
		bool advanced = _TOBOOL( pgi->Flags & UnicodeGlyphInfo::FlagAdvanced);
		if ( !invisible && !advanced)
		{
			HBITMAP hGlyphBmp = GetGlyphBitmap(pgi->Code);
			int iImage = ImageList_AddMasked(m_imageList, hGlyphBmp, 0);
			DeleteObject( hGlyphBmp );
			int iAdd = m_wndList.InsertItem(0, _T(""), iImage);
			m_wndList.SetItemText(iAdd, 1, CString(pgi->Data));
			m_wndList.SetItemText(iAdd, 2, Convert::ToString(pgi->Code, _T("%X")));
			m_wndList.SetItemText(iAdd, 3, pgi->Name);
			m_wndList.SetItemData(iAdd, (DWORD_PTR)pgi.Detach());
		}
	}
	m_wndList.SortItems(CompareFunc, 0);

	return TRUE;
}

HBITMAP CGlyphPage_Table::GetGlyphBitmap(WCHAR wChar)
{
	CRect rc( CPoint(0,0), mOld::ImageList_GetIconSize(m_imageList));
	HBITMAP hRet=NULL;

	//get desktop DC
	HDC hDesktopDC = ::GetDC(NULL);
	if (hDesktopDC==NULL)
		return NULL;

	CDC memDC;
	CBitmap bitmap, *pOldBmp;
	CFont font, *pOldFont;
	LOGFONT lf = m::LogFont_FromString(L"FaceName=Tahoma; Size=12; Bold=0;");
	font.CreateFontIndirect(&lf);
	CBrush br(RGB(0,0,0));

	if ( memDC.CreateCompatibleDC(NULL)!=NULL && bitmap.CreateCompatibleBitmap(CDC::FromHandle(hDesktopDC), rc.Width(), rc.Height())!=NULL )
	{
		pOldBmp = memDC.SelectObject(&bitmap);
		pOldFont = memDC.SelectObject(&font);

		//check is glyph exists
		SCRIPT_CACHE sc=NULL;
		WORD outGlyphIndex;
		HRESULT hr = ScriptGetCMap(memDC, &sc, &wChar, 1, 0, &outGlyphIndex);

		if (hr==S_FALSE)
		{
			memDC.SelectObject(pOldFont);
			font.DeleteObject();
			LOGFONT lf2 = m::LogFont_FromString(_T("FaceName=Courier New; Size=12; Bold=1;"));
			font.CreateFontIndirect(&lf2);
			pOldFont = memDC.SelectObject(&font);
		}


		CSize sizeChar;
		GetTextExtentPoint32W(memDC, &wChar, 1, &sizeChar);
		CPoint ptPos( (rc.Width()-sizeChar.cx)/2, (rc.Height()-sizeChar.cy)/2);
		
		memDC.FillSolidRect(rc, RGB(255,255,255));
		rc.DeflateRect(1,1);
		memDC.FrameRect(rc, &br);
		rc.InflateRect(1,1);
		//ExtTextOut(memDC, ptPos.x, ptPos.y, ETO_GLYPH_INDEX, rc, (TCHAR*)&outGlyphIndex, 1, NULL);
		ExtTextOut(memDC, ptPos.x, ptPos.y, 0*ETO_GLYPH_INDEX, rc, &wChar, 1, NULL);


		hRet = BitmapTools::Create(memDC, CRect(0, 0, rc.Width(), rc.Height()), m::GetSystemBitCount());
		memDC.SelectObject(pOldBmp);
		memDC.SelectObject(pOldFont);
	}
	::ReleaseDC(NULL, hDesktopDC);
	return hRet;
}

void CGlyphPage_Table::OnBnClickedCharmap()
{
	Path charMapFile = m::GetSystemDirectory() + Path(L"charmap.exe");

	CWnd* pWnd = CWnd::FindWindow(NULL, L"Character Map" );
	if ( pWnd!=NULL )
	{
		pWnd->ShowWindow(SW_NORMAL);
		pWnd->SetForegroundWindow();
	}
	else
        m::ShellExecute(charMapFile);
}

int CGlyphPage_Table::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/)
{
	UnicodeGlyphInfoPtr pgli1 = (UnicodeGlyphInfo*)lParam1;
	UnicodeGlyphInfoPtr pgli2 = (UnicodeGlyphInfo*)lParam2;
	LCID lcid = MAKELCID(MAKELANGID(LANG_FARSI, SUBLANG_DEFAULT), SORT_DEFAULT);
	return CompareString(lcid,0, pgli1->Data, pgli1->Data.GetLength(), pgli2->Data, pgli2->Data.GetLength())-CSTR_EQUAL;
}


void CGlyphPage_Table::OnLvnItemDeleteItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	//Add your control notification handler code here
	UnicodeGlyphInfo* pgi = (UnicodeGlyphInfo*)pNMLV->lParam;
	pgi->Release();
	*pResult = 0;
}

void CGlyphPage_Table::OnLvnItemchangedListGlyphs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	//Add your control notification handler code here

	if ( pNMLV->uNewState & LVIS_SELECTED )
	{
		UpdateData(FALSE);
	}

	*pResult = 0;
}

void CGlyphPage_Table::OnLvnItemchangingListGlyphs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// Add your control notification handler code here

	if ( pNMLV->uOldState & LVIS_SELECTED )
	{
		UpdateData(TRUE);
	}

	*pResult = 0;
}

void CGlyphPage_Table::OnBnClickedFont()
{
	FontHandleClassPtr pfont = pFontMap->DefaultFontGet();
	LOGFONT lf = {0};
	if (pfont!=NULL)
		mOld::Font_GetLogfont(*pfont, &lf);
	CFontDialog dlg(pfont!=NULL ? &lf : NULL);
	if ( dlg.DoModal()==IDOK )
	{
		pFontMap->DefaultFontSet( new FontHandleClass(CreateFontIndirect(&lf)) );
		UpdateFont();
	}
}

void CGlyphPage_Table::UpdateFont(void)
{
	GetDlgItem(IDC_EDIT_DATA)->SendMessage(WM_SETFONT, (WPARAM)pFontMap->DefaultFontGet()->HandleGet(), TRUE);
}


void CGlyphPage_Table::DoRecoverGlyphs()
{
	//if (!UpdateData(TRUE))
	//	return;

	////get list index
	//int iIndex = m::List_GetCurSelection(m_wndList);
	//if (iIndex<0)
	//	return;

	////get list char
	//WCHAR wUnicodeGlyph = List_GetGlyphCode(iIndex);
	//CString strRecover = TryRecoverGlyphs(wUnicodeGlyph);
 //   
	////read other gm flags if found
	//CGlyphMap gm;
	//pFontMap->Glyph_FromUnicode(wUnicodeGlyph, gm);
	////set new gm
	//gm.FontGlyphs = strRecover;
	//pFontMap->m_glyphMaps.SetAt(wUnicodeGlyph, gm);
 //   
	////update data to list
	//UpdateData(FALSE);
}

void CGlyphPage_Table::OnEnChangeEditData()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// Add your control notification handler code here
	
	//convert edit to data code
	CString strChars = m::Wnd::GetDlgItemText(*this, IDC_EDIT_DATA);
	if ( !m_bChangeEditLock )
	{
		m_bChangeEditLock = true;
		SetDlgItemText(IDC_EDIT_DATACODE, Convert_CharsToHexString(strChars));
		m_bChangeEditLock = false;
	}
}

void CGlyphPage_Table::OnEnChangeEditDatacode()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString hexString = m::Wnd::GetDlgItemText(*this, IDC_EDIT_DATACODE);
	if ( !m_bChangeEditLock )
	{
		m_bChangeEditLock = true;
		ShowCharsInEdit(UnicodeGlyphInfo::CodeToData(hexString));
		m_bChangeEditLock = false;
	}
}

CString CGlyphPage_Table::Convert_CharsToHexString(CString strChars)
{
	CString ret;
	for (int i=0; i<strChars.GetLength(); i++)
		ret += Convert::ToString(strChars[i], _T("%x,"));
	ret.Trim(_T(","));
	return ret;
}


void CGlyphPage_Table::ShowCharsInEdit(CString strChars)
{
	CString str = strChars;
    SetDlgItemText(IDC_EDIT_DATA, str );
}
