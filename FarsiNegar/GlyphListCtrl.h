#pragma once


// CGlyphListCtrl

class CGlyphListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CGlyphListCtrl)

public:
	CGlyphListCtrl();
	virtual ~CGlyphListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};


