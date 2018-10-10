#include "stdafx.h"
#include "Glypher.h"
#include "MyToolBar.h"
#include "MainFrm.h"

IMPLEMENT_DYNAMIC(MyToolBar, CToolBar)


BEGIN_MESSAGE_MAP(MyToolBar, CToolBar)
	ON_CBN_SELENDOK(IDC_QUICK_COMBO, OnCbnSelendokCombo)
END_MESSAGE_MAP()

void MyToolBar::OnCbnSelendokCombo()
{
    GetMainWnd()->OnQuickComboChange( (CComboBox*)GetDlgItem(IDC_QUICK_COMBO) );
}

