#pragma once
#include "Base.h"
namespace Mad
{
	//Array class
	template<class T, typename E>
	class Array : public IUnknownImpl<T>, public CAtlArray<E>
	{
		Public virtual CComPtr<T> Clone()
		{
			CComPtr<T> obj = new T();
			obj->SetCount(GetCount());
			for (int i=0; i<(int)GetCount(); i++)
			{
				obj->SetAt(i, GetAt(i));
			}
			return obj;
		}

		Public virtual bool Equals(CComPtr<T> obj)
		{
			if (obj==NULL || GetCount()!=obj->GetCount())
				return false;

			//check each element
			for (int i=0; i<(int)obj->GetCount(); i++)
			{
				if ( GetAt(i)!=obj->GetAt(i) )
					return false;
			}

			return true;
		}


		Protected virtual ~Array() {}
	};

	//ArrayEC class, E has Equal and Clone
	template<class T, typename E>
	class ArrayEC : public Array<T, E>
	{
		Public virtual CComPtr<T> Clone()
		{
			CComPtr<T> obj = new T();
			obj->SetCount(GetCount());
			for (int i=0; i<(int)GetCount(); i++)
			{
				obj->SetAt(i, GetAt(i)->Clone());
			}
			return obj;
		}

		Public virtual bool Equals(CComPtr<T> obj)
		{
			if (obj==NULL || GetCount()!=obj->GetCount())
				return false;

			//check each element
			for (int i=0; i<(int)obj->GetCount(); i++)
			{
				if ( !EQUALS(obj->GetAt(i), GetAt(i)) )
					return false;
			}

			return true;
		}

		Protected virtual ~ArrayEC() {}
	};
	
	//List class
	template<class T, typename E>
	class List : public IUnknownImpl<T>, public CAtlList<E>
	{
		Public template<typename TARAAY> void AddTailArray(CComPtr<TARAAY> ar)
		{
			for (int i=0; i<(int)ar->GetCount(); i++)
			{
				AddTail(ar->GetAt(i));
			}
		}
		
		Public template<typename TARAAY> CComPtr<TARAAY> ToArray()
		{
			CComPtr<TARAAY> ar = new TARAAY();
			POSITION pos = GetHeadPosition();
			while (pos!=NULL)
				ar->Add(GetNext(pos));
			return ar;
		}

		Public virtual CComPtr<T> Clone()
		{
			CComPtr<T> obj = new T();
			POSITION pos = GetHeadPosition();
			while (pos!=NULL)
			{
				obj->AddTail(GetNext(pos));
			}
			return obj;
		}

		Public virtual bool Equals(CComPtr<T> obj)
		{
			if (obj==NULL)
				return false;

			//check each element
			POSITION pos = GetHeadPosition();
			POSITION posObj = obj->GetHeadPosition();
			while (pos!=NULL && posObj!=NULL)
			{
				if ( GetNext(pos)!=obj->GetNext(posObj) ) 
					return false;
			}

			return pos==posObj; //both must be NULL
		}
		
		__if_exists(MSXML2::IXMLDOMElementPtr){
		Public template<typename ET> void Serialize(MSXML2::IXMLDOMElementPtr element, bool save, CString elementTagName)
		{
			if (save)
			{
				POSITION pos = GetHeadPosition();
				while (pos!=NULL)
				{
					MSXML2::IXMLDOMElementPtr subElement = element->ownerDocument->createElement((LPCTSTR)elementTagName);
					element->appendChild(subElement);
					GetNext(pos)->Serialize(subElement, save);
				}
			}
			else
			{
				MSXML2::IXMLDOMNodeListPtr subElements = element->selectNodes((LPCTSTR)elementTagName);
				for (int i=0; subElements!=NULL && i<(int)subElements->length; i++)
				{
					MSXML2::IXMLDOMElementPtr objElement = subElements->Getitem(i);
					E object = new ET();
					object->Serialize(objElement, save);
					AddTail(object);
				}
			}
		}}

		Protected virtual ~List()
		{
		}
	};

	//ListEC class, E has Equal and Clone
	template<class T, typename E>
	class ListEC : public List<T, E>
	{
		Public virtual CComPtr<T> Clone()
		{
			CComPtr<T> obj = new T();
			POSITION pos = GetHeadPosition();
			while (pos!=NULL)
			{
				obj->AddTail(GetNext(pos)->Clone());
			}
			return obj;
		}

		Public virtual bool Equals(CComPtr<T> obj)
		{
			if (obj==NULL)
				return false;

			//check each element
			POSITION pos = GetHeadPosition();
			POSITION posObj = obj->GetHeadPosition();
			while (pos!=NULL && posObj!=NULL)
			{
				if ( !EQUALS(GetAt(pos), obj->GetAt(posObj)) ) 
					return false; //dont use GetNext in EQUALS Macro
				GetNext(pos); 
				obj->GetNext(posObj);
			}

			return pos==posObj; //both must be NULL
		}

		Protected virtual ~ListEC()
		{
		}
	};

	
	//StringArray class
	DEFINE_COMPTR(StringArray);
	class StringArray : public Array<StringArray, CString>
	{
		Private enum SortFlags{
			SortDesc	= 0x0001,
			SortNoCase	= 0x0002,
		};

		Private static int __cdecl CompareProc(void* context, const void* arg1, const void* arg2)
		{
			DWORD flags = *((DWORD*)context);
			CString* pstring1 = (CString*)arg1;
			CString* pstring2 = (CString*)arg2;
			if (Convert::ToBoolean(flags&SortDesc))
			{
				pstring1 = (CString*)arg2;
				pstring2 = (CString*)arg1;
			}

			if (Convert::ToBoolean(flags&SortNoCase))
				return pstring1->CompareNoCase(*pstring2);
			else 
				return pstring1->Compare(*pstring2);
		}

		//search in sorted array
		Public int Search(CString value, bool ignoreCase=false)
		{
			DWORD flags = 0;
			if (ignoreCase) flags |= SortNoCase;
			void* pfound = bsearch_s(&value, GetData(), GetCount(), sizeof CString, CompareProc, &flags);
			if (pfound==NULL)
				return -1;
			int index = ((int)(INT_PTR)pfound-(int)(INT_PTR)GetData())/sizeof CString;
			return index;
		}

		Public void Sort(bool ignoreCase=false, bool ascendant=true)
		{
			DWORD flags = 0;
			if (ignoreCase) flags |= SortNoCase;
			if (!ascendant) flags |= SortDesc;
			qsort_s(GetData(), GetCount(), sizeof CString, CompareProc, &flags);
		}

		Protected virtual ~StringArray()
		{
		}
	};

	
	//StringList class
	DEFINE_COMPTR(StringList);
	class StringList : public List<StringList, CString>
	{
		Public CString Join(CString separator)
		{
			CString ret;
			POSITION pos = GetHeadPosition();
			while (pos!=NULL)
			{
				if (!ret.IsEmpty()) ret += L";";
				ret += GetNext(pos);
			}
			return ret;
		}

		Public POSITION FindNoCase(CString str, POSITION posStartAfter = NULL)
		{
			POSITION pos = posStartAfter!=NULL ? posStartAfter : GetHeadPosition();
			while (pos!=NULL)
			{
				POSITION curPos = pos;
				if (GetNext(pos).CompareNoCase(str)==0)
					return curPos;
			}
			return NULL;
		}

		Protected virtual ~StringList() { }
	};


	
	//IUnknownList class
	DEFINE_COMPTR(IUnknownList);
	class IUnknownList : public List<IUnknownList, IUnknownPtr>
	{
		Public IUnknownList() {}
		Protected virtual ~IUnknownList() {}
	};

	
	//ByteArray class
	DEFINE_COMPTR(ByteArray);
	class ByteArray : public Array<ByteArray, BYTE>
	{
	public:
		ByteArray()
		{
		}

		size_t Append(ByteArrayPtr aSrc)
		{
			return __super::Append(*aSrc);
		}

		void AppendBuffer(LPBYTE pbuffer, size_t count)
		{
			size_t oldCount = GetCount();
			SetCount(oldCount+count);
			LPBYTE last = GetData() + oldCount;
			CopyMemory(last, pbuffer, count);
		}

		//allocate buffer and initilize to zero
		//@param count number of byte in array
		explicit ByteArray(int count)
		{
			ATLENSURE( SetCount(count) );
			memset(GetData(), 0, GetCount());
		}

		ByteArray(const void* pbuffer, int count)
		{
			ATLENSURE( SetCount(count) );
			CopyMemory(GetData(), pbuffer, count);
		}

		Public static void Reverse(ByteArrayPtr obj)
		{
			int count = (int)obj->GetCount();
			ByteArrayPtr pnewArray = new ByteArray(count);
			for (int i=0; i<count; i++)
			{
				pnewArray->SetAt(count-i-1, obj->GetAt(i));
			}

			//copy to array
			for (int i=0; i<count; i++)
				obj->SetAt(i, pnewArray->GetAt(i));
		}


		CString ToHexString()
		{
			CStringA outStringA;
			int requiredLength = AtlHexEncodeGetRequiredLength((int)GetCount());
			ATLENSURE( AtlHexEncode(GetData(), (int)GetCount(), outStringA.GetBuffer(requiredLength), &requiredLength) );
			outStringA.ReleaseBuffer(requiredLength);
			return CString(outStringA);
		}

		void FromHexString(CString hexString)
		{
			RemoveAll();
			int requiredLength = AtlHexDecodeGetRequiredLength(hexString.GetLength());
			ATLENSURE( SetCount(requiredLength) );
			ATLENSURE( AtlHexDecode(CStringA(hexString), CStringA(hexString).GetLength(), GetData(), &requiredLength) );
			ATLENSURE( SetCount(requiredLength) );

		}

		//@see Base64EncodeGetRequiredLength for flags
		CString ToBase64String(DWORD flags=ATL_BASE64_FLAG_NONE)
		{
			CStringA outStringA;
			int requiredLength = Base64EncodeGetRequiredLength((int)GetCount(), flags);
			ATLENSURE( Base64Encode(GetData(), (int)GetCount(), outStringA.GetBuffer(requiredLength), &requiredLength, flags) );
			outStringA.ReleaseBuffer(requiredLength);
			return CString(outStringA);
		}

		void FromBase64String(CString base64String)
		{
			RemoveAll();
			int requiredLength = Base64DecodeGetRequiredLength(base64String.GetLength());
			ATLENSURE( SetCount(requiredLength) );
			ATLENSURE( Base64Decode(CStringA(base64String), CStringA(base64String).GetLength(), GetData(), &requiredLength) );
			ATLENSURE( SetCount(requiredLength) );
		}

	protected:
		virtual ~ByteArray()
		{
		}
	};


	
	//Time class
	DEFINE_COMPTR(Time);
	class Time : public IUnknownImpl<Time>, public CTime
	{
	public:
		virtual bool Equals(TimePtr obj){ 	return obj!=NULL && *obj==*this; }
		virtual TimePtr Clone(){ return new Time(GetTime()); }
		Time(void) : CTime(){}
		Time(__time64_t time) : CTime(time){}

	protected:
		virtual ~Time(void){};
	};

	//RegKey class
	DEFINE_COMPTR(RegKey);
	class RegKey : public IUnknownImpl<RegKey>, public CRegKey
	{
	public:
		RegKey(HKEY keyHandle=NULL)
			: CRegKey(keyHandle)
		{
		}

	protected:
		virtual ~RegKey()
		{
		}
	};

	
	//class Image
	DEFINE_COMPTR(Image);
	class Image : public IUnknownImpl<ImagePtr>, public CImage
	{
	private:
		bool UseGdiPlusForDraw;

	public:
		//create new mirrored image from current image
		virtual ImagePtr GetMirrored()
		{
			//create new bitmap
			ImagePtr pimage = Clone();
			HDC dc = pimage->GetDC();
			int  oldLayout = SetLayout(dc, LAYOUT_RTL);
			TESTLASTERR( oldLayout!=GDI_ERROR );
			TESTLASTERR( BitBlt(dc, 0, 0) );
			SetLayout(dc, oldLayout ); //CImage::GetDC always cash last state
			pimage->ReleaseDC();
			return pimage;
		}

		virtual ImagePtr Clone()
		{
			HBITMAP bitmapHandle = (HBITMAP)CopyImage(*this, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			ImagePtr pimage = new Image(bitmapHandle);
			pimage->UseGdiPlusForDraw = UseGdiPlusForDraw;
			return pimage;
		}

		explicit Image(int width, int height, int bpp, DWORD flags=0)
		{
			TESTLASTERR( Create(width, height, bpp, flags) );
		}

		explicit Image(HBITMAP bitmapHandle=NULL)
			: UseGdiPlusForDraw(false) //gdiplus has problem on mirreored window
		{
			if (bitmapHandle!=NULL)
				Attach(bitmapHandle);
		}

		HRESULT Load(_Inout_ IStream* pStream) throw()
		{
			return __super::Load(pStream);
		}

		HRESULT Load(LPCTSTR pszFileName) throw()
		{
			CString file = pszFileName;
			file.Replace(L"file:///", L"");
			file.Replace(L"/", L"\\");
			return __super::Load(file);
		}

		static ImagePtr FromFile(CString file)
		{
			ImagePtr image = new Image();
			TESTHR( image->Load(file) );
			return image;
		}

		bool GetUseGdiPlusForDraw() const
		{
			return UseGdiPlusForDraw;
		}

		void SetUseGdiPlusForDraw(bool value)
		{
			UseGdiPlusForDraw = value;
		}

		BOOL Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, bool useOrgDraw=false ) throw()
		{
			if (GetBPP()==32 && !useOrgDraw)
			{ 
				ImagePtr pimage = this;
				DWORD desLayout = GetLayout(hDestDC);
				if ( _TOBOOL(desLayout&LAYOUT_RTL) && !_TOBOOL(desLayout&LAYOUT_BITMAPORIENTATIONPRESERVED))
					pimage = GetMirrored();

				if (GetUseGdiPlusForDraw() && IsDIBSection())
				{
					Gdiplus::Graphics graphics(hDestDC);
					graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
					Gdiplus::Bitmap bitmap( pimage->GetWidth(), pimage->GetHeight(), pimage->GetPitch(), PixelFormat32bppARGB, (BYTE*)pimage->GetBits() );
					return graphics.DrawImage(&bitmap, Gdiplus::Rect(xDest, yDest, nDestWidth, nDestHeight), xSrc, ySrc, nSrcWidth, nSrcHeight, Gdiplus::UnitPixel)==0;
				}
				else
				{
					HDC dc = pimage->GetDC();
					BLENDFUNCTION bf;
					bf.BlendOp = AC_SRC_OVER;
					bf.BlendFlags = 0;
					bf.SourceConstantAlpha = 0xff;
					bf.AlphaFormat = AC_SRC_ALPHA;
					BOOL bResult = ::AlphaBlend( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 
						dc, xSrc, ySrc, nSrcWidth, nSrcHeight, bf );
					pimage->ReleaseDC();
					return bResult;
				}
			}
			else
			{
				return __super::Draw(hDestDC, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight);
			}
		}

		BOOL Draw( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc ) throw()
		{
			return( Draw( hDestDC, rectDest.left, rectDest.top, rectDest.right-
				rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top, 
				rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top ) );
		}
		
		BOOL Draw( HDC hDestDC, int xDest, int yDest ) throw()
		{
			return( Draw( hDestDC, xDest, yDest, GetWidth(), GetHeight(), 0, 0, GetWidth(), GetHeight() ) );
		}

		BOOL Draw( HDC hDestDC, const POINT& pointDest ) throw()
		{
			return( Draw( hDestDC, pointDest.x, pointDest.y, GetWidth(), GetHeight(), 0, 0, GetWidth(), GetHeight() ) );
		}

		BOOL Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight ) throw()
		{
			return( Draw( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, GetWidth(), GetHeight() ) );
		}

		BOOL Draw( HDC hDestDC, const RECT& rectDest ) throw()
		{
			return( Draw( hDestDC, rectDest.left, rectDest.top, rectDest.right-rectDest.left, rectDest.bottom-rectDest.top, 0, 0, GetWidth(), GetHeight() ) );
		}

	protected:
		virtual ~Image()
		{
		}
	};

	
	//RectFs class
	__if_exists(Gdiplus::RectF){
	DEFINE_COMPTR(RectFs);
	class RectFs : public IUnknownImpl<RectFs>, public CAtlList<Gdiplus::RectF>
	{
		Public RectFs()
		{
		}

		Public explicit RectFs(RectFsPtr obj)
		{
			AddTailList(obj);
		}

		Public RectFsPtr Clone()
		{
			return new RectFs(this);
		}

		Protected virtual ~RectFs()
		{
		}
	};
	} //__if_exists

	//PointFs class
	__if_exists(Gdiplus::PointF){
	DEFINE_COMPTR(PointFs);
	class PointFs : public IUnknownImpl<PointFs>, public CAtlList<Gdiplus::PointF>
	{
		Protected virtual ~PointFs()
		{
		}
	};
	} //__if_exists

	//PointFArray class
	__if_exists(Gdiplus::PointF){
	DEFINE_COMPTR(PointFArray);
	class PointFArray : public IUnknownImpl<PointFs>, public CAtlArray<Gdiplus::PointF>
	{
		Protected virtual ~PointFArray()
		{
		}
	};
	} //__if_exists

	//Font class
	__if_exists(WTL::CFont){
	DEFINE_COMPTR(Font);
	class Font : public IUnknownImpl<Font>, public WTL::CFont
	{
		Public COLORREF Color;
		Public virtual FontPtr Clone()
		{
			FontPtr obj = new Font();
			LOGFONT lf = LogFont;
			TESTLASTERR( obj->CreateFontIndirect(&lf)!=NULL) ;
			return obj;
		}

		Public Font(HFONT fontHandle=NULL) 
			: WTL::CFont(fontHandle)
		{
			Color = NOTCOLOR;
		}

		Public Font(int pointSize, LPCTSTR faceName, HDC hDC = NULL, bool bold = false, bool italic = false) 
		{
			TESTLASTERR ( CreatePointFont(pointSize, faceName, hDC, bold, italic)!=NULL );
		}


		__if_exists(MSXML2::IXMLDOMElementPtr){
		Public explicit Font(MSXML2::IXMLDOMElementPtr element)
		{
			Serialize(element, false);
		}}

		Public __declspec(property(get=_LogFontGet)) LOGFONT LogFont;
		Public LOGFONT _LogFontGet()
		{
			LOGFONT lf = {0};
			TESTLASTERR( GetLogFont(lf) );
			return lf;
		}

		Public static LONG PointSizeToHeight(int pointSize, HDC hdc=NULL)
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

		Public static LONG HeightToPointSize(int logheight, HDC hdc=NULL)
		{
			bool getDC = hdc == NULL;
			if (getDC) hdc = GetDC(NULL);
			int pointsize = MulDiv(logheight, 72, GetDeviceCaps(hdc, LOGPIXELSY));
			if (getDC ) ReleaseDC(NULL, hdc);

			return abs(pointsize);
		}

		__if_exists(MSXML2::IXMLDOMElementPtr){
		Public void Serialize(MSXML2::IXMLDOMElementPtr element, bool save)
		{
			if (save)
			{
				LOGFONT logFont = LogFont;
				Serialize(logFont, element, save, &Color);
			}
			else
			{
				LOGFONT lf = {0};
				Serialize(lf, element, save, &Color);
				TESTLASTERR( CreateFontIndirect(&lf)!=NULL );
			}
		}

		Public static void Serialize(LOGFONT& logFont, MSXML2::IXMLDOMElementPtr element, bool save, COLORREF* color=NULL)
		{
			if (save)
			{
				element->setAttribute(L"Name", logFont.lfFaceName);
				element->setAttribute(L"Size", HeightToPointSize(logFont.lfHeight));
				element->setAttribute(L"Bold", (LPCTSTR)Convert::ToString(logFont.lfWeight>=700));
				element->setAttribute(L"Italic", (LPCTSTR)Convert::ToString(logFont.lfItalic!=0));
				element->setAttribute(L"Underline", (LPCTSTR)Convert::ToString(logFont.lfUnderline!=0));
				element->setAttribute(L"StrikeOut", (LPCTSTR)Convert::ToString(logFont.lfStrikeOut!=0));
				element->setAttribute(L"CharSet", logFont.lfCharSet);
				if (color!=NULL) element->setAttribute(L"Color", (LPCTSTR)Convert::ToStringColor(*color));
			}
			else
			{
				_variant_t attr;
				attr = element->getAttribute(L"Name");
				if (attr.vt!=VT_NULL) StrCpyN(logFont.lfFaceName, CString(attr), min(LF_FACESIZE, CString(attr).GetLength()+1));
				attr = element->getAttribute(L"Size");
				if (attr.vt!=VT_NULL) logFont.lfHeight = PointSizeToHeight(attr);
				attr = element->getAttribute(L"Bold");
				if (attr.vt!=VT_NULL) logFont.lfWeight = Convert::ToBoolean(CString(attr)) ? 700 : 0;
				attr = element->getAttribute(L"Italic");
				if (attr.vt!=VT_NULL) logFont.lfItalic = Convert::ToBoolean(CString(attr));
				attr = element->getAttribute(L"Underline");
				if (attr.vt!=VT_NULL) logFont.lfUnderline = Convert::ToBoolean(CString(attr));
				attr = element->getAttribute(L"StrikeOut");
				if (attr.vt!=VT_NULL) logFont.lfStrikeOut = Convert::ToBoolean(CString(attr));
				attr = element->getAttribute(L"CharSet");
				if (attr.vt!=VT_NULL) logFont.lfCharSet = (BYTE)Convert::ToInt(CString(attr));
				attr = element->getAttribute(L"Color");
				if (attr.vt!=VT_NULL && color!=NULL) *color = Convert::ToColor(CString(attr));
			}
		}}

		__if_exists(MSXML2::IXMLDOMElementPtr){
		Public static FontPtr FromXml(CString xml)
		{
			MSXML2::IXMLDOMDocumentPtr xmlDoc = NULL;
			TESTHR(xmlDoc.CreateInstance("Msxml2.DOMDocument.3.0"));
			if (xmlDoc->loadXML((LPCTSTR)xml)==VARIANT_FALSE)
				ThrowComError(L"Could not load font XML!");
			return new Font(xmlDoc->documentElement);
		}}

		__if_exists(MSXML2::IXMLDOMElementPtr){
		Public CString ToXml()
		{
			MSXML2::IXMLDOMDocumentPtr xmlDoc = NULL;
			TESTHR(xmlDoc.CreateInstance("Msxml2.DOMDocument.3.0"));
			MSXML2::IXMLDOMElementPtr fontElement = xmlDoc->createElement(L"Font");
			xmlDoc->appendChild(fontElement);
			Serialize(fontElement, true);
			return (LPCTSTR)xmlDoc->xml;
		}}



		Protected virtual ~Font(void)
		{
		}
	};
	} //__if_exists

	//Brush class
	__if_exists(WTL::CBrush){
	DEFINE_COMPTR(Brush);
	class Brush : public IUnknownImpl<Brush>, public WTL::CBrush
	{
	public:
		virtual BrushPtr Clone()
		{
			LOGBRUSH lb;
			TESTLASTERR( GetLogBrush(&lb)!=0 );
			BrushPtr obj = new Brush;
			TESTLASTERR( obj->CreateBrushIndirect(&lb)!=NULL );
			return obj;
		}

		Brush(HBRUSH brushHandle=NULL) 
			: WTL::CBrush(brushHandle)
		{
		}

	protected:
		virtual ~Brush(void)
		{
		}
	};
	} //__if_exists

	//Icon class
	__if_exists (WTL::CIcon){
	DEFINE_COMPTR(Icon);
	class Icon : public IUnknownImpl<Icon>, public WTL::CIcon
	{
	public:
		Icon(HICON iconHandle=NULL) 
			: WTL::CIcon(iconHandle)
		{
		}

		virtual IconPtr Clone()
		{
			return new Icon(CopyIcon());
		}

		IconPtr GetMirrored()
		{
			ICONINFO iconInfo = {0};
			GetIconInfo(&iconInfo);
			ImagePtr colorImage;
			if (iconInfo.hbmColor!=NULL)
			{
				colorImage = new Image(iconInfo.hbmColor);
				colorImage = colorImage->GetMirrored();
				iconInfo.hbmColor = *(colorImage);
			}
			ImagePtr maskImage;
			if (iconInfo.hbmMask!=NULL)
			{
				maskImage = new Image(iconInfo.hbmMask);
				maskImage  = maskImage->GetMirrored();
				iconInfo.hbmMask = *(maskImage);
			}
			IconPtr icon = new Icon();
			icon->CreateIconIndirect(&iconInfo);
			return icon;
		}

	protected:
		virtual ~Icon(void){}
	};
	} //__if_exists

	//Rgn class
	__if_exists (WTL::CRgn){
	DEFINE_COMPTR(Rgn);
	class Rgn : public IUnknownImpl<Rgn>, public WTL::CRgn
	{
	public:
		Rgn(HRGN hRgn=NULL) 
			: WTL::CRgn(hRgn)
		{
		}

		virtual RgnPtr Clone()
		{
			RgnPtr ret = new Rgn();
			ret->CreateRectRgn(0,0,0,0);
			ret->CopyRgn(*this);
			return ret;
		}

	protected:
		virtual ~Rgn(void)
		{
		}
	};
	} //__if_exists

	//Menu class
	__if_exists (WTL::CMenu){
	DEFINE_COMPTR(Menu);
	class Menu : public IUnknownImpl<Menu>, public WTL::CMenu
	{
		Public Menu(HMENU menuHandle = NULL)
			: WTL::CMenu(menuHandle)
		{
		}

		Public MenuPtr Clone()
		{
			return Clone(false);
		}

		Public MenuPtr Clone(bool cloneBitmaps)
		{
			return new Menu(CloneMenu(*this, cloneBitmaps));
		}

	protected:
		HMENU CloneMenu(HMENU menuHandle, bool cloneBitmaps)
		{
			WTL::CMenuHandle menu(menuHandle);
			WTL::CMenu newMenu;
			
			TESTLASTERR( newMenu.CreatePopupMenu() );
			for (int i=0; i<menu.GetMenuItemCount(); i++)
			{
				CString textBuffer;
				MENUITEMINFO mii = {0};
				mii.cbSize = sizeof mii;
				mii.cch = 255;
				mii.dwTypeData = textBuffer.GetBuffer(255);
				mii.fMask = MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_DATA | MIIM_FTYPE| MIIM_ID | MIIM_STATE | MIIM_STRING | MIIM_SUBMENU;
				TESTLASTERR( menu.GetMenuItemInfo(i, TRUE, &mii) );
				textBuffer.ReleaseBuffer();

				if (cloneBitmaps)
				{
					if (mii.hbmpChecked!=NULL) mii.hbmpChecked = (HBITMAP)CopyImage(mii.hbmpChecked, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
					if (mii.hbmpItem!=NULL) mii.hbmpItem = (HBITMAP)CopyImage(mii.hbmpItem, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
					if (mii.hbmpUnchecked!=NULL) mii.hbmpItem = (HBITMAP)CopyImage(mii.hbmpItem, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
					if (mii.hSubMenu!=NULL) mii.hSubMenu = CloneMenu(menuHandle, cloneBitmaps);
				}
				TESTLASTERR(newMenu.InsertMenuItem(i, TRUE, &mii));
			}
			return newMenu.Detach();
		}

		virtual ~Menu(void)
		{
		}
	};
	}

	//Bitmap class
	__if_exists (WTL::CBitmap){
	DEFINE_COMPTR(Bitmap);
	class Bitmap : public IUnknownImpl<Bitmap>, public WTL::CBitmap
	{
	public:
		virtual BitmapPtr Clone()
		{
			HBITMAP hbmp = (HBITMAP)CopyImage((HBITMAP)*this, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			TESTLASTERR(hbmp!=NULL);
			BitmapPtr obj = new Bitmap(hbmp);
			return obj;
		}

		Bitmap(HBITMAP bitmapHandle = NULL) 
			: WTL::CBitmap(bitmapHandle)
		{
		}

	protected:
		virtual ~Bitmap(void)
		{
		}
	};
	} //__if_exists

	//ImageList class
	__if_exists (WTL::CImageList){
	DEFINE_COMPTR(ImageList);
	class ImageList : public IUnknownImpl<ImageList>, public WTL::CImageList
	{
	public:
		ImageList(HIMAGELIST hImageList = NULL) : WTL::CImageList(hImageList){}

	protected:
		virtual ~ImageList(void)
		{
			if (!IsNull())
				Destroy();
		}
	}; 
	}// __if_exists

	//Layout class
	DEFINE_COMPTR(Layout);
	class Layout : public IUnknownImpl<Layout>
	{
		Public enum LayoutEnum{
			Default,
			LTR,
			RTL,
		};

		Public Layout(Layout* obj)
		{
			Value = obj->Value;
		}

		Public virtual LayoutPtr Clone()
		{
			return new Layout(this);
		}

		Public virtual bool Equals(Layout* obj)
		{
			return obj!=NULL && 
				obj->Value==Value;
		}

		Public Layout(LayoutEnum value=Default)
		{
			Value = value;
		}

		Public LayoutEnum Value;

		Public CString ToString()
		{
			switch(Value){
			case Default: return L"Default";
			case RTL: return L"RTL";
			case LTR: return L"LTR";
			default: 
				AtlThrow(E_INVALIDARG);
			};
		}

		Public static LayoutPtr FromString(CString string)
		{
			if ( string.CompareNoCase(L"RTL")==0 ) return new Layout(RTL);
			else if ( string.CompareNoCase(L"LTR")==0 ) return new Layout(LTR);
			else return new Layout(Default);
		}
	};


}
