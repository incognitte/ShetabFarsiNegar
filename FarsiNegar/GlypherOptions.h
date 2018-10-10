#pragma once
#include "Core\GlypherManager.h"
using namespace GlypherCore;

DEFINE_COMPTR(GlypherOptions);
class GlypherOptions : public IUnknownImpl<GlypherOptions>
{
	Public DEFINE_COMPTR(FontMapOptions);
	Public class FontMapOptions : public IUnknownImpl<FontMapOptions>
	{
		Public Path FilePath;
		Public CString Name;
		Public bool Enabled;
		Public FontHandleClassPtr pFontField;

		Public explicit FontMapOptions(FontMapOptions* pobj)
		{
			FilePath = pobj->FilePath;
			Name = pobj->Name;
			Enabled = pobj->Enabled;
			pFontField = pobj->pFontField;
		}

		Public FontMapOptionsPtr Clone()
		{
			return new FontMapOptions(this);
		}
		
		Public FontHandleClassPtr FontGet()
		{
			return pFontField;
		}

		//this method delete old font
		Public void FontSet(FontHandleClassPtr pfont)
		{
			pFontField = pfont;
		}

		Public explicit FontMapOptions(CString name, Path filePath, bool enabled, FontHandleClassPtr pfont)
		{
			Name = name;
			FilePath = filePath;
			Enabled = enabled;
			pFontField = pfont;
		}

		Public explicit FontMapOptions(MSXML2::IXMLDOMElementPtr pelement)
		{
			Enabled = true;
			Serialize(pelement, false);
		}

		Public FontMapOptions()
		{
			Enabled = true;
		}

		Protected ~FontMapOptions()
		{
		}

		Public void Serialize(MSXML2::IXMLDOMElementPtr pelement, bool save)
		{
			if (save)
			{
				pelement->setAttribute(L"FilePath", (LPCTSTR)FilePath);
				pelement->setAttribute(L"Name", (LPCTSTR)Name);
				pelement->setAttribute(L"Enabled", Enabled);
				if (pFontField!=NULL && pFontField->HandleGet()!=NULL)
				{
					MSXML2::IXMLDOMElementPtr pfontElement = pelement->ownerDocument->createElement(L"Font");
					pelement->appendChild(pfontElement);
					mOld::Font_SaveToXmlElement(pfontElement, pFontField->HandleGet());
				}
			}
			else
			{
				_variant_t attr;
				attr = pelement->getAttribute(L"FilePath");
				if (attr.vt!=VT_NULL) FilePath = Path(CString(attr));
				attr = pelement->getAttribute(L"Name");
				if (attr.vt!=VT_NULL) Name = CString(attr);
				attr = pelement->getAttribute(L"Enabled");
				if (attr.vt!=VT_NULL) Enabled = attr;
				MSXML2::IXMLDOMElementPtr pfontElement = pelement->selectSingleNode(L"Font");
				if (pfontElement!=NULL)
					pFontField = new FontHandleClass( mOld::Font_LoadFromXmlElement(pfontElement) );
			}
		}
	};

	Public DEFINE_COMPTR(FontMapOptionsList);
	Public class FontMapOptionsList : public List<FontMapOptionsList, FontMapOptionsPtr>
	{
		Public POSITION FindByName(CString name)
		{
			POSITION pos = GetHeadPosition();
			while (pos!=NULL)
			{
				POSITION curPos = pos;
				FontMapOptionsPtr pfmo = GetNext(pos);
				if( pfmo->Name.CompareNoCase(name)==0 )
					return curPos;
			}
			return NULL;
		}

		Public FontMapOptionsPtr FindRefByName(CString name)
		{
			POSITION pos = FindByName(name);
			return pos!=NULL ? GetAt(pos) : NULL;
		}

		Public FontMapOptionsList()
		{
		}

		Public void Serialize(MSXML2::IXMLDOMElementPtr pelement, bool save)
		{
			if (save)
			{
				POSITION pos = GetHeadPosition();
				while (pos!=NULL)
				{
					FontMapOptionsPtr pobj = GetNext(pos);
					MSXML2::IXMLDOMElementPtr pfontMapOptionsElement = pelement->ownerDocument->createElement(L"FontMapOptions");
					pelement->appendChild(pfontMapOptionsElement);
					pobj->Serialize(pfontMapOptionsElement, save);
				}
			}
			else
			{
				RemoveAll();
				MSXML2::IXMLDOMNodeListPtr psubElements = pelement->selectNodes(L"FontMapOptions");
				for (int i=0; psubElements!=NULL && i<(int)psubElements->length; i++)
				{
					MSXML2::IXMLDOMElementPtr pfontMapOptionsElement = psubElements->Getitem(i);
					AddTail(new FontMapOptions(pfontMapOptionsElement));
				}
			}
		}
		
		Protected ~FontMapOptionsList()
		{
		}

		Public void FromXml(CString xml)
		{
			if (xml.IsEmpty())
				return;

			//initialize msxml
			MSXML2::IXMLDOMDocumentPtr pdocPtr;
			TESTHR(pdocPtr.CreateInstance("Msxml2.DOMDocument.3.0"));
			if (pdocPtr->loadXML((LPCTSTR)xml)!=VARIANT_FALSE)
			{
				MSXML2::IXMLDOMElementPtr pelement = pdocPtr->selectSingleNode(L"FontMapOptionsList");
				if (pelement!=NULL)
					Serialize(pelement, false);
			}
		}

		Public CString ToXml()
		{
			//initialize msxml
			MSXML2::IXMLDOMDocumentPtr pdocPtr;
			TESTHR(pdocPtr.CreateInstance("Msxml2.DOMDocument.3.0"));
			
			//initialize xml document
			MSXML2::IXMLDOMProcessingInstructionPtr pprocIns = pdocPtr->createProcessingInstruction(L"xml", L"version='1.0' encoding='utf-8'");
			pdocPtr->appendChild(pprocIns);

			//create FontGlyphs element
			MSXML2::IXMLDOMElementPtr pelement = pdocPtr->createElement(L"FontMapOptionsList");
			pdocPtr->appendChild(pelement);

			Serialize(pelement, true);

			return (LPCTSTR)pelement->xml;
		}
	};

public:
	enum FlagsEnum{
		flagNoPasteKashida		= 0x0001,
		flagNoConvertKashida	= 0x0002,
		flagNoPasteEnter		= 0x0004,
		flagNoConvertEnter		= 0x0008,
		flagWrap				= 0x0010,
		flagShowRuler			= 0x0020,
		flagClearBeforePaste	= 0x0040,
		flagWrapEditor			= 0x0080,
		flagPromptOnExit		= 0x0100,
		flagNoUnicodeCopy		= 0x0400,
		flagShowSysTray			= 0x0800,
	};

	enum Hints{
		hintDemoDialog		= 0x0001,
		hintAnsiMode		= 0x0002,
		hintNormalCopy		= 0x0004,
		hintTip1			= 0x0008,
	};

	Private GlypherOptions();
	Public explicit GlypherOptions(FontMapOptionsListPtr pfontMapOptionsListDefault)
	{
		pFontMapOptionsListDefault = pfontMapOptionsListDefault;
		SetDefault();
	}

	Public GlypherOptionsPtr Clone()
	{
		return new GlypherOptions(this);
	}

	Public explicit GlypherOptions(GlypherOptions* pobj)
	{
		AlignMode = pobj->AlignMode;
		AlignSize = pobj->AlignSize;
		EditorHeight = pobj->EditorHeight;
		Flags = pobj->Flags;
		FontFolderField = pobj->FontFolderField;
		LocalAdverIndex = pobj->LocalAdverIndex;
		m_bEnableFastConvertor = pobj->m_bEnableFastConvertor;
		m_dwHints = pobj->m_dwHints;
		m_strEditorFont = pobj->m_strEditorFont;
		m_strFontLoader = pobj->m_strFontLoader;
		m_wLangId = pobj->m_wLangId;
		if (pobj->pFontMapOptionsList!=NULL) pFontMapOptionsList = pobj->pFontMapOptionsList->Clone();
		if (pobj->pFontMapOptionsListDefault!=NULL) pFontMapOptionsListDefault = pobj->pFontMapOptionsListDefault->Clone();
		QuickFontMapName = pobj->QuickFontMapName;
	}

	Protected ~GlypherOptions(void)
	{
	}
	
	Public FontMapOptionsListPtr pFontMapOptionsListDefault;
	Public FontMapOptionsListPtr pFontMapOptionsList;
	Public CString m_strFontLoader;
	Public Path FontFolderField;
	Public GlypherManager::AlignEnum AlignMode; //one of ViewMode
	Public bool m_bEnableFastConvertor;
	Public CString QuickFontMapName;
	Public DWORD Flags;
	Public DWORD m_dwHints;
	Public CString m_strEditorFont;
	Public int AlignSize;
	Public int EditorHeight;
	Public void SetDefault();
	Public void Save(HKEY hKey, CString strSubkey);
	Public void Load(HKEY hKey, CString strSubkey);
	Public bool IsFontMapEnabled(FontMapPtr pfontMap);
	Public LANGID m_wLangId;

	Private LANGID GetLangFromSoftLock();
	Private CString LocalAdverIndex;
	Public void SetLocalAdverIndex(CString pool, int index)
	{
		Param::Set(pool, index, LocalAdverIndex);
	}

	Public int GetLocalAdverIndex(CString pool)
	{
		return Param::Get(pool, 0, LocalAdverIndex);
	}

	Public void SetFontForFontMap(FontMapPtr pfontMap, FontHandleClassPtr pfont)
	{
		FontMapOptionsPtr pfind = pFontMapOptionsList->FindRefByName(pfontMap->NameGet());
		if (pfind==NULL)
			pFontMapOptionsList->AddTail(new FontMapOptions(pfontMap->NameGet(), pfontMap->FileNameGet(), true, pfont));
		else
			pfind->FontSet(pfont);
	}

	Public FontHandleClassPtr GetFontForFontMap(FontMapPtr pfontMap)
	{
		FontMapOptionsPtr pfind = pFontMapOptionsList->FindRefByName(pfontMap->NameGet());
		if (pfind==NULL || pfind->FontGet()==NULL)
			return pfontMap->DefaultFontGet();
		return pfind->FontGet();
	}

	Public void SortFontMaps(FontMapsPtr psrcFontmaps)
	{
		FontMapsPtr pfontMaps = new FontMaps();

		//find in user list
		POSITION pos = pFontMapOptionsList->GetHeadPosition();
		while (pos!=NULL)
		{
			FontMapOptionsPtr pfontMapOptions = pFontMapOptionsList->GetNext(pos);
			POSITION findPos = psrcFontmaps->FindByName(pfontMapOptions->Name);
			if (findPos!=NULL)
			{
				pfontMaps->AddTail(psrcFontmaps->GetAt(findPos));
				psrcFontmaps->RemoveAt(findPos);
			}
		}

		//add rest of fontmaps
		pfontMaps->AddTailList(psrcFontmaps);

		//repalce source pointer
		psrcFontmaps->RemoveAll();
		psrcFontmaps->AddTailList(pfontMaps);
	}
};
