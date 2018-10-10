#pragma once

#define IDC_QUICK_COMBO		5000

// MyToolBar

class MyToolBar : public CToolBar
{
	DECLARE_DYNAMIC(MyToolBar)

	Public MyToolBar()
	{
	}

	Public virtual ~MyToolBar()
	{
	}

	Protected DECLARE_MESSAGE_MAP()
	Public void OnCbnSelendokCombo();
};


