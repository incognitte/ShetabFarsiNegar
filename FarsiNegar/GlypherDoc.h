// GlypherDoc.h : interface of the CGlypherDoc class
//
#pragma once
class CGlypherEditView;
class CGlypherDoc : public CDocument
{
protected: // create from serialization only
	CGlypherDoc();
	DECLARE_DYNCREATE(CGlypherDoc)

// Overrides
public:
	virtual BOOL OnNewDocument();

// Implementation
public:
	virtual ~CGlypherDoc();

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	CGlypherEditView* GetGlypherEditView(void);

public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

protected:
	virtual BOOL SaveModified();
};

