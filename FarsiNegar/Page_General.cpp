// Page_General.cpp : implementation file
//

#include "stdafx.h"
#include "Glypher.h"
#include "Page_General.h"
#include "Options_Sheet.h"
#include "MainFrm.h"

#include "PropertySheetFarsi.h"
#include "GlyphPage_Settings.h"
#include "GlyphPage_Table.h"
#include "GlyphPage_Info.h"


// CPage_General dialog

IMPLEMENT_DYNAMIC(CPage_General, CPropertyPage)
CPage_General::CPage_General()
	: CPropertyPage(CPage_General::IDD)
{
}

CPage_General::~CPage_General()
{
}

void CPage_General::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	BOOL bValue = GetData()->m_bEnableFastConvertor;
	GetData()->m_bEnableFastConvertor = bValue!=FALSE;
   
	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
	}
}


BEGIN_MESSAGE_MAP(CPage_General, CPropertyPage)
END_MESSAGE_MAP()


// CPage_General message handlers
GlypherOptionsPtr CPage_General::GetData()
{
	return GetParent()->pOptions;
}

GlypherOptionsPtr CPage_Editor::GetData()
{
	return GetParent()->pOptions;
}

GlypherOptionsPtr CPage_FontMaps::GetData()
{
	return GetParent()->pOptions;
}

BOOL CPage_General::OnApply()
{
    GetMainWnd()->ApplyOptions( GetData() );	
	return CPropertyPage::OnApply();
}

// CPage_Editor dialog

IMPLEMENT_DYNAMIC(CPage_Editor, CPropertyPage)
CPage_Editor::CPage_Editor()
	: CPropertyPage(CPage_Editor::IDD)
{
}

CPage_Editor::~CPage_Editor()
{
}

void CPage_Editor::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	m::DDX::CheckFlag(pDX->m_bSaveAndValidate, *GetDlgItem(IDC_CHECK_WRAPEDITOR), GlypherOptions::flagWrapEditor, GetData()->Flags);
	m::DDX::CheckFlag(pDX->m_bSaveAndValidate, *GetDlgItem(IDC_CHECK_NOPASTEKASHIDA), GlypherOptions::flagNoPasteKashida, GetData()->Flags);
	m::DDX::CheckFlag(pDX->m_bSaveAndValidate, *GetDlgItem(IDC_CHECK_NOPASTEENTER), GlypherOptions::flagNoPasteEnter, GetData()->Flags);
	m::DDX::CheckFlag(pDX->m_bSaveAndValidate, *GetDlgItem(IDC_CHECK_CLEAREBEFOREPASTE), GlypherOptions::flagClearBeforePaste, GetData()->Flags);
	m::DDX::CheckFlag(pDX->m_bSaveAndValidate, *GetDlgItem(IDC_CHECK_UseTray), GlypherOptions::flagShowSysTray, GetData()->Flags);
}


BEGIN_MESSAGE_MAP(CPage_Editor, CPropertyPage)
END_MESSAGE_MAP()

BOOL CPage_Editor::OnApply()
{
    GetMainWnd()->ApplyOptions( GetData() );	
	return CPropertyPage::OnApply();
}

// CPage_FontMaps dialog
IMPLEMENT_DYNAMIC(CPage_FontMaps, CPropertyPage)
CPage_FontMaps::CPage_FontMaps()
	: CPropertyPage(CPage_FontMaps::IDD)
{
}

CPage_FontMaps::~CPage_FontMaps()
{
}

void CPage_FontMaps::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_wndList);

	if (pDX->m_bSaveAndValidate)
	{
		GlypherOptions::FontMapOptionsListPtr pfontMapOptionsList = new GlypherOptions::FontMapOptionsList();
		for (int i=0; i<m_wndList.GetItemCount(); i++)
		{
			FontMapPtr pfontMap = GetFontMapByItem(i);
			
			//find old options
			GlypherOptions::FontMapOptionsPtr pold = GetData()->pFontMapOptionsList->FindRefByName(pfontMap->NameGet());
			FontHandleClassPtr pfont = (pold!=NULL) ? pold->FontGet() : NULL;
			GlypherOptions::FontMapOptionsPtr fmo = new GlypherOptions::FontMapOptions(pfontMap->NameGet(), pfontMap->FileNameGet(), m_wndList.GetCheck(i)!=0, pfont);
			pfontMapOptionsList->AddTail(fmo);
		}
		GetData()->pFontMapOptionsList = pfontMapOptionsList;
	}
	else
	{
		//select items
		for (int i=0; i<m_wndList.GetItemCount(); i++)
		{
			FontMapPtr pfontMap = GetFontMapByItem(i);
			bool bShow = GetData()->IsFontMapEnabled( pfontMap );
			m_wndList.SetCheck(i, bShow);
		}
	}
}


BEGIN_MESSAGE_MAP(CPage_FontMaps, CPropertyPage)
	ON_BN_CLICKED(IDC_UP, OnBnClickedUp)
	ON_BN_CLICKED(IDC_DOWN, OnBnClickedDown)
	ON_BN_CLICKED(IDC_DEFINE, OnBnClickedDefine)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(IDC_ADDFROMFILE, OnBnClickedAddfromfile)
	ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CPage_FontMaps::OnLvnItemchangedList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CPage_FontMaps::OnNMDblclkList)
END_MESSAGE_MAP()


// CPage_FontMaps message handlers

BOOL CPage_FontMaps::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	//Add extra initialization here

	if (IsAppRTL())
	{
		m_wndList.ModifyStyleEx(0, WS_EX_LAYOUTRTL);
		m_wndList.GetHeaderCtrl()->ModifyStyleEx(0, WS_EX_LAYOUTRTL);
	}
	ListView_SetExtendedListViewStyleEx(m_wndList, LVS_EX_CHECKBOXES, LVS_EX_CHECKBOXES);
	
	CRect rc;
	m_wndList.GetClientRect(rc);
	rc.DeflateRect(50, 0);
	m_wndList.InsertColumn(0, _L(IDS_COLUMN_FONTMAPNAME), LVCFMT_LEFT, rc.Width()-20 );
	ListFill();

	return TRUE;
}

void CPage_FontMaps::ListFill(void)
{
	m_wndList.DeleteAllItems();
	POSITION pos = GetManager()->FontMapsGet()->GetHeadPosition();
	while (pos!=NULL)
	{
		FontMapPtr pFontMap = GetManager()->FontMapsGet()->GetNext(pos);
		int iAdd = m_wndList.InsertItem(m_wndList.GetItemCount(), pFontMap->Title);
		m_wndList.SetItemData(iAdd, (DWORD_PTR)(FontMap*)pFontMap);
	}
	UpdateData(FALSE);
}

FontMapPtr CPage_FontMaps::GetFontMapByItem(UINT uItemId)
{
	if (uItemId==-1)
		return NULL;

	return (FontMap*)m_wndList.GetItemData(uItemId);
}

void CPage_FontMaps::OnBnClickedUp()
{
	int curSel = m::List_GetCurSelection(m_wndList);
	if ( curSel>0 )
	{
		FontMapPtr pselFontMap = GetFontMapByItem(curSel);
		POSITION posCur = GetManager()->FontMapsGet()->Find(pselFontMap);
		POSITION posPrev = posCur; GetManager()->FontMapsGet()->GetPrev(posPrev);
		GetManager()->FontMapsGet()->SwapElements(posCur, posPrev);
		ListFill();
		m::List_SetCurSelection(m_wndList, curSel-1);
		EnableButtons();
	}
}

void CPage_FontMaps::OnBnClickedDown()
{
	int curSel = m::List_GetCurSelection(m_wndList);
	if ( curSel+1<m_wndList.GetItemCount() )
	{
		FontMapPtr pselFontMap = GetFontMapByItem(curSel);
		POSITION posCur = GetManager()->FontMapsGet()->Find(pselFontMap);
		POSITION posNext = posCur; GetManager()->FontMapsGet()->GetNext(posNext);
		if (posNext!=NULL)
		{
			GetManager()->FontMapsGet()->SwapElements(posCur, posNext);
			ListFill();
			m::List_SetCurSelection(m_wndList, curSel+1);
			EnableButtons();
		}
	}
}

void CPage_FontMaps::EnableButtons(void)
{
	int curSel = m::List_GetCurSelection(m_wndList);
	FontMapPtr pfontMap = curSel!=-1 ? GetFontMapByItem(curSel) : NULL;
	bool buildinFontMap = pfontMap!=NULL && pfontMap->FileNameGet().IsRelativeToFolder(GetApp()->GetPath(CGlypherApp::pathFontmapFolder));

	GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
	GetDlgItem(IDC_ADDFROMFILE)->EnableWindow( TRUE );
	GetDlgItem(IDC_REMOVE)->EnableWindow( curSel>=0 && !buildinFontMap );
	GetDlgItem(IDC_DEFINE)->EnableWindow( curSel>=0 );

	GetDlgItem(IDC_UP)->EnableWindow(curSel>0);
	GetDlgItem(IDC_DOWN)->EnableWindow(curSel<m_wndList.GetItemCount()-1);
}

void CPage_FontMaps::OnBnClickedDefine()
{
	CWaitCursor wc;
	int iSelect = m::List_GetCurSelection(m_wndList);
	if ( iSelect<0 )
		return;

	FontMapPtr pfontMap = (FontMap*)m_wndList.GetItemData(iSelect);
	if ( pfontMap==NULL )
		return;

	//create property sheet
	CString caption = _L(IDS_CAPTION_FONTMAPPROP);
	caption.Replace(_T("%1"), pfontMap->Title);
	CPropertySheetFarsi sheet(caption, this);
	sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	//create pages
	CGlyphPage_Table pageTable;
	CGlyphPage_Settings pageSettings;
	CGlyphPage_Info pageInfo;
	pageTable.pFontMap = pfontMap;
	pageSettings.pFontMap = pfontMap;
	pageInfo.pFontMap = pfontMap;
	sheet.AddPage(&pageTable);
	sheet.AddPage(&pageSettings);
	sheet.AddPage(&pageInfo);


	if (sheet.DoModal()==IDOK)
	{
		try
		{
			pfontMap->Save(GetApp()->GetPath(CGlypherApp::pathSortXsdFile));
		}
		catch (_com_error& e)
		{
			CString msg = L"Could not save %1.\nError=%2";
			msg.Replace(L"%1", pfontMap->FileNameGet());
			msg.Replace(L"%2", (LPCTSTR)e.Description());
			theApp.DoMessageBox(msg, MB_ICONWARNING, 0);
		}
	}
	else
	{
		pfontMap->Reload();
	}

	//refresh list
	ListFill();
	for (int i=0; i<m_wndList.GetItemCount(); i++)
	{
		if ( (FontMap*)m_wndList.GetItemData(i)==pfontMap )
			m::List_SetCurSelection(m_wndList, i);
	}
	EnableButtons();
}


void CPage_FontMaps::OnBnClickedAdd()
{
	Path defFileName(L"New Fontmap.fontmap");
	CString filter = CString(FILTER_FONTMAP) + CString(FILTER_ALLFILES) + L"|";
	CFileDialog dlg(FALSE, FONTMAP_FILEEXT, defFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, this);
	m::File::CreateDirectory( GetApp()->GetPath(CGlypherApp::pathFontmapFolder) );

	if ( dlg.DoModal()==IDOK)
	{
		FontMapPtr pfontMap = GetApp()->CreateNewFontmap( Path(dlg.GetPathName()) );
		if (pfontMap!=NULL)
		{
			GetManager()->FontMapsGet()->AddTail(pfontMap);
			ListFill();
		}
	}
}


void CPage_FontMaps::OnBnClickedRemove()
{
	int curSel = m::List_GetCurSelection(m_wndList); 
	if ( curSel>=0 )
	{
		FontMapPtr pfontMap = GetFontMapByItem(curSel);

		//prompt to remove
		CString msg = _L(IDS_PROMPT_REMOVEFONTMAP);
		msg.Replace(_T("%1"), pfontMap->Title );
		int res = AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION);
		if ( res!=IDYES )
			return;

		POSITION pos = GetManager()->FontMapsGet()->Find(pfontMap);
		GetManager()->FontMapsGet()->RemoveAt(pos);
		ListFill();
		m::List_SetCurSelection(m_wndList, curSel); 
		EnableButtons();
	}
}

void CPage_FontMaps::OnBnClickedAddfromfile()
{
	CString msg;
	CString filter = CString(FILTER_FONTMAP) + CString(FILTER_ALLFILES) + L"|";
	CFileDialog dlg(TRUE, FONTMAP_FILEEXT, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, filter, this);
	if ( dlg.DoModal()==IDOK)
	{
		try
		{
			FontMapPtr pfontMap = new FontMap(Path(dlg.GetPathName()));
			//check is fontmap already exists
			if ( GetManager()->FontMapsGet()->FindByName(pfontMap->NameGet())!=NULL )
			{
				msg = _L(IDS_FONTMAP_ALREADYEXISTS);
				msg.Replace(_T("%1"), pfontMap->FileNameGet());
				msg.Replace(_T("%2"), pfontMap->Title);
				AfxMessageBox(msg, MB_ICONSTOP);
			   return;
			}
			GetManager()->FontMapsGet()->AddTail(pfontMap);
		}
		catch (_com_error& e)
		{
			msg = L"Could not load %1.\nError=%2";
			msg.Replace(L"%1", dlg.GetPathName());
			msg.Replace(L"%2", (LPCTSTR)e.Description());
			theApp.DoMessageBox(msg, MB_ICONWARNING, 0);
		}

		ListFill();
	}
}


void CPage_FontMaps::OnLvnItemchangedList(NMHDR* /*pNMHDR*/, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	EnableButtons();
	*pResult = 0;
}

void CPage_FontMaps::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = (LPNMITEMACTIVATE)(pNMHDR);
	if (pNMItemActivate->iItem==-1)
		return;
	OnBnClickedDefine();
	*pResult = 0;
}
