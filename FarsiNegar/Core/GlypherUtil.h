#pragma once

DEFINE_COMPTR(FontHandleClass);
class FontHandleClass : public IUnknownImpl<FontHandleClass>
{
	Private HFONT FontHandleField;
	Public explicit FontHandleClass(HFONT fontHandle)
	{
		ATLENSURE(fontHandle!=NULL);
		FontHandleField = fontHandle;
	}

	Public HFONT HandleGet()
	{
		return FontHandleField;
	}

	Public operator HFONT()
	{
		return HandleGet();
	}
	
	Private FontHandleClass();
	Protected ~FontHandleClass()
	{
		if (FontHandleField!=NULL)
			DeleteObject(FontHandleField);
	}
};

class mOld
{
public:
	// Retrieves the dimensions of images in an image list. All images in an image list have the same dimensions. 
	// @see ImageList_GetIconSize in SDK
	static CSize ImageList_GetIconSize(HIMAGELIST imageListHandle)
	{
		int cx,cy;
		ATLENSURE( ::ImageList_GetIconSize(imageListHandle, &cx, &cy) );
		return CSize(cx, cy);
	}

	static LONG Font_HeightToPointSize(int logheight, HDC hdc=NULL)
	{
		bool getDC = hdc == NULL;
		if (getDC) hdc = GetDC(NULL);
		int pointsize = MulDiv(logheight, 72, GetDeviceCaps(hdc, LOGPIXELSY));
		if (getDC ) ReleaseDC(NULL, hdc);

		return abs(pointsize);
	}

	static LONG Font_PointSizeToHeight(int pointSize, HDC hdc=NULL)
	{
		pointSize *= 10;

		//{calculate width by point size (MFC)
		bool getDC = hdc == NULL;
		if (getDC)
			hdc = GetDC(NULL);

		// convert nPointSize to logical units based on pDC
		POINT pt = { 0, 0 };
		pt.y = ::MulDiv(::GetDeviceCaps(hdc, LOGPIXELSY), pointSize, 720);   // 72 points/inch, 10 decipoints/point
		TESTLASTERR( DPtoLP(hdc, &pt, 1) ); 
		POINT ptOrg = { 0, 0 };
		TESTLASTERR( DPtoLP(hdc, &ptOrg, 1) );
		int lfHeight = -abs(pt.y - ptOrg.y);
		if (getDC ) ReleaseDC(NULL, hdc);
		//}calculate width by point size (MFC)
		return lfHeight;
	}

	static HFONT Font_LoadFromXmlElement(MSXML2::IXMLDOMElementPtr element)
	{
		_variant_t attr;
		CString str;

		LOGFONT lf = {0};
		attr = element->getAttribute(L"Name");
		if (attr.vt!=VT_NULL) StrCpyN(lf.lfFaceName, CString(attr), min(LF_FACESIZE, CString(attr).GetLength()+1));
		attr = element->getAttribute(L"Size");
		if (attr.vt!=VT_NULL) lf.lfHeight = mOld::Font_PointSizeToHeight(attr);
		attr = element->getAttribute(L"Bold");
		if (attr.vt!=VT_NULL && Convert::ToInt(attr)!=0) lf.lfWeight = 700;
		attr = element->getAttribute(L"Italic");
		if (attr.vt!=VT_NULL && Convert::ToInt(attr)!=0) lf.lfItalic = TRUE;
		attr = element->getAttribute(L"Underline");
		if (attr.vt!=VT_NULL && Convert::ToInt(attr)!=0) lf.lfUnderline = TRUE;
		attr = element->getAttribute(L"StrikeOut");
		if (attr.vt!=VT_NULL && Convert::ToInt(attr)!=0) lf.lfStrikeOut = TRUE;
		attr = element->getAttribute(L"CharSet");
		if (attr.vt!=VT_NULL && Convert::ToInt(attr)!=0) lf.lfCharSet = attr;
		HFONT fontHandle = CreateFontIndirect(&lf);
		return fontHandle;
	}

	static void Font_SaveToXmlElement(MSXML2::IXMLDOMElementPtr element, HFONT fontHandle)
	{
		LOGFONT lf = {0};
		TESTLASTERR( ::GetObject(fontHandle, sizeof(LOGFONT), &lf) );

		_variant_t attr;
		element->setAttribute(L"Name", lf.lfFaceName);
		element->setAttribute(L"Size", mOld::Font_HeightToPointSize(lf.lfHeight));
		element->setAttribute(L"Bold", lf.lfWeight>=700 ? L"1" : L"0");
		element->setAttribute(L"Italic", lf.lfItalic);
		element->setAttribute(L"Underline", lf.lfUnderline);
		element->setAttribute(L"StrikeOut", lf.lfStrikeOut);
		element->setAttribute(L"CharSet", lf.lfCharSet);
	}

	Public static HFONT Font_Create(CString fontFormat, HDC hdc=NULL)
	{
		//convert string parameter to LOGFONT
		LOGFONT lf = m::LogFont_FromString(fontFormat, hdc);
		return CreateFontIndirect(&lf);
	}

	static bool Font_GetLogfont(HFONT fontHandle, LOGFONT* plogFont)
	{
		ATLASSERT (plogFont!=NULL);
		return ( GetObject(fontHandle, sizeof LOGFONT, plogFont) ) !=0;
	}

};

class GlypherUtil
{
	Public static void String_RemoveEnter(CString& text)
	{
		text.Replace(L"\r\n", L"\n");
		text.Replace(L'\r', L'\n');
		text.Replace(L" \n", L" \x1");
		text.Replace(L"\n ", L"\x1 ");
		text.Replace(L'\n', L' ');
		text.Remove(0x1);
	}

	Public static StringListPtr String_GetLines(CString text)
	{
		StringListPtr pret = new StringList();
		text.Replace(L"\r\n", L"\n");
		text.Replace(L'\r', 1);
		text.Replace(L'\n', 1);

		int iStart=0;
		bool bFound = true;
		while(bFound)
		{
			int iEnd = text.Find((WCHAR)1, iStart);
			bFound = iEnd!=-1;
			if (iEnd==-1) iEnd = text.GetLength();
			CString line = text.Mid(iStart, iEnd-iStart);
			pret->AddTail(line);
			iStart = iEnd+1;
		}
		return pret;
	}

	Public static bool String_GetTextSize(HDC hDC, CString text, LPSIZE pSize)
	{
		int cbOutGlyphs = text.GetLength();
		WORD* pOutGlyphs = new WORD[cbOutGlyphs];
		memset(pOutGlyphs, 0, cbOutGlyphs*sizeof WORD);
		SCRIPT_CACHE sc=NULL;
		ScriptGetCMap(hDC, &sc, (LPCWSTR)text, text.GetLength(), 0*SGCM_RTL, pOutGlyphs);
		bool res = GetTextExtentPointI(hDC, pOutGlyphs, cbOutGlyphs, pSize)!=FALSE;
		delete pOutGlyphs;
		return res;
	}
};
