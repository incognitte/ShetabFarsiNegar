#include "stdafx.h"
#include "Glypher.h"
#include "MainFrm.h"
#include "GlypherDoc.h"
#include "GlypherView.h"
#include "GlypherOptions.h"
#include "Dialog_Hint.h"
#include "CWebBrowser2.h"

//#pragma comment(linker, "/SECTION:.shared,RWS") 
//#pragma data_seg(".shared")
//#pragma data_seg()  //mad


BEGIN_MESSAGE_MAP(CGlypherApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CGlypherApp construction
CGlypherApp::CGlypherApp()
{
}

void CGlypherApp::LoadFontMaps()
{
	pGlypherManagerField->FontMapsGet()->LoadFolder(GetPath(pathFontmapFolder));
	GetAppOptions()->SortFontMaps(pGlypherManagerField->FontMapsGet());
	//add user fontmaps
	POSITION pos = GetAppOptions()->pFontMapOptionsList->GetHeadPosition();
	while (pos!=NULL)
	{
		GlypherOptions::FontMapOptionsPtr fmo = GetAppOptions()->pFontMapOptionsList->GetNext(pos);
		Path file = fmo->FilePath;
		if (!file.IsEmpty() && GetManager()->FontMapsGet()->FindByFilename(file)==NULL)
		{
			try
			{
				GetManager()->FontMapsGet()->AddTail(new FontMap(file));
			}
			catch(_com_error& e)
			{
				CString msg = L"Could not load %1.\nError=%2";
				msg.Replace(L"%1", file);
				msg.Replace(L"%2", (LPCTSTR)e.Description());
				MessageBox(0, msg, NULL, MB_ICONWARNING);
			}
		}
	}
}

CGlypherApp::~CGlypherApp()
{
}

// The one and only CGlypherApp object
CGlypherApp theApp;

// CGlypherApp initialization
BOOL CGlypherApp::InitInstance()
{
	//find previous instance
	CreateMutex(NULL, FALSE, _T("Glypher_InitInstance"));
	if ( GetLastError()==ERROR_ALREADY_EXISTS)
	{
		SendMessage(HWND_BROADCAST, g_uMessageShow, 0, 0);
		return FALSE;
	}

	//Initialize
	CoInitialize(0);
	pGlypherManagerField = new GlypherManager(GetPath(pathUnicodeGlyphFile));
	m_pAppOptions = new GlypherOptions(GetDefaultFontMapOptions());
	CFontLoader::LoadFonts( GetPath(pathAppFontFolder) );
	
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();
	AfxInitRichEdit2();
	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
		return FALSE;
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	App_SetRegistryKey( REGKEY_APP );
	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)
	GetAppOptions()->Load(GetAppRegistryKey(), _T("Settings") );
	SetThreadLocale(MAKELANGID(GetAppOptions()->m_wLangId, SORT_DEFAULT));
	SetThreadUILanguage(MAKELANGID(GetAppOptions()->m_wLangId, SORT_DEFAULT));
	UpdateAppName();

	//find show flags
	bool bShow = Param::Get(_T("/Show"), 1, m_lpCmdLine)!=0;
	if ( !bShow ) m_nCmdShow = SW_HIDE;

	//start font loader
	ReInitFontLoader();

	//LoadFontMaps
	LoadFontMaps();

	//Update IE features
	CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, TRUE);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGlypherDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CGlypherView));
	AddDocTemplate(pDocTemplate);
	
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

		// The one and only window has been initialized, so show and update it
	if ( bShow )
	{
		m_pMainWnd->ShowWindow(SW_SHOW);
		m_pMainWnd->UpdateWindow();
	}

	//show tip
	//if ( bShow && !_TOBOOL(GetAppOptions()->m_dwHints & GlypherOptions::hintTip1) )
	//{
	//  CDialog_Hint dlg(m_pMainWnd);
	//	dlg.m_strText = _L(IDS_TIP1);
	//	dlg.DoModal();
	//	if (dlg.m_bDontShow)
	//		GetAppOptions()->m_dwHints |= GlypherOptions::hintTip1;
	//}


	return TRUE;
}



// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog( CAboutDlg::IDD )
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
		//set register information
		//theApp.Lock_Check(); //ensure called
		//typedef LPCWSTR (CALLBACK *GETREGISTRATIONINFOW)();
		//GETREGISTRATIONINFOW pGetRegistrationInfoW = (GETREGISTRATIONINFOW) GetProcAddress(theApp.GetSoftLockModuleHandle(), "GetRegistrationInfoW");
		//if (pGetRegistrationInfoW!=NULL)
		//{
		//	CString strInfo(pGetRegistrationInfoW());
		//	CString strFirstName = Param::Get(_T("FirstName"), _T(""), strInfo);
		//	CString strLastName = Param::Get(_T("LastName"), _T(""), strInfo);
		//	CString strCompany = Param::Get(_T("Company"), _T(""), strInfo);
		//	SetDlgItemText(IDC_STATIC_REGNAME, strFirstName + _T(" ") + strLastName);
		//	SetDlgItemText(IDC_STATIC_REGCOMPANY, strCompany);
		//}
	}
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// App command to run the dialog
void CGlypherApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

Path CGlypherApp::GetPath(CGlypherApp::PathEnum path)
{
	Path exeFile = m::GetModuleFileName();
	Path exeFolder = exeFile.GetFolder();
	Path appFolder = exeFolder.GetFolder();
	LANGID langId = LANGIDFROMLCID(GetThreadLocale());

	switch(path){
	case pathExeFile:
		return exeFile;

	case pathAppFolder:
		return appFolder;

	case pathAppDllFile:
		return exeFolder + Path(L"FarsiNegar.dll");

	case pathConfigFile:
		return appFolder + Path(L"config.xml");

	case pathFontmapFolder:
		return appFolder + Path(L"Fontmaps");

	case pathUnicodeGlyphFile:
		return appFolder + Path(L"UnicodeGlyphs.xml");

	case pathReadmeFile:
		return PRIMARYLANGID(langId)==LANG_FARSI 
			? GetPath(pathDocFolder) + Path(L"farsi.url")
			: GetPath(pathDocFolder) + Path(L"english.url");
			break;

	case pathDocFolder:
		return GetPath(pathAppFolder) + Path(L"Documents");

	case pathAppFontFolder:
		return appFolder + Path(L"Fonts");

	case pathFileConvertorFile:
		return appFolder + Path(L"FileConvertor.exe");

	case pathSortXsdFile:
		return appFolder + Path(L"FontMapSorter.xslt");

	case pathAdverUrl:
		return Path(L"http://adver.shetab.com/farsinegar/{PoolName}.html");

	case pathAdverLocalFolder:
		return appFolder + Path(L"AdverPools");

	default:
		AtlThrow(E_INVALIDARG);
	}
}

FontMapPtr CGlypherApp::CreateNewFontmap(Path fileName)
{
	FontMapPtr pfontMap = new FontMap(fileName, fileName.GetTitle());
	pfontMap->Title = fileName.GetTitle();
	pfontMap->FileNameGet() = fileName.GetFileName();
	pfontMap->Save(GetApp()->GetPath(CGlypherApp::pathSortXsdFile));
	return pfontMap;
}

int CGlypherApp::ExitInstance()
{
	if (pGlypherManagerField!=0)
	{
		CFontLoader::UnloadFonts( GetPath(pathAppFontFolder) );
		//CoUninitialize(); //if internet explorer not closed yet, it will throw error
	}
	return CWinApp::ExitInstance();
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Add extra initialization here
	CString str = m::Wnd::GetDlgItemText(*this, IDC_STATIC_VERSION);
	CString strVersion = m::File::GetVersionAsString( m::GetModuleFileName() );
	str.Replace(_T("%1"), strVersion);
	SetDlgItemText(IDC_STATIC_VERSION, str);

	//set caption
	SetWindowText(GetApp()->m_pszAppName);


	//use LoadLibrary
	LoadLibraryA(""); //used to link LoadLibraryA for SoftLock
	return TRUE;
}

bool IsAppRTL()
{ 
	return Convert::ToInt(_L(IDS_RTL))!=0; 
}

void CGlypherApp::ReInitFontLoader(void)
{
	AfxBeginThread(ReInitFontLoaderThread, 0, THREAD_PRIORITY_BELOW_NORMAL);
}

UINT CGlypherApp::ReInitFontLoaderThread( LPVOID /*pParam*/ )
{
	CWaitCursor wc;
	theApp.m_fontLoader.AddRemoveFonts(GetAppOptions()->FontFolderField, GetAppOptions()->m_strFontLoader);
	return 0;
}


int CGlypherApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	CString prompt(lpszPrompt);
	if (IsAppRTL() && prompt.GetLength()>0 && prompt[0]>255)
		nType |= MB_RIGHT | MB_RTLREADING;
	return CWinApp::DoMessageBox(lpszPrompt, nType, nIDPrompt);
}

void CGlypherApp::UpdateAppName(void)
{
	App_SetAppName( _L(AFX_IDS_APP_TITLE) );
	if (GetMainWnd()!=NULL) 
	{
		((CMainFrame*)GetMainWnd())->SetTitle( GetApp()->m_pszAppName );
		((CMainFrame*)GetMainWnd())->SetWindowText( GetApp()->m_pszAppName );
	}
}

void SetInputText(CString text)
{
	if (GetEditConrol()==NULL)
		return;

	GetEditConrol()->SetWindowText(text);
	if (GetMainWnd()!=NULL && GetMainWnd()->GetGlypherDoc()!=NULL)
		GetMainWnd()->GetGlypherDoc()->UpdateAllViews(NULL);
}
