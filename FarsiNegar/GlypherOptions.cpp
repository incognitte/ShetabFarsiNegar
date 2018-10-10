#include "StdAfx.h"
#include "glypher.h"
#include "glypheroptions.h"


LANGID GlypherOptions::GetLangFromSoftLock()
{
	try
	{
		CString activationString = m::Reg::GetString(HKEY_CURRENT_USER, L"Software\\Shetab\\SoftLock\\Activation Codes\\521", L"ActivationString");
		MSXML2::IXMLDOMDocumentPtr xmlDoc;
		TESTHR(xmlDoc.CreateInstance("Msxml2.DOMDocument.3.0"));
		if (xmlDoc->loadXML((LPCTSTR)activationString)!=VARIANT_FALSE)
		{
			_variant_t attr = xmlDoc->documentElement->getAttribute(L"LanguageId");
			return (LANGID)Convert::ToInt(attr, 0);
		}
	}
	catch(...)
	{
	}
	return  0;
}

void GlypherOptions::SetDefault()
{
	Flags = flagNoPasteKashida | flagWrap | flagShowRuler | 
		0*flagPromptOnExit | flagWrapEditor;
	AlignMode = GlypherManager::AlignNone;
	m_strEditorFont = L"Name=Tahoma;Size=8;Script=Arabic;";
	FontFolderField = m::Shell_GetSpecialFolderPath(0, CSIDL_PERSONAL , TRUE) + Path(L"Fonts");
	AlignSize = 400;
	EditorHeight = 200;
	m_dwHints = 0;
	m_bEnableFastConvertor = true;
	pFontMapOptionsList = new FontMapOptionsList();
	LocalAdverIndex.Empty();
	pFontMapOptionsList = pFontMapOptionsListDefault;

	m_wLangId = GetLangFromSoftLock();
	if (m_wLangId==0)
		m_wLangId = GetUserDefaultLangID();
}

void GlypherOptions::Save(HKEY hKey, CString strSubkey)
{
	m::Reg::Write(hKey, strSubkey, L"Flags", Flags);
	m::Reg::Write(hKey, strSubkey, L"Hints", m_dwHints);
	m::Reg::Write(hKey, strSubkey, L"ViewMode", AlignMode);
	m::Reg::Write(hKey, strSubkey, L"WrapSize", AlignSize);
	m::Reg::Write(hKey, strSubkey, L"EditorHeight", EditorHeight);
	m::Reg::Write(hKey, strSubkey, L"EnableFastConvertor", m_bEnableFastConvertor);
	m::Reg::Write(hKey, strSubkey, L"LanguageId", m_wLangId);
	m::Reg::Write(hKey, strSubkey, L"LocalAdverIndex", LocalAdverIndex);
	m::Reg::Write(hKey, strSubkey, L"QuickFontMapName", QuickFontMapName);
	m::Reg::Write(hKey, strSubkey, L"EditorFont", m_strEditorFont);
	m::Reg::Write(hKey, strSubkey, L"FontFolder", FontFolderField);
	m::Reg::Write(hKey, strSubkey, L"FontLoader", m_strFontLoader);
	m::Reg::Write(hKey, strSubkey, L"FontMapOptionsListXml", pFontMapOptionsList->ToXml());
}

void GlypherOptions::Load(HKEY hKey, CString strSubkey)
{
	SetDefault();
	Flags = m::Reg::Get(hKey, strSubkey, L"Flags", Flags);
	m_dwHints = m::Reg::Get(hKey, strSubkey, L"Hints", m_dwHints);
	m_bEnableFastConvertor = m::Reg::Get(hKey, strSubkey, L"EnableFastConvertor", m_bEnableFastConvertor)!=0;
	QuickFontMapName = m::Reg::Get(hKey, strSubkey, L"QuickFontMapName", QuickFontMapName);
	m_wLangId = (LANGID)m::Reg::Get(hKey, strSubkey, L"LanguageId", m_wLangId);
	LocalAdverIndex = m::Reg::Get(hKey, strSubkey, L"LocalAdverIndex", LocalAdverIndex);
	AlignMode = (GlypherManager::AlignEnum)m::Reg::Get(hKey, strSubkey, L"ViewMode", AlignMode);
	m_strEditorFont = m::Reg::Get(hKey, strSubkey, L"EditorFont", m_strEditorFont);
	FontFolderField = Path(m::Reg::Get(hKey, strSubkey, L"FontsFolder", FontFolderField));
	m_strFontLoader = m::Reg::Get(hKey, strSubkey, L"FontLoader", m_strFontLoader);
	AlignSize = m::Reg::Get(hKey, strSubkey, L"WrapSize", AlignSize);
	EditorHeight = m::Reg::Get(hKey, strSubkey, L"EditorHeight", EditorHeight);
	pFontMapOptionsList->FromXml(m::Reg::Get(hKey, strSubkey, L"FontMapOptionsListXml", L""));

	//ensure My Fonts folder exists
	m::File::CreateDirectory(FontFolderField);
}


bool GlypherOptions::IsFontMapEnabled(FontMapPtr pfontMap)
{
	FontMapOptionsPtr pfmo = pFontMapOptionsList->FindRefByName(pfontMap->NameGet());
	return pfmo==NULL || pfmo->Enabled;
}
