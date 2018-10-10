#pragma once
#include "CWebBrowser2.h"

enum AlignEnum
{
	AlignTop,
	AlignLeft,
	AlignRight,
	AlignBottom,
};

// AdverBar
class AdverBar : public CDialogBar
{
	DECLARE_DYNAMIC(AdverBar)
	Public bool AdverInitField;
	Public bool AdverStateShow;
	Public DWORD AdverStateDockBardId;

	Private void NavigateNextLocalAdver();
	Private static const int TimerIdEvent = 1;
	Private static const int TimerInterval = 5 * 60 * 1000;
	Private static const int TimerIdEvent2 = 2;
	Private static const int TimerInterval2 = 10 * 1000;
	Private UINT_PTR TimerId;
	Private UINT_PTR TimerId2;

	Private CString PoolNameField;
	Public AdverBar(CMainFrame* pmainFrame, CString poolName)
	{
		TimerId = 0;
		pMainFrame = pmainFrame;
		Path adverPoolFolder (theApp.GetPath(CGlypherApp::pathAdverLocalFolder) + Path(poolName));
		pLocaldAvers = m::File::Find(adverPoolFolder, L"*.htm", m::File::FindFlagFile | m::File::FindFlagRecursive);
		pLocaldAvers->AddTailList(m::File::Find(adverPoolFolder, L"*.html", m::File::FindFlagFile | m::File::FindFlagRecursive));
		AdverStateDockBardId = 0xFFFFFFFF;
		AdverStateShow = false;
		AdverInitField = false;
		PoolNameField = poolName;
		FirstAdver = false;
	}

	Private PathListPtr pLocaldAvers;
	Private CString GetHtmlTitle();

	Private CSize GetAdverSize();
	Private bool IsAdver();
	Private CMainFrame* pMainFrame;
	Private int LocalAdverIndex;
	Public void Update();

	Private bool IsRTL()
	{
		LANGID langId = LANGIDFROMLCID(GetThreadLocale());
		return PRIMARYLANGID(langId)==LANG_FARSI;
	}

	Public AlignEnum GetAdverAlign()
	{
		CString align = Param::Get(L"Align", L"", GetHtmlTitle());
		align.MakeLower();
		if (align==L"left") return IsRTL() ? AlignRight : AlignLeft;
		else if (align==L"right") return IsRTL() ? AlignLeft : AlignRight;
		else if (align==L"top") return AlignTop;
		else if (align==L"bottom") return AlignBottom;

		CSize size = GetAdverSize();
		if (size==CSize(0,0))
			return AlignRight;
		
		if (size.cy==0)
			return AlignRight;

		if (size.cx==0)
			return AlignBottom;

		if (size.cx>size.cy)
			return AlignBottom;

		return AlignRight;
	}

	Public bool IsAdverVertical()
	{
		AlignEnum align = GetAdverAlign();
		return align==AlignLeft || align==AlignRight;
	}

	Public static DWORD AlignToDockId(AlignEnum align)
	{
		switch(align){
		case AlignTop: return AFX_IDW_DOCKBAR_TOP;
		case AlignBottom: return AFX_IDW_DOCKBAR_BOTTOM;
		case AlignLeft: return AFX_IDW_DOCKBAR_LEFT;
		default: return AFX_IDW_DOCKBAR_RIGHT;
		}
	}

public:
	virtual ~AdverBar()
	{
	}

	CWebBrowser2 Web;

	Private LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	Public CSize CalcDynamicLayout(int nLength, DWORD dwMode);

Private void UpdateAdvers();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_EVENTSINK_MAP()
	void NavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT* URL);
	void DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL);
	void DownloadCompleteExplorer1();
	void TitleChangeExplorer1(LPCTSTR Text);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	Private IDispatch* pBrowserDisp;
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	Private bool FirstAdver;
};


