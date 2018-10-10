// AdverBar.cpp : implementation file
//

#include "stdafx.h"
#include "Glypher.h"
#include "AdverBar.h"
#include "MainFrm.h"

// AdverBar

IMPLEMENT_DYNAMIC(AdverBar, CDialogBar)

BEGIN_MESSAGE_MAP(AdverBar, CDialogBar)
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog )
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()

LRESULT AdverBar::OnInitDialog(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	//create timer
	TimerId = SetTimer(TimerIdEvent, TimerInterval, NULL);
	TimerId2 = SetTimer(TimerIdEvent2, TimerInterval2, NULL);

	CRect rc;
	GetClientRect(rc);
	Web.Create(L"", WS_CHILD|WS_VISIBLE, rc, this, IDC_EXPLORER1);
	Web.put_RegisterAsBrowser(FALSE);
	Web.put_RegisterAsDropTarget(FALSE);

	Update();
	return 0;
}

void AdverBar::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize(nType, cx, cy);

	if (Web.GetSafeHwnd()!=NULL)
	{
		CRect rc;
		GetClientRect(rc);
		Web.MoveWindow(rc);
	}
}
BEGIN_EVENTSINK_MAP(AdverBar, CDialogBar)
	ON_EVENT(AdverBar, IDC_EXPLORER1, 252, AdverBar::NavigateComplete2Explorer1, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(AdverBar, IDC_EXPLORER1, 259, AdverBar::DocumentCompleteExplorer1, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(AdverBar, IDC_EXPLORER1, 104, AdverBar::DownloadCompleteExplorer1, VTS_NONE)
	ON_EVENT(AdverBar, IDC_EXPLORER1, 113, AdverBar::TitleChangeExplorer1, VTS_BSTR)
END_EVENTSINK_MAP()

void AdverBar::Update()
{
	pMainFrame->ShowAdverWindow(this, IsAdver(), AlignToDockId(GetAdverAlign()));
	if (!IsAdver())
		NavigateNextLocalAdver();
}

CSize AdverBar::GetAdverSize()
{
	CString title = GetHtmlTitle();
	CString sizeString = Param::Get(L"Size", L"0,0", title);
	CSize size = Convert::ToSize(sizeString);
	return size;
}

CString AdverBar::GetHtmlTitle()
{
	LPDISPATCH pdispath = Web.get_Document();
	if (pdispath==NULL)
		return CString(); //loading

	CComPtr<IHTMLDocument2> pdoc2;
	pdispath->QueryInterface(IID_IHTMLDocument2, (void**)&pdoc2);
	if (pdoc2==NULL)
		return CString(); //loading

	BSTR bstr;
	pdoc2->get_title(&bstr);
	CString title(bstr);
	SysFreeString(bstr);
	return title;
}

bool AdverBar::IsAdver()
{
	CString title = GetHtmlTitle();
	bool adver = Param::Get(L"ShetabAdver", 0, title)!=0;
	return adver;
}

CSize AdverBar::CalcDynamicLayout(int /*nLength*/, DWORD /*dwMode*/)
{
	CRect rc;
	GetParent()->GetClientRect(rc);

	CSize size = GetAdverSize();
	if (IsAdverVertical())
		return CSize(size.cx, rc.Height());
	else
		return CSize(rc.Width(), size.cy);
}

void AdverBar::NavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT* /*URL*/)
{
	pBrowserDisp = pDisp;
}

void AdverBar::DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* /*URL*/)
{
	if (pBrowserDisp==pDisp)
	{
		Update();
	}
}

void AdverBar::DownloadCompleteExplorer1()
{
}

void AdverBar::TitleChangeExplorer1(LPCTSTR /*Text*/)
{
}

void AdverBar::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent==TimerIdEvent)
	{
		UpdateAdvers();
	}
	else if (nIDEvent==TimerIdEvent2)
	{
		FirstAdver = true;
		UpdateAdvers();
		KillTimer(TimerIdEvent2);
	}

	CDialogBar::OnTimer(nIDEvent);
}

void AdverBar::NavigateNextLocalAdver()
{
	if (pLocaldAvers->IsEmpty())
		return;

	int nextStep = 1;
	if (FirstAdver)
	{
		FirstAdver = false;
		nextStep = 0;
	}

	int index = GetAppOptions()->GetLocalAdverIndex(PoolNameField)+nextStep;

	if (index>=(int)pLocaldAvers->GetCount()) index = 0;
	GetAppOptions()->SetLocalAdverIndex(PoolNameField, index);

	POSITION pos = pLocaldAvers->FindIndex(index);
	if (pos!=NULL)
	{
		Path file = pLocaldAvers->GetAt(pos);
		Web.Navigate(file, 0, 0, 0, 0);
	}
}

void AdverBar::UpdateAdvers()
{
	CString location(Web.get_LocationURL());
	location.MakeLower();
	bool local = location.Find(L"file://")==0;

	//try to connect to site time to timw if current html is not adver or it is localsite
	if (local || !IsAdver())
	{
		CString url = theApp.GetPath(CGlypherApp::pathAdverShetabUrl);
		url.Replace(L"{PoolName}", PoolNameField);
		Web.Navigate(url, 0,0,0,0);
	}
}


BOOL AdverBar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message==WM_RBUTTONDOWN)
		return TRUE;

	return CDialogBar::PreTranslateMessage(pMsg);
}
