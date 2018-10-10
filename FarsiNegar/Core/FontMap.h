#pragma once
#include "UnicodeGlyph.h"
#include "GlypherUtil.h"

namespace GlypherCore{

//UnicodeGlyphMap keep font glyph for each unicode glyph
//(Unicode Glyph)->(Font Glyph)
DEFINE_COMPTR(UnicodeGlyphMap);
class UnicodeGlyphMap : public IUnknownImpl<UnicodeGlyphMap>
{
	Public bool CanInverse;
	Public CString FontGlyphs;
	Public UnicodeGlyphMap(CString fontGlyphs, bool canInverse)
	{
		FontGlyphs = fontGlyphs;
		CanInverse = canInverse;
	}

	Public explicit UnicodeGlyphMap(UnicodeGlyphMap* pobj)
	{
		FontGlyphs = pobj->FontGlyphs;
		CanInverse = pobj->CanInverse;
	}

	Public UnicodeGlyphMapPtr Clone()
	{
		return new UnicodeGlyphMap(this);
	}

	Public explicit UnicodeGlyphMap(MSXML2::IXMLDOMElementPtr pelement)
	{
		CanInverse = false;
		Serialize(pelement, false);
	}

	Public void Serialize(MSXML2::IXMLDOMElementPtr pelement, bool save)
	{
		if (save)
		{
			pelement->setAttribute(L"GlyphSymbol", (LPCTSTR)FontGlyphs);
			pelement->setAttribute(L"FontGlyphCode", (LPCTSTR)UnicodeGlyphInfo::DataToCode(FontGlyphs));
			pelement->setAttribute(L"CanReverse", CanInverse);
		}
		else
		{
			_variant_t attr;
			attr = pelement->getAttribute(L"FontGlyphCode");
			FontGlyphs = UnicodeGlyphInfo::CodeToData(attr);
			attr = pelement->getAttribute(L"CanReverse");
			if (attr.vt!=VT_NULL) CanInverse = attr;
		}
	}

	Private UnicodeGlyphMap();
	Protected ~UnicodeGlyphMap()
	{
	}
};

DEFINE_COMPTR(UnicodeGlyphMaps);
class UnicodeGlyphMaps : public IUnknownImpl<UnicodeGlyphMaps>, public CAtlMap<WCHAR, UnicodeGlyphMapPtr>
{
	Public explicit UnicodeGlyphMaps()
	{
	}

	Public UnicodeGlyphMapPtr Find(WCHAR unicodeGlyph)
	{
		CPair* pair = Lookup(unicodeGlyph);
		if ( pair!=NULL )
			return pair->m_value;
		return NULL;
	}

	Public explicit UnicodeGlyphMaps(MSXML2::IXMLDOMElementPtr pelement)
	{
		Serialize(pelement, false);
	}

	Public void Serialize(MSXML2::IXMLDOMElementPtr pelement, bool save)
	{
		if (save)
		{

			POSITION pos = GetStartPosition();
			while (pos!=NULL)
			{
				CPair* ppair = GetNext(pos);
				WCHAR unicodeGlyph = ppair->m_key;
				UnicodeGlyphMapPtr pgm = ppair->m_value;
				if (unicodeGlyph>32)
				{
					MSXML2::IXMLDOMElementPtr pglyphElement = pelement->ownerDocument->createElement(L"Glyph");
					pelement->appendChild(pglyphElement);
					pglyphElement->setAttribute(L"Code", (LPCTSTR)Convert::ToString(unicodeGlyph, L"%04x"));
					pgm->Serialize(pglyphElement, save);
				}
			}
		}
		else
		{
			MSXML2::IXMLDOMNodeListPtr psubElements = pelement!=NULL ? pelement->selectNodes("Glyph") : NULL;
			for (int i=0; psubElements!=NULL && i<(int)psubElements->length; i++)
			{
				MSXML2::IXMLDOMElementPtr pglyphElement = psubElements->Getitem(i);
				_variant_t attr;
				attr = pglyphElement->getAttribute(L"Code");
				WCHAR code = (WCHAR)Convert::ToInt(attr, 0, 16);
				UnicodeGlyphMapPtr pgm = new UnicodeGlyphMap(pglyphElement);
				SetAt(code, pgm);
			}
		}
	}

	Protected ~UnicodeGlyphMaps()
	{
	}
};

//class contain all glyphs info about a font
DEFINE_COMPTR(FontMap);
class FontMap : public IUnknownImpl<FontMap>
{
	Public enum Flags
	{
		FlagUnicodeRange			= 0x0001, //one or more glyph exists in unicode range
		FlagHasEngCapGlyphs			= 0x0004,
		FlagHasEngGlyphs			= 0x0008,
		FlagInvertGlyphs			= 0x0010,
		FlagUseUnicodeClipboard			= 0x0040,
		FlagShowInViewAsDefault		= 0x0080,
	};

	// set text to clipboard using application and fontmap options; 
	// this method do not any convert;
	// @param text is fontmap glyphs
	Public bool Clipboard_Set(CString glyphs, bool ansiMode)
	{
		//start copy text to clipboard
		CStringA textA = CT2A(glyphs, 1252);
		CStringW textW = glyphs;

		//clear Ansi Text
		if ( IsInUnicodeRange() )
		{
			textA.Empty();
		}
		//clear Unicode Text
		else if ( !_TOBOOL(Flags & FontMap::FlagUseUnicodeClipboard) || ansiMode )
		{
			textW.Empty();
		}
	        
		return m::Clipboard::CopyText(textA, textW, true);
	}

	// Get glyphs from clipboard depend to fontmap settings
	Public CString Clipboard_Get(bool ansiMode)
	{
		//get text from clipboard
		CStringA textA;
		CStringW textW;
		CString ret;
		int res = m::Clipboard::PasteText(textA, textW);
		bool hasAnsi = _TOBOOL(res&1);
		bool hasUnicode = _TOBOOL(res&2);
		if (hasAnsi || hasUnicode)
		{
			if (!_TOBOOL(Flags & FontMap::FlagUseUnicodeClipboard))
				ansiMode = true;

			//force ansiMode if unicode clipboard not exist but ansi exist
			bool justAnsiExist = hasAnsi && !hasUnicode;
			if (justAnsiExist || (hasAnsi && ansiMode && !IsInUnicodeRange()))
			{
				ret = CA2W(textA, 1252);
			}
			else if ( hasUnicode ) 
			{
				ret = textW;
			}

			return ret;
		}

		return false;
	}


	Public UnicodeGlyphMapsPtr pUnicodeGlyphMapsField;
	Public UnicodeGlyphMapsPtr UnicodeGlyphMapsGet()
	{
		return pUnicodeGlyphMapsField;
	}

	Public void SetEngLetters(bool value)
	{
		for (WCHAR c=0x0041; c<=0x005A; c++)
		{
			UnicodeGlyphMapPtr pgm = new UnicodeGlyphMap(CString(c), true);
			if (value)
				UnicodeGlyphMapsGet()->SetAt(c, pgm);
			else
				UnicodeGlyphMapsGet()->RemoveKey(c);
		}
	}

	Public void SetEngCapsLetters(bool value)
	{
		for (WCHAR c=0x0061; c<=0x007A; c++)
		{
			UnicodeGlyphMapPtr pgm = new UnicodeGlyphMap(CString(c), true);
			if (value)
				UnicodeGlyphMapsGet()->SetAt(c, pgm);
			else
				UnicodeGlyphMapsGet()->RemoveKey(c);
		}
	}

	Public bool IsUnicodeView()
	{ 
		return FileNameGet().GetTitle().CompareNoCase(L"UnicodeView")==0; 
	}

	bool IsInUnicodeRange() 
	{ 
		return _TOBOOL(Flags & FlagUnicodeRange); 
	}

	//Find UnicodeGlyphMap By UnicodeGlyph Code
	UnicodeGlyphMapPtr FindUnicodeGlyphMapByUnicodeGlyph(WCHAR unicodeGlyph)
	{
		UnicodeGlyphMapPtr pret = UnicodeGlyphMapsGet()->Find(unicodeGlyph);
		if ( pret==NULL && IsUnicodeView() )
			pret = new UnicodeGlyphMap(CString(unicodeGlyph), true);
		return pret;
	}

	//Find UnicodeGlyph Code by FontGlyphCode
	WCHAR FindUnicodeGlyphByFontGlyph(WCHAR fontGlyph)
	{
		POSITION pos = UnicodeGlyphMapsGet()->GetStartPosition();
		while(pos!=NULL)
		{
			UnicodeGlyphMaps::CPair* ppair = UnicodeGlyphMapsGet()->GetNext(pos);
			WCHAR unicodeGlyph = ppair->m_key;
			UnicodeGlyphMapPtr pgm = ppair->m_value;
			if ( pgm->FontGlyphs.GetLength()==1 && pgm->FontGlyphs[0]==fontGlyph && pgm->CanInverse)
				return unicodeGlyph;
		}

		//return fontGlyph in Standard fontamp and if glyph not associated at yet
		if ( IsUnicodeView() )
			return fontGlyph;

		return 0;
	}

	Public CString FontGlyphsToUnicodeGlyphs(CString fontGlyphs)
	{
		//convert
		CString ret;
		for (int i=0; i<fontGlyphs.GetLength(); i++)
		{
			ret += FindUnicodeGlyphByFontGlyph(fontGlyphs[i]);
		}

		//canInverse each line
		if ( _TOBOOL(Flags&FlagInvertGlyphs) )
		{

			ret = InvertEachLine(ret);
		}

		return ret;
	}

	Public CString UnicodeGlyphsToFontGlyphs(CString unicodeGlyphs)
	{
		//convert
		CString ret;
		for (int i=0; i<unicodeGlyphs.GetLength(); i++)
		{
			WCHAR unicodeGlyph = unicodeGlyphs.GetAt(i);
			UnicodeGlyphMapPtr pgm = FindUnicodeGlyphMapByUnicodeGlyph(unicodeGlyph);
			if (pgm!=NULL)
			{
				CString glyphs = pgm->FontGlyphs;
				if ( _TOBOOL(Flags & FlagInvertGlyphs) )
				{
					InvertString(glyphs, 0, glyphs.GetLength()-1);
				}
				ret += glyphs;
			}
		}

		//canInverse each line
		if ( _TOBOOL(Flags&FlagInvertGlyphs) )
		{
			ret = InvertEachLine(ret);
		}

		return ret;
	}

	Public void CorrectKashida(LOGFONT* pLogfont, CString& glyphs, int nWidth, bool bLastLine)
	{
		ASSERT(pLogfont!=NULL);

		HDC hDC = GetDC(NULL);
		HFONT hFont = CreateFontIndirect(pLogfont);
		HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
		CorrectKashida(hDC, glyphs, nWidth, bLastLine);

		//clean up
		SelectObject(hDC, hOldFont);
		DeleteObject((HGDIOBJ)hFont);
		ReleaseDC(NULL, hDC);
	}

	// correct kashida text with nWidth
	Public void CorrectKashida(HDC hDC, CString& glyphs, int nWidth, bool bLastLine)
	{
		//in win9x GetTextExtended32W does not work correctly specially on Standard Fonts
		if (IsInUnicodeRange())
			return;

		StringListPtr plines = GlypherUtil::String_GetLines(glyphs);
		glyphs.Empty();
		POSITION pos = plines->GetHeadPosition();
		while (pos!=NULL)
		{
			CString line = plines->GetNext(pos);
			if (bLastLine || pos!=NULL)
				CorrectKashida_Line(hDC, line, nWidth);

			glyphs += line;
			if ( pos!=NULL )
				glyphs += L"\r\n";
		}
	}

	Public void Reload()
	{
		Load();
	}

	Private void Load()
	{
		//initialize msxml
		MSXML2::IXMLDOMDocumentPtr pdocPtr;
		TESTHR(pdocPtr.CreateInstance("Msxml2.DOMDocument.3.0"));
		
		//initialize xml document
		if (pdocPtr->load((LPCTSTR)FileNameGet())==VARIANT_FALSE)
			ThrowComError(L"Could not read " + FileNameGet().ToString());

		//get UnicodeGlyphs element
		MSXML2::IXMLDOMElementPtr punicodeGlyphsMapElement = pdocPtr->selectSingleNode(L"UnicodeGlyphMaps");
		Serialize(punicodeGlyphsMapElement, false);
		InsertInlineChars();
	}
	
	Public void Save(Path sortXstFile)
	{
		//initialize msxml
		MSXML2::IXMLDOMDocumentPtr pdocPtr;
		TESTHR(pdocPtr.CreateInstance("Msxml2.DOMDocument.3.0"));
		
		//initialize xml document
		MSXML2::IXMLDOMProcessingInstructionPtr pprocIns = pdocPtr->createProcessingInstruction(L"xml", L"version='1.0' encoding='utf-8'");
		pdocPtr->appendChild(pprocIns);

		//create FontGlyphs element
		MSXML2::IXMLDOMElementPtr pfontGlyphElement = pdocPtr->createElement(L"UnicodeGlyphMaps");
		pdocPtr->appendChild(pfontGlyphElement);

		Serialize(pfontGlyphElement, true);

		MSXML2::IXMLDOMDocumentPtr psortXST;
		TESTHR(psortXST.CreateInstance("Msxml2.DOMDocument.3.0"));
		if (psortXST->load((LPCTSTR)sortXstFile)==VARIANT_FALSE)
			ThrowComError(L"Could not load " + sortXstFile.ToString());

		MSXML2::IXMLDOMDocumentPtr pdocPtr2;
		TESTHR(pdocPtr2.CreateInstance("Msxml2.DOMDocument.3.0"));
		_bstr_t bstr = pdocPtr->transformNode(psortXST);
		if (pdocPtr2->loadXML(bstr)==VARIANT_FALSE)
			ThrowComError(L"Could not load transformerd XML!");
		pdocPtr2->save((LPCTSTR)FileNameGet());
	}

	Public void Serialize(MSXML2::IXMLDOMElementPtr pelement, bool save)
	{
		if (save)
		{
			pelement->setAttribute(L"Title", (LPCTSTR)Title);
			pelement->setAttribute(L"UnicodeRange", _TOBOOL(Flags&FlagUnicodeRange));
			pelement->setAttribute(L"IncludeEnglishUppercaseGlyphs", _TOBOOL(Flags&FlagHasEngCapGlyphs));
			pelement->setAttribute(L"IncludeEnglishLowercaseGlyphs", _TOBOOL(Flags&FlagHasEngGlyphs));
			pelement->setAttribute(L"Inverted", _TOBOOL(Flags&FlagInvertGlyphs));
			pelement->setAttribute(L"UseUnicodeClipboard", _TOBOOL(Flags&FlagUseUnicodeClipboard));
			pelement->setAttribute(L"ShowInViewAsDefault", _TOBOOL(Flags&FlagShowInViewAsDefault));
			pelement->setAttribute(L"DownloadFontUrl1", (LPCTSTR)DownloadFontUrl1);
			pelement->setAttribute(L"DownloadFontUrl2", (LPCTSTR)DownloadFontUrl2);
			pelement->setAttribute(L"DownloadFontUrl3", (LPCTSTR)DownloadFontUrl3);

			//Note
			MSXML2::IXMLDOMElementPtr pnoteElement = pelement->ownerDocument->createElement(L"Note");
			pelement->appendChild(pnoteElement);
			pnoteElement->text = (LPCTSTR)Note;
			//Font
			if (pDefaultFontField!=NULL)
			{
				MSXML2::IXMLDOMElementPtr pdefaultFontElement = pelement->ownerDocument->createElement(L"DefaultFont");
				pelement->appendChild(pdefaultFontElement);
				mOld::Font_SaveToXmlElement(pdefaultFontElement, *pDefaultFontField);
			}

			pUnicodeGlyphMapsField->Serialize(pelement, save);
		}
		else
		{
			Flags = 0;
			_variant_t attr;
			attr = pelement->getAttribute(L"Name");
			if (attr.vt!=VT_NULL) Title = attr;
			attr = pelement->getAttribute(L"Title");
			if (attr.vt!=VT_NULL) Title = attr;
			attr = pelement->getAttribute(L"UnicodeRange");
			if (attr.vt!=VT_NULL && Convert::ToBoolean(attr, false)) Flags|=FlagUnicodeRange;
			attr = pelement->getAttribute(L"IncludeEnglishUppercaseGlyphs");
			if (attr.vt!=VT_NULL && Convert::ToBoolean(attr, false)) Flags|=FlagHasEngCapGlyphs;
			attr = pelement->getAttribute(L"IncludeEnglishLowercaseGlyphs");
			if (attr.vt!=VT_NULL && Convert::ToBoolean(attr, false)) Flags|=FlagHasEngGlyphs;
			attr = pelement->getAttribute(L"Inverted");
			if (attr.vt!=VT_NULL && Convert::ToBoolean(attr, false)) Flags|=FlagInvertGlyphs;
			attr = pelement->getAttribute(L"UseUnicodeClipboard");
			if (attr.vt!=VT_NULL && Convert::ToBoolean(attr, false)) Flags|=FlagUseUnicodeClipboard;
			attr = pelement->getAttribute(L"ShowInViewAsDefault");
			if (attr.vt!=VT_NULL && Convert::ToBoolean(attr, false)) Flags|=FlagShowInViewAsDefault;
			attr = pelement->getAttribute(L"DownloadFontUrl1");
			if (attr.vt!=VT_NULL) DownloadFontUrl1 = CString(attr);
			attr = pelement->getAttribute(L"DownloadFontUrl2");
			if (attr.vt!=VT_NULL) DownloadFontUrl2 = CString(attr);
			attr = pelement->getAttribute(L"DownloadFontUrl3");
			if (attr.vt!=VT_NULL) DownloadFontUrl3 = CString(attr);
			//Note
			MSXML2::IXMLDOMElementPtr pnoteElement = pelement->selectSingleNode(L"Note");
			if (pnoteElement!=NULL)
				pnoteElement->text = (LPCTSTR)Note;
			//Font
			MSXML2::IXMLDOMElementPtr pdefaultFontElement = pelement->selectSingleNode(L"DefaultFont");
			if (pdefaultFontElement!=NULL)
				pDefaultFontField = new FontHandleClass(mOld::Font_LoadFromXmlElement(pdefaultFontElement));
			//UnicodeGlyphMaps
			pUnicodeGlyphMapsField = new UnicodeGlyphMaps(pelement);
		}
	}

	Public FontMap(Path file, CString title)
	{
		pUnicodeGlyphMapsField = new UnicodeGlyphMaps();
		pDefaultFontField = new FontHandleClass( mOld::Font_Create(L"Name=Tahoma;Size=12;") );
		Flags = FontMap::FlagShowInViewAsDefault;
		FileNameField = file;
		Title = title;
	}

	Public explicit FontMap(Path file)
	{
		pUnicodeGlyphMapsField = new UnicodeGlyphMaps();
		pDefaultFontField = new FontHandleClass( mOld::Font_Create(L"Name=Tahoma;Size=12;") );
		Flags = FontMap::FlagShowInViewAsDefault;
		FileNameField = file;
		Load();
	}

	Private FontMap();
	Protected ~FontMap(void)
	{
	}

	Private FontHandleClassPtr pDefaultFontField;
	Public FontHandleClassPtr DefaultFontGet()
	{
		return pDefaultFontField;
	}
	Public void DefaultFontSet(FontHandleClassPtr pdefaultFont)
	{
		ATLENSURE(pdefaultFont!=NULL);
		pDefaultFontField = pdefaultFont;
	}

	Public DWORD Flags;
	Public CString DownloadFontUrl1;
	Public CString DownloadFontUrl2;
	Public CString DownloadFontUrl3;
	Public CString Note;
	Public CString Title;

	Public CString NameGet()
	{
		return FileNameField.GetTitle();
	}

	Private Path FileNameField;
	Public void FileNameSet(Path value)
	{
		FileNameField = value;
	}
	Public Path FileNameGet()
	{
		return FileNameField;
	}

	Private void CorrectKashida_Line(HDC hDC, CString& glyphs, int nWidth)
	{
		//get dash size
		UnicodeGlyphMapPtr pgm = FindUnicodeGlyphMapByUnicodeGlyph(0x640);
		if (pgm==NULL)
			return;

		CSize size(0,0);
		GlypherUtil::String_GetTextSize(hDC, pgm->FontGlyphs, &size);
		int nDashWidth = size.cx;
		if (nDashWidth==0)
			return;
		
		//correct line
		GlypherUtil::String_GetTextSize(hDC, glyphs, &size);
		int nLineWidth = size.cx;

		//find how much dash must added or deleted
		int nDash = (int)floor( (double)(nWidth-nLineWidth)/(double)nDashWidth );
		
		//increase line width if possible
		if ( nDash<0 )
		{
			for (int i=0; i<-nDash; i++)
			{
				int iFind = glyphs.Find(pgm->FontGlyphs);
				if (iFind==-1)
					break;
				else
					glyphs.Delete(iFind, pgm->FontGlyphs.GetLength());
			}
		}

		//decrease line width
		if ( nDash>0 )
		{
			int findIndex = 0;
			for (int i=0; i<nDash && findIndex!=-1; i++)
			{
				findIndex = glyphs.Find(pgm->FontGlyphs, findIndex);
				if (findIndex==-1)
					findIndex = glyphs.Find(pgm->FontGlyphs, 0);
				if (findIndex!=-1)
					glyphs.Insert(findIndex, pgm->FontGlyphs);
			}
		}
	}
	
	Private static CString InvertEachLine(CString text)
	{
		CString ret = text;
		
		int iCur=0;
		while( iCur<ret.GetLength() )
		{
			//find start
			int iStart=-1;
			for (; iCur<ret.GetLength(); iCur++)
			{
				if ( ret[iCur]!=L'\n' && ret[iCur]!=L'\r' )
				{
					iStart = iCur;
					break;
				}
			}

			//find end
			int iEnd=iStart;
			for (; iCur<ret.GetLength(); iCur++)
			{
				if ( ret[iCur]==L'\n' || ret[iCur]==L'\r' )
					break;
				iEnd = iCur;
			}

			//reverse
			if (iStart!=-1)
			{
				InvertString(ret, iStart, iEnd);
			}
		}

		return ret;
	}

	Private static void InvertString(CString& text, int iStart, int iEnd)
	{
		for (int i=0; i<(iEnd-iStart+1)/2; i++)
		{
			WCHAR c1 = text.GetAt(iStart+i);
			WCHAR c2 = text.GetAt(iEnd-i);
			text.SetAt(iStart+i, c2);
			text.SetAt(iEnd-i, c1);
		}
	}

	Private void InsertInlineChars(void)
	{
		//arabic characters
		for (WCHAR c=0x660; c<=0x669; c++)
		{
			UnicodeGlyphMapPtr pgm = FindUnicodeGlyphMapByUnicodeGlyph(0x6F0+c-0x660);
			if (pgm!=NULL)
			{
				UnicodeGlyphMapPtr pgm2 = new UnicodeGlyphMap(pgm->FontGlyphs, false);
				UnicodeGlyphMapsGet()->SetAt(c, pgm2);
			}
		}

		//insert space
		UnicodeGlyphMapsGet()->SetAt(L' ', new UnicodeGlyphMap(L" ", true));
		UnicodeGlyphMapsGet()->SetAt(L'\x00a0', new UnicodeGlyphMap(L" ", false));

		//insert next line
		UnicodeGlyphMapsGet()->SetAt(L'\n', new UnicodeGlyphMap(L"\n", true));
		UnicodeGlyphMapsGet()->SetAt(L'\r', new UnicodeGlyphMap(L"\r", true));
		
		//insert digit seprator
		UnicodeGlyphMapPtr pgm = FindUnicodeGlyphMapByUnicodeGlyph(0x60C);
		if (pgm!=NULL)
			UnicodeGlyphMapsGet()->SetAt(0x66C, new UnicodeGlyphMap(pgm->FontGlyphs, false));

		//insert soft hyphen same as minues
		pgm = FindUnicodeGlyphMapByUnicodeGlyph(0x2D);
		if (pgm!=NULL)
			UnicodeGlyphMapsGet()->SetAt(0xAD, new UnicodeGlyphMap(pgm->FontGlyphs, false));
	}

	Public bool HasDownloadFontUrl(void)
	{
		return !DownloadFontUrl1.IsEmpty() || !DownloadFontUrl2.IsEmpty() || !DownloadFontUrl3.IsEmpty();
	}
};

DEFINE_COMPTR(FontMaps);
class FontMaps : public List<FontMaps, FontMapPtr>
{
	Protected ~FontMaps()
	{
	}

	Public void LoadFolder(Path folder)
	{
		PathListPtr pfiles = m::File::Find(folder, L"*.fontmap", m::File::FindFlagFile | m::File::FindFlagRecursive);
		POSITION pos = pfiles->GetHeadPosition();
		while (pos!=NULL)
		{
			Path file = pfiles->GetNext(pos);
			try
			{
				FontMapPtr pfontMap = new FontMap(file);
				AddTail(pfontMap);
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

	Public POSITION FindByName(CString name)
	{
		POSITION pos = GetHeadPosition();
		while (pos!=NULL)
		{
			POSITION curPos = pos;
			FontMapPtr pfontMap = GetNext(pos);
			if( pfontMap->NameGet().CompareNoCase(name)==0 )
				return curPos;
		}
		return NULL;
	}

	Public FontMapPtr FindRefByName(CString name)
	{
		POSITION pos = FindByName(name);
		return pos!=NULL ? GetAt(pos) : NULL;
	}

	Public POSITION FindByFilename(Path fileName)
	{
		POSITION pos = GetHeadPosition();
		while (pos!=NULL)
		{
			POSITION curPos = pos;
			FontMapPtr pfontMap = GetNext(pos);
			if( pfontMap->FileNameGet().CompareNoCase(fileName)==0 )
				return curPos;
		}
		return NULL;
	}

	Public FontMapPtr FindRefByFilename(Path fileName)
	{
		POSITION pos = FindByFilename(fileName);
		return pos!=NULL ? GetAt(pos) : NULL;
	}

};

} //namespace