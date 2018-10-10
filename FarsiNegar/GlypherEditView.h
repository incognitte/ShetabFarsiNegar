#pragma once
#include "afxole.h"
#include "afxwin.h"


// CGlypherEditView view
class CGlypherDoc;
class CGlypherEditView : public CEditView
{
	DECLARE_DYNCREATE(CGlypherEditView)

protected:
	CGlypherEditView();
	virtual ~CGlypherEditView();

// Attributes
public:
	CGlypherDoc* GetDocument() const;

protected:
	DECLARE_MESSAGE_MAP()
public:
	void OnEnChange();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	void OnEditCopy();
	void OnUpdateEditCopy(CCmdUI *pCmdUI);
	void OnEditCut();
	void OnUpdateEditCut(CCmdUI *pCmdUI);
	void OnEditCopy2();
	void OnUpdateEditCopy2(CCmdUI *pCmdUI);
	void OnEditCut2();
	void OnUpdateEditCut2(CCmdUI *pCmdUI);
	void OnEditPaste2();
	void OnUpdateEditPaste2(CCmdUI* pCmdUI);
	
	bool IsRTL(void);

private:
	CPoint GetShMenuPos();
};

