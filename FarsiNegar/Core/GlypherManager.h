#pragma once
#include "FontMapConverter.h"

namespace GlypherCore{

	DEFINE_COMPTR(GlypherManager);
class GlypherManager : public IUnknownImpl<GlypherManager>
{
	Public enum AlignEnum{
		AlignNone,
		AlignWrap,
		AlignKashida,
		AlignKashidaFull,
	};

	Private UnicodeGlyphConverterPtr pUnicodeGlyphConverterField;
	Public UnicodeGlyphConverterPtr UnicodeGlyphConverterGet()
	{
		return pUnicodeGlyphConverterField;
	}
	
	Private GlypherManager();
	Public explicit GlypherManager(Path unicodeGlyphsFile)
	{
		pUnicodeGlyphConverterField = new UnicodeGlyphConverter(new UnicodeGlyphInfos(unicodeGlyphsFile));
		pFontMapConverterField = new FontMapConverter(pUnicodeGlyphConverterField);
	}

	Private FontMapConverterPtr pFontMapConverterField;
	Public FontMapConverterPtr FontMapsConverterGet()
	{
		return pFontMapConverterField;
	}

	Public FontMapsPtr FontMapsGet()
	{
		return FontMapsConverterGet()->FontMapsGet();
	}

	Public CString WindowsTextToFontMap(CString text, CString fontmapName, bool rightToLeft)
	{
		return WindowsTextToFontMap(text, fontmapName, NULL, 0, AlignNone, rightToLeft);
	}

	Public CString WindowsTextToFontMap(CString text, CString fontmapName, HFONT fontHandle, int width, AlignEnum align, bool rightToLeft)
	{
		FontMapPtr pfontMap = FontMapsGet()->FindRefByName(fontmapName);
	
		//return itself for windows codepage
		if (fontmapName.CompareNoCase(L"windows")==0)
			return text;

		//return empty string if fontmap not found
		if ( pfontMap==NULL )
			return CString();

		//conver to unicode glyph
		CString ret;
		if (align!=AlignNone && fontHandle!=NULL)
		{
			//make kashida with tahoma base
			LOGFONT lf = {0};
			if (mOld::Font_GetLogfont(fontHandle, &lf))
			{
				LOGFONT glf = lf;
				glf.lfCharSet = ARABIC_CHARSET;
				CString baseFont = L"Tahoma";
				_tcscpy_s(glf.lfFaceName, LF_FACESIZE, baseFont);
				HFONT fontHandle2 = CreateFontIndirect(&glf);
				DWORD flags = UnicodeGlyphConverter::FlagBreak;
				if (rightToLeft) flags |= UnicodeGlyphConverter::FlagRightToLeft;
				if (align==AlignKashida) flags |= UnicodeGlyphConverter::FlagKashida;
				if (align==AlignKashidaFull) flags |= UnicodeGlyphConverter::FlagKashidaFull;
				CString unicodeGlyphs = UnicodeGlyphConverterGet()->StringToGlyphs(text, fontHandle2, width, flags);
				DeleteObject(fontHandle2);

				//correct kashida
				ret = pfontMap->UnicodeGlyphsToFontGlyphs(unicodeGlyphs);
				if (align==AlignKashida || align==AlignKashidaFull)
					pfontMap->CorrectKashida(&lf, ret, width, align==AlignKashidaFull);
			}
		}
		else
		{
			CString unicodeGlyphs = UnicodeGlyphConverterGet()->StringToGlyphs(text, rightToLeft);
			ret = pfontMap->UnicodeGlyphsToFontGlyphs(unicodeGlyphs);
		}

		return ret;
	}

	Public bool CanCopyForFont(CString text, CString /*fontmapName*/)
	{
		return !text.IsEmpty();
	}

	//text is unicode text
	Public void CopyForFont(CString text, CString fontmapName, HFONT fontHandle, int width, AlignEnum align, bool rightToLeft, bool ansiMode)
	{
		CString glyphs = WindowsTextToFontMap(text, fontmapName, fontHandle, width, align, rightToLeft);
		FontMapPtr pfontMap = FontMapsGet()->FindRefByName(fontmapName);
		if (pfontMap!=NULL)
			pfontMap->Clipboard_Set(glyphs, ansiMode);
		else
			m::Clipboard::CopyText(glyphs);
	}

	Public CString PasteForFont(CString fontmapName, bool rightToLeft, bool ansiMode)
	{
		FontMapPtr pfontMap = FontMapsGet()->FindRefByName(fontmapName);
		if (pfontMap==NULL)
		{
			CString ret;
			m::Clipboard::PasteText(ret);
			return ret;
		}

		CString text = pfontMap->Clipboard_Get(ansiMode);
		return pFontMapConverterField->Convert(text, fontmapName, L"", rightToLeft);
	}

	Public bool CanPasteForFont(CString fontmapName, bool ansiMode)
	{
		FontMapPtr pfontMap = FontMapsGet()->FindRefByName(fontmapName);
		if (pfontMap==NULL)
		{
			CString ret;
			m::Clipboard::PasteText(ret);
			return !ret.IsEmpty();
		}

		CString text = pfontMap->Clipboard_Get(ansiMode);
		return !text.IsEmpty();
	}
};

}//namespace