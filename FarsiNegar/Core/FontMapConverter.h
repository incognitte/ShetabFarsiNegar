#pragma once
#include "FontMap.h"

namespace GlypherCore{

DEFINE_COMPTR(FontMapConverter);
class FontMapConverter : public IUnknownImpl<FontMapConverter>
{
	Private FontMapsPtr pFontMapsField;
	Public FontMapsPtr FontMapsGet()
	{
		return pFontMapsField;
	}

	Private UnicodeGlyphConverterPtr pUnicodeGlyphConvertorField;

	Private FontMapConverter();
	Public explicit FontMapConverter(UnicodeGlyphConverterPtr punicodeGlyphConvertor)
	{
		pUnicodeGlyphConvertorField = punicodeGlyphConvertor;
		pFontMapsField = new FontMaps();
	}

	Public CString Convert(CString text, CString srcName, CString desName, bool rightToLeft)
	{
		HFONT fontHandle = mOld::Font_Create(L"Name=Tahoma;Size=8");
		DWORD flags = rightToLeft ? UnicodeGlyphConverter::FlagRightToLeft : 0;
		CString ret = Convert(text, srcName, desName, fontHandle, 0, flags);
		DeleteObject( fontHandle );
		return ret;
	}

	Public CString Convert(CString text, CString srcName, CString desName, HFONT fontHandle, int width, DWORD flags)
	{
		//find fontmap
		FontMapPtr psrcFontMap = FontMapsGet()->FindRefByName(Path(srcName));
		FontMapPtr pdesFontMap = FontMapsGet()->FindRefByName(Path(desName));

		//convert text to glyphs
		CString srcUnicodeGlyphs;
		if (psrcFontMap==NULL)
		{
			srcUnicodeGlyphs = pUnicodeGlyphConvertorField->StringToGlyphs(text, fontHandle, width, flags); //source is text
		}
		else
		{
			srcUnicodeGlyphs = psrcFontMap->FontGlyphsToUnicodeGlyphs(text); //source is font glyphs
		}
		
		//convert text to glyphs
		CString ret;
		if (pdesFontMap==NULL)
			ret = pUnicodeGlyphConvertorField->GlyphsToString(srcUnicodeGlyphs); //return unicode text
		else
			ret = pdesFontMap->UnicodeGlyphsToFontGlyphs(srcUnicodeGlyphs); //return requested glyphs

		return ret;
	}
};

}//namespace