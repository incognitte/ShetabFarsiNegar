// Page_FontLoader.cpp : implementation file
//

#include "stdafx.h"
#include "Glypher.h"
#include "Options_Sheet.h"
#include "Page_FontLoader.h"


// CPage_FontLoader dialog

IMPLEMENT_DYNAMIC(CPage_FontLoader, CPropertyPage)
CPage_FontLoader::CPage_FontLoader()
	: CPropertyPage(CPage_FontLoader::IDD)
{
}

CPage_FontLoader::~CPage_FontLoader()
{
}

void CPage_FontLoader::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FONTLIST, m_wndList);
	DDX_Text(pDX, IDC_EDIT_FONTFOLDER, GetData()->FontFolderField);
	
	if (pDX->m_bSaveAndValidate)
	{
		//save to m_strFontLoader
		GetData()->m_strFontLoader.Empty();
		for (int i=0; i<m_wndList.GetItemCount(); i++)
		{
			CString folderName = *(CString*)m_wndList.GetItemData(i);
			Param::Set(folderName, m_wndList.GetCheck(i), GetData()->m_strFontLoader);
		}
	}
	else
	{
		//set from m_strFontLoader
		for (int i=0; i<m_wndList.GetItemCount(); i++)
		{
			CString folderName = *(CString*)m_wndList.GetItemData(i);
			bool bChecked = Param::Get(folderName, 1, GetData()->m_strFontLoader)!=0; //default checked
			m_wndList.SetCheck(i, bChecked);
		}
	}
}


BEGIN_MESSAGE_MAP(CPage_FontLoader, CPropertyPage)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
	ON_NOTIFY(LVN_DELETEITEM, IDC_FONTLIST, OnLvnDeleteitemFontlist)
END_MESSAGE_MAP()


// CPage_FontLoader message handlers
void CPage_FontLoader::List_Fill(void)
{
	m_wndList.DeleteAllItems();
	PathListPtr pfolders = m::File::Find(GetData()->FontFolderField, L"*.*", m::File::FindFlagFolder);
	POSITION pos = pfolders->GetHeadPosition();
	while (pos!=NULL)
	{
		Path folder = pfolders->GetNext(pos);
		CString folderName = folder.GetFileName();
		CString name = folderName;
		int iAdd = m_wndList.InsertItem(m_wndList.GetItemCount(), name);
		m_wndList.SetItemData(iAdd, (DWORD_PTR)new CString(folderName));
	}
	UpdateData(FALSE);
}

GlypherOptionsPtr CPage_FontLoader::GetData()
{
	return GetParent()->pOptions;
}

BOOL CPage_FontLoader::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add extra initialization here
	if (IsAppRTL())
	{
		m_wndList.ModifyStyleEx(0, WS_EX_LAYOUTRTL);
		m_wndList.GetHeaderCtrl()->ModifyStyleEx(0, WS_EX_LAYOUTRTL);
	}

	ListView_SetExtendedListViewStyleEx(m_wndList, LVS_EX_CHECKBOXES, LVS_EX_CHECKBOXES);
	CRect rc;
	m_wndList.GetClientRect(rc);
	m_wndList.InsertColumn(0, _L(IDS_COLUMN_FONTNAME), LVCFMT_LEFT, rc.Width()-25);

	List_Fill();


	return TRUE;
}

void CPage_FontLoader::OnBnClickedBrowse()
{
	m::BrowseForFolder(GetData()->FontFolderField, _L(IDS_SELECTFONTFOLDER), m_hWnd);
	List_Fill();
}

void CPage_FontLoader::OnLvnDeleteitemFontlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// Add your control notification handler code here
	delete (CString*)pNMLV->lParam;
	*pResult = 0;
}
