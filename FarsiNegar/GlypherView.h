// GlypherView.h : interface of the CGlypherView class
//


#pragma once

class CGlypherDoc;
class CMapFont;
class CGlypherView : public CScrollView
{
protected: // create from serialization only
	CGlypherView();
	DECLARE_DYNCREATE(CGlypherView)

public:
	CGlypherDoc* GetDocument() const;
	Public void Draw(HDC dchandle);
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	CRect GetRulerRect();

protected:
	DECLARE_MESSAGE_MAP()

// Implementation
public:
	virtual ~CGlypherView();

private:
	void DrawRuler(HDC dcHandle);
	void DrawRulerGuide(HDC dcHandle, CPoint ptMouse);
	void DrawWrapGuide(HDC dcHandle);
	CRect m_rcGuide;
	GCP_RESULTS gcpField; //use for cash
	int& GetWrapSize();

public:
	virtual void OnInitialUpdate();
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void Pos_ToLogical(CPoint* pPoint);
	void Pos_ToClient(CPoint* pPoint);
	void Pos_ToClient(CRect* pRect);
	void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	void OnLButtonUp(UINT nFlags, CPoint point);
private:
	bool m_bStartWrapSize;
public:
	// @return size of text
	CSize DrawFontMapText(HDC dcHandle, CString text, CPoint point, FontMapPtr pfontMap);
	// @param strUnicodeGlyphs string in standard unicode glyph
	// @return size of text
	CSize DrawGlyphsLine(HDC dcHandle, CString glyphs, CPoint point);
	CSize DrawFontMapSection(HDC dcHandle, CPoint point, FontMapPtr pfontMap);

private:
	DWORD GetViewFlags(void);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
