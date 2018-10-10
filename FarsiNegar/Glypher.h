// Glypher.h : main header file for the Glypher application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "global.h"
#include "Core\GlypherManager.h"
#include "GlypherOptions.h"
#include "LockTime.h"
#include "FontLoader.h"
using namespace GlypherCore;

//menu item
#define MENUID_COPY_FIRST		2000
#define MENUID_COPY_LAST		2050
#define MENUID_PASTE_FIRST		2100
#define MENUID_PASTE_LAST		2150
#define MENUID_CUT_FIRST		2200
#define MENUID_CUT_LAST			2250
#define MENUID_FONT_FIRST		2300
#define MENUID_FONT_LAST		2350
#define MENUID_AI_FIRST			2400
#define MENUID_AI_LAST			2450
#define MENUID_DWFONT_FIRST		2500
#define MENUID_DWFONT_LAST		2550
#define MENUID_CONVERT_FIRST	3000
#define MENUID_CONVERT_LAST		4000

typedef LONG (CALLBACK *CHECKLOCK)(LPCSTR pszParam);
typedef LPCWSTR (CALLBACK *GETREGISTRATIONINFOW)();

// CGlypherApp:
class CMainFrame;
class GlypherOptions;
class CGlypherApp : public CWinApp
{
public:
	enum PathEnum{
		pathExeFile,
		pathAppFolder,
		pathAppDllFile,
		pathConfigFile,
		pathFontmapFolder,
		pathUnicodeGlyphFile,
		pathDocFolder,
		pathReadmeFile,
		pathAppFontFolder,
		pathFileConvertorFile,
		pathSortXsdFile,
		pathAdverLocalFolder,
		pathAdverShetabUrl,
	};


	void LoadFontMaps();

	Private void App_SetAppName(CString appName)
	{
		free((void*)AfxGetApp()->m_pszAppName);
		AfxGetApp()->m_pszAppName = _tcsdup( appName );
		AfxGetModuleState()->m_lpszCurrentAppName = AfxGetApp()->m_pszAppName;
	}

	Private void App_SetRegistryKey(CString regKey)
	{
		ATLASSERT( regKey.Left(9).CompareNoCase(_T("Software\\"))==0 );
		regKey.Delete(0, 9);

		//{CWinApp::SetRegistryKey body
		free((void*)AfxGetApp()->m_pszRegistryKey);
		AfxGetApp()->m_pszRegistryKey = _tcsdup( regKey );
		free((void*)AfxGetApp()->m_pszProfileName);
		AfxGetApp()->m_pszProfileName = _tcsdup( _T("") );
		//}{CWinApp::SetRegistryKey body
	}

public:
	CGlypherApp();
	~CGlypherApp();

// Overrides
public:
	Path GetPath(PathEnum path);
	GlypherOptionsPtr m_pAppOptions;
	CFontLoader m_fontLoader;

	// @return NULL if failed
	FontMapPtr CreateNewFontmap(Path fileName);

	virtual BOOL InitInstance();
	void OnAppAbout();

protected:
	DECLARE_MESSAGE_MAP()

private:

public:
	// this method mey show dialog and announce use to register
	// @param bExpiredOnly show dialog if time is expired and application not registerd at yet
	// @return 0 if continue; 1 if continue but time expired; 2 if user choose exit applicaton
	virtual int ExitInstance();
	void ReInitFontLoader(void);
	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);

private:
	static UINT __cdecl ReInitFontLoaderThread( LPVOID pParam );

public:
	void UpdateAppName(void);

	Private GlypherManagerPtr pGlypherManagerField;
	Public GlypherManagerPtr GlypherManagerGet() 
	{ 
		return pGlypherManagerField;
	}

	Private GlypherOptions::FontMapOptionsListPtr GetDefaultFontMapOptions()
	{
		MSXML2::IXMLDOMElementPtr pelement = GetConfigDoc()->selectSingleNode(L"Config/FontMapOptionsList");
		GlypherOptions::FontMapOptionsListPtr pfontMapOptionsList = new GlypherOptions::FontMapOptionsList();
		pfontMapOptionsList->Serialize(pelement, false);
		return pfontMapOptionsList;
	}

	Private MSXML2::IXMLDOMDocumentPtr pConfigDoc;
	Public MSXML2::IXMLDOMDocumentPtr GetConfigDoc()
	{
		if (pConfigDoc!=NULL)
			return pConfigDoc;
		
		//initialize msxml
		TESTHR(pConfigDoc.CreateInstance("Msxml2.DOMDocument.3.0"));
		
		//initialize xml document
		Path file = GetPath(pathConfigFile);
		if (pConfigDoc->load((LPCTSTR)file)==VARIANT_FALSE)
			ThrowComError(L"Could not read " + file.ToString());

		return pConfigDoc;
	}
};

extern CEdit* pEditControl;
extern CGlypherApp theApp;
bool IsAppRTL(); 
inline CGlypherApp* GetApp() { return &theApp; }
inline CMainFrame* GetMainWnd() { return (CMainFrame*)AfxGetMainWnd(); }
inline GlypherOptionsPtr GetAppOptions() { return GetApp()->m_pAppOptions; }
inline GlypherManagerPtr GetManager() { return GetApp()->GlypherManagerGet(); }
inline UnicodeGlyphInfosPtr GetGlyphInfos() { return GetManager()->UnicodeGlyphConverterGet()->UnicodeGlyphInfosGet(); }
inline CEdit* GetEditConrol() {return pEditControl;}
inline CString GetInputText()
{
	if (GetEditConrol()==NULL)
		return L"";

	CString text = m::Wnd::GetWindowText(*GetEditConrol());

	int startChar;
	int endChar;
	GetEditConrol()->GetSel(startChar, endChar);
	if (startChar<endChar)
		text = text.Mid(startChar, endChar-startChar);

	if ( text.IsEmpty() )
         text = m::Wnd::GetWindowText(*GetEditConrol());
	return text;
}

void SetInputText(CString text);


inline bool GetInputRtl()
{
	return GetEditConrol()!=NULL && _TOBOOL(GetEditConrol()->GetExStyle()&WS_EX_RTLREADING);
}