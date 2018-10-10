#pragma once
#define _MAD_BASE_

#include <atlbase.h>
#include <atlstr.h>
#include <atltime.h>
#include <atlfile.h>
#include <atlwin.h>
#include <atlimage.h>
#include <atlpath.h>
#include <atltypes.h>
#include <atlcoll.h>
#include <atlenc.h>
#include <mshtml.h>
#include <comdef.h>
#include <comdefsp.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <commdlg.h>
#include <Richedit.h>
#include <math.h>
#include <stdlib.h>

#pragma comment(lib, "version.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "comctl32.lib")

namespace Mad
{
	//win32
	#define ILC_MIRROR		0x00002000      // Mirror the icons contained, if the process is mirrored
	#define CP_WINUNICODE	1200
	#define NOTCOLOR		((COLORREF)-1)
	#define HIMETRIC_INCH	2540
	_COM_SMARTPTR_TYPEDEF(IShellLink, __uuidof(IShellLink));
	_COM_SMARTPTR_TYPEDEF(IShellFolder, __uuidof(IShellFolder));
	
	//My Macro	
	#ifndef __MFUNCTION__
	#define __MFUNCTION__ __FUNCTION__
	#endif

	//Property Macro
	#define _prop_get(pre, type, name, post) __declspec(property(get=_##name##_get, put=_##name##_set)) type name; pre type _##name##_get() post
	#define _prop_set(pre, type, name, post) __declspec(property(get=_##name##_get, put=_##name##_set)) type name; pre void _##name##_set(type value) post
	#define prop_get(type, name) _prop_get(##, type, name, ##)
	#define prop_set(type, name) _prop_set(##, type, name, ##)
	#define virtual_prop_get(type, name) _prop_get(virtual, type, name, ##)
	#define virtual_prop_set(type, name) _prop_set(virtual, type, name, ##)

	//quick debug
	#ifdef _DEBUG
	#define ms(a)	::MessageBox(0, CString(a), 0, MB_OK)
	#define merr	WinError::ShowLastErr()
	#define ma(a)	m::QuickBox((int)a) //see information under constructor
	#define m1		ms(L"Flag1")
	#define m2		ms(L"Flag2")
	#define m3		ms(L"Flag3")
	#define m4		ms(L"Flag4")
	#define m5		ms(L"Flag5")
	#endif

	//C# Compatibility
	#define Public public:
	#define Protected protected:
	#define Private private:

	#define ThrowComError(msg) Mad::WinError::ThrowComErrorImpl(msg, __FILE__, __LINE__, __MFUNCTION__)
	#define ThrowComError2(hr, msg) Mad::WinError::ThrowComErrorImpl(hr, msg, __FILE__, __LINE__, __MFUNCTION__)
	#define AtlThrowWin32(err) AtlThrow(HRESULT_FROM_WIN32(err))
	#define AtlThrowLastWin32() AtlThrowWin32(GetLastError())
	#define TESTHR(hr) do{HRESULT val=hr; if (FAILED(val)) AtlThrow(val);}while (0)
	#define TESTLASTERR(val) do{if ((val)==FALSE) AtlThrowLastWin32();}while (0)
	#define _TOBOOL(a) ((a)!=0)
}

namespace Mad
{
	#define EQUALS(pobj1, pobj2) (((pobj1==NULL)==(pobj2==NULL)) && (pobj1==NULL || pobj1->Equals(pobj2)))
	#define DEFINE_ICOMPTR(x) interface x; typedef CComPtr<x> x##Ptr;
	#define DEFINE_COMPTR(x) class x; typedef CComPtr<x> x##Ptr;
	#define DEFINE_COLLECTION(x) class x; typedef CComPtr<x> x##Ptr;  \
		DEFINE_COMPTR(x##Array); class x##Array : public Array<x##Array, x##Ptr>{ Protected virtual ~x##Array(){} }; \
		DEFINE_COMPTR(x##List); class x##List : public List<x##List, x##Ptr>{ Protected virtual ~x##List(){} };
	#define DEFINE_COLLECTIONEC(x) class x; typedef CComPtr<x> x##Ptr;  \
		DEFINE_COMPTR(x##Array); class x##Array : public ArrayEC<x##Array, x##Ptr>{ Protected virtual ~x##Array(){} }; \
		DEFINE_COMPTR(x##List); class x##List : public ListEC<x##List, x##Ptr>{ Protected virtual ~x##List(){} };
	
	//class Nullable
	template<class T>
	class Nullable
	{
		Public Nullable() 
		{
			Clear();
		}

		Public Nullable(T& value) 
			: _Value(value)
			, _HasValue(true)
		{
		}


		Public void Clear()
		{
			_Value = T();
			_HasValue = false;
		}

		//HasValue
		__declspec(property(get=_HasValueGet)) bool HasValue;
		Public bool _HasValueGet()
		{
			return _HasValue;
		}

		//Value
		__declspec(property(get=_ValueGet,put=_ValueSet)) T& Value;
		Public T& _ValueGet()
		{
			if (!HasValue)
				ThrowComError(L"Could not get value on NULL object!");
			return _Value;
		}
		Public void _ValueSet(T& value)
		{
			_Value = value;
			_HasValue = true;
		}

		T& operator=(T value)
		{
			Value = value;
			return Value;
		}

		operator T&()
		{
			return Value;
		}

		Private T _Value;
		Private bool _HasValue;
	};

	//class IUnknownImpl
	template<class T, class TBase=IUnknown>
	class IUnknownImpl : public TBase
	{
		Public IUnknownImpl()
			: RefCount(0)
		{
		}

		Public virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID /*riid*/, void __RPC_FAR *__RPC_FAR* /*ppvObject*/)
		{
			return E_NOINTERFACE;
		}

		Public virtual ULONG STDMETHODCALLTYPE AddRef(void)
		{
			return InterlockedIncrement(&RefCount);
		}

		Public virtual ULONG STDMETHODCALLTYPE Release(void)
		{
			LONG ret = InterlockedDecrement(&RefCount);
			if (ret<=0)
				delete this;
			return ret;
		}

		Protected virtual ~IUnknownImpl() {}


		Private LONG RefCount;
	};

	//class IUnknownImpl
	template<class TBase=IUnknown>
	class IUnknownImpT : public IUnknownImpl<IUnknown, TBase>
	{
		Protected virtual ~IUnknownImpT() { }
	};

	//class IUnknownImpl
	class IUnknownImp : public IUnknownImpT<>
	{
		Protected virtual ~IUnknownImp() { }
	};
	
	//Convert Class
	class Convert
	{
		__if_exists(Gdiplus::REAL){
		Public static Gdiplus::REAL ToReal(CString value)
		{
			return (Gdiplus::REAL)ToDouble(value);
		}}

		__if_exists(Gdiplus::PointF){
		Public static Gdiplus::PointF ToPointF(CString value)
		{
			Gdiplus::SizeF ret = ToSizeF(value);
			return Gdiplus::PointF(ret);
		}}


		__if_exists(Gdiplus::SizeF){
		Public static Gdiplus::SizeF ToSizeF(CString value)
		{
			int comma = value.Find(',');
			if (comma==-1)
				return Gdiplus::SizeF();

			Gdiplus::REAL cx = ToReal(value.Left(comma));
			Gdiplus::REAL cy = ToReal(value.Right(comma));
			return Gdiplus::SizeF(cx, cy);
		}}

		//format: "cx,cy"
		Public static CSize ToSize(CString str)
		{
			int count = str.GetLength();

			const int nParam = 2;
			CString val[nParam];
			int j=0;

			for (int i=0; i<count; i++)
			{
				if (str.GetAt(i)==',')
				{
					j++;
					if (j+1>nParam) break;
				}
				else
					val[j] += str.GetAt(i);
			}

			return CSize (ToInt(val[0]), ToInt(val[1]));
		}

		Public static bool IsValidGuidFormat(CString value)
		{
			GUID guid;
			BSTR bstr = value.AllocSysString();
			HRESULT hResult = CLSIDFromString(bstr, &guid);
			SysFreeString(bstr);
			return SUCCEEDED(hResult)!=FALSE;
		}

		Public static GUID ToGuid(CString value)
		{
			GUID guid;
			BSTR bstr = value.AllocSysString();
			CLSIDFromString(bstr, &guid);
			SysFreeString(bstr);
			return guid;
		}
		
		Public static CString ToString(GUID value)
		{
			OLECHAR lpsz[255];
			StringFromGUID2(value, lpsz, sizeof lpsz);
			return CString(lpsz);
		}

		// return string from in following Format: yyyy/mm/dd-hh:mm:ss
		Public static CString ToString(CTime& value)
		{
			CString str;
			str.Format(L"%04d/%02d/%02d-%02d:%02d:%02d", 
				value.GetYear(), value.GetMonth(), value.GetDay(),
				value.GetHour(), value.GetMinute(), value.GetSecond());
			return str;
		}

		Public static bool IsValidTimeFormat(CString value)
		{
			value.Trim();
			CString format = L"2000/01/01-00:00:00";
			return value.GetLength()==format.GetLength();
		}

		// str Format yyyy/mm/dd-hh:mm:ss
		// @param value must retrieved from Time_GetString
		Public static CTime ToTime(CString value)
		{
			value.Trim();
			if ( !IsValidTimeFormat(value) )
			{
				ATLASSERT( false ); //invalid format
				return CTime();
			}

			int nYear = Convert::ToInt(value.Mid(0,4), 10);
			int nMonth = Convert::ToInt(value.Mid(5,2), 10);
			int nDay = Convert::ToInt(value.Mid(8,2), 10);
			int nHour= Convert::ToInt(value.Mid(11,2), 10);
			int nMin = Convert::ToInt(value.Mid(14,2), 10);
			int nSec = Convert::ToInt(value.Mid(17,2), 10);

			return CTime(nYear, nMonth, nDay, nHour, nMin, nSec);
		}
		
		// @return number of point that copied to lpPoint
		Public static void ToPoints(CString str, CAtlArray<CPoint>* ppoints)
		{
			str += ','; //last char must ',' to calculate last number

			CString val;
			CPoint pt;
			int j=0;
			for (int i=0; i<str.GetLength(); i++)
			{
				if (str.GetAt(i)==',')
				{
					if ( j%2 == 0)
					{
						pt.x = Convert::ToInt(val);
					}
					else
					{
						pt.y = Convert::ToInt(val);
						ppoints->Add(pt);
					}

					j++;
					val.Empty();
 				}
				else
					val += str.GetAt(i);
			}
		}

		Public static CPoint ToPoint(CString value)
		{
			return ToSize(value);
		}

		// retrieve Rect from string
		// format "left, top, right, bottom"
		Public static CRect ToRect(CString value)
		{ 
			int count = value.GetLength();
			CString val[4];
			int j=0;

			for (int i=0; i<count; i++)
			{
				if (value.GetAt(i)==',')
				{
					j++;
					if (j+1>4) break;
				}
				else
					val[j] += value.GetAt(i);
			}

			return CRect (ToInt(val[0]), ToInt(val[1]), ToInt(val[2]), ToInt(val[3]));
		}

			// convert rect to, "left,top,right,bottom" string
		Public static CString ToString(CRect rect)
		{
			CString str;
			str.Format(L"%d,%d,%d,%d", rect.left, rect.top, rect.right, rect.bottom);
			return str;
		}

		__if_exists(_variant_t){
		Public static COLORREF ToColor(_variant_t value, COLORREF defValue)
		{
			return (value.vt!=VT_NULL) ? ToColor(CString(value)) : defValue;
		}}

		Public static COLORREF ToColor(CString value)
		{
			if (value.GetLength()==0)
				return 0;

			//find from hex string
			if (value[0]=='#')
			{
				value.Replace(L"#", L"");
				return RGB( Convert::ToInt(value.Mid(0,2), 16), Convert::ToInt(value.Mid(2,2), 16), Convert::ToInt(value.Mid(4,2), 16));
			}

			if (value.Find(L",")==-1)
			{
				value.Replace(L"0x", L"");
				value.Replace(L"&h", L"");
				value.Replace(L"@h", L"");
				return Convert::ToInt(value, 16);
			}

			//finf from rgb
			int count = value.GetLength();
			CString val[3];
			int j=0;

			for (int i=0; i<count; i++)
			{
				if (value.GetAt(i)==',')
				{
					j++;
					if (j+1>3) break;
				}
				else
					val[j] += value.GetAt(i);
			}

			return RGB(ToInt(val[0]), ToInt(val[1]), ToInt(val[2]));
		}

		Public static CString ToStringColor(COLORREF value, bool hexFormat=false)
		{
			if (hexFormat)
			{
				return ToString(value, L"%x");
			}
			else
			{
				CString rgb;
				rgb.Format(L"%d,%d,%d", (int)GetRValue(value), (int)GetGValue(value), (int)GetBValue(value));
				return rgb;
			}
		}

		Public static double ToDouble(CString value)
		{
			return _tstof(value);
		}

		Public static int ToInt(CString value, int base=0)
		{
			TCHAR* endptr;
			return (int)_tcstoi64(value, &endptr, base);
		}

		Public static INT64 ToInt64(CString value, int base=0)
		{
			TCHAR* endptr;
			return _tcstoi64(value, &endptr, base);
		}

		Public static bool ToBoolean(int value)
		{
			return value!=0;
		}

		__if_exists(_variant_t){
		Public static bool ToBoolean(_variant_t value, bool defValue)
		{
			return (value.vt!=VT_NULL) ? ToBoolean(CString(value)) : defValue;
		}}

		__if_exists(_variant_t){
		Public static int ToInt(_variant_t value, int defValue, int base=0)
		{
			return (value.vt!=VT_NULL) ? ToInt(CString(value), base) : defValue;
		}}

		__if_exists(_variant_t){
		Public static INT64 ToInt64(_variant_t value, INT64 defValue, int base=0)
		{
			return (value.vt!=VT_NULL) ? ToInt64(CString(value), base) : defValue;
		}}

		__if_exists(_variant_t){
		Public static CString ToString(_variant_t value, CString defValue)
		{
			return (value.vt!=VT_NULL) ? value : defValue;
		}}

		Public static bool ToBoolean(CString value)
		{
			value.MakeLower();
			if (value=="yes" || value=="true" || value=="on")
				return true;
			return ToInt(value)!=0;
		}

		Public static CString ToString(bool value)
		{
			return value ? L"1" : L"0";
		}

		Public static CString ToString(INT64 value, LPCTSTR format=L"%I64d")
		{
			CString ret;
			ret.Format(format, value);
			return ret;
		}

		Public static CString ToString(DWORD value, LPCTSTR format=L"%u")
		{
			CString ret;
			ret.Format(format, value);
			return ret;
		}

		Public static CString ToString(CSize value)
		{
			return ToString(value.cx) + L"," + ToString(value.cy);
		}

		Public static CString ToString(CPoint value)
		{
			return ToString(value.x) + L"," + ToString(value.y);
		}

		Public static CString ToString(int value, LPCTSTR format=L"%d")
		{
			CString ret;
			ret.Format(format, value);
			return ret;
		}

		Public static CString ToString(UINT value, LPCTSTR format=L"%u")
		{
			CString ret;
			ret.Format(format, value);
			return ret;
		}

		Public static CString ToString(double value, LPCTSTR format=L"%f")
		{
			CString ret;
			ret.Format(format, value);
			return ret;
		}
	};
}

namespace Mad
{
	class WinError
	{
		Public static CString AppTraceName;

		//ComErrorInfo class
		Private class ComErrorInfo : public IUnknownImpl<ComErrorInfo, IErrorInfo>
		{
			Private HRESULT HResult;
			Private CString Description;
			Private CString SourceFileName;
			Private int SourceLineNo;
			Private CString SourceFunctionName;
			Private CString GetSourceString()
			{
				if (SourceFileName.IsEmpty())
					return L"";

				CString ret;
				CString fileName = PathFindFileName(SourceFileName);
				if (!SourceFileName.IsEmpty()) ret+= L"FileName=" + fileName + L"\n";
				if (SourceLineNo!=0) ret+= L"Line=" + Convert::ToString(SourceLineNo) + L"\n";
				if (!SourceFunctionName.IsEmpty()) ret+= L"FunctionName=" + SourceFunctionName + L"\n";
				return ret;
			}

			Public ComErrorInfo(HRESULT hr, CString description, CString sourceFileName, int sourceLineNo, CString sourceFunctionName)
			{
				HResult = hr;
				Description = description;
				SourceFileName = sourceFileName;
				SourceLineNo = sourceLineNo;
				SourceFunctionName = sourceFunctionName;
			}

			Public virtual HRESULT STDMETHODCALLTYPE GetGUID(GUID* pGUID)
			{
				if (pGUID==NULL)
					return E_INVALIDARG;

				// {9C415194-3E1A-45dd-A62B-A8EA3DB0242D}
				static const GUID id = 
					{ 0x9c415194, 0x3e1a, 0x45dd, { 0xa6, 0x2b, 0xa8, 0xea, 0x3d, 0xb0, 0x24, 0x2d } };

				*pGUID = id;
				return S_OK;
			}

			Public virtual HRESULT STDMETHODCALLTYPE GetSource(BSTR *pBstrSource)
			{
				if (pBstrSource==NULL)
					return E_INVALIDARG;

				*pBstrSource = GetSourceString().AllocSysString();
				return S_OK;
			}
		    
			Public virtual HRESULT STDMETHODCALLTYPE GetDescription(BSTR *pBstrDescription)
			{
				if (pBstrDescription==NULL)
					return E_INVALIDARG;

				CString description = Description;
				if (description.IsEmpty())
					description = GetErrorMessage(HResult);
				*pBstrDescription = description.AllocSysString();
				return S_OK;
			}
		    
			Public virtual HRESULT STDMETHODCALLTYPE GetHelpFile(BSTR* /*pBstrHelpFile*/)
			{
				return E_NOTIMPL;
			}
		    
			Public virtual HRESULT STDMETHODCALLTYPE GetHelpContext(DWORD* /*pdwHelpContext*/)
			{
				return E_NOTIMPL;
			}

			Protected virtual ~ComErrorInfo()
			{
			}
		};

		Public static void ShowLastErr()
		{
			ShowLastErr(GetLastError());
		}

		Public static void ShowLastErr(int err)
		{
			MsgBox( GetErrorMessage(err), L"Error", MB_OK | MB_ICONINFORMATION );
		}

		Public static int MsgBox(LPCTSTR lpText, LPCTSTR lpCaption=NULL, UINT uType=MB_OK)
		{
			HWND hWnd = NULL;
			return MessageBox(hWnd, lpText, lpCaption, uType);
		}

		Public static CString GetErrorMessage(DWORD dwErrorId, LANGID languageId=0)
		{
			return GetSystemMessage(dwErrorId, languageId);
		}
		
		Public static CString GetSystemMessage(DWORD dwMessageId, LANGID languageId=0)
		{
			CString ret;
			LPTSTR lpMsgBuf = NULL;
			DWORD result =  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |  FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
				dwMessageId, languageId,
				(LPTSTR) &lpMsgBuf, 0, NULL );// Process any inserts in lpMsgBuf.
			
			//if languageId failed try to load default
			if (result==0 && languageId!=0 && GetLastError()==ERROR_RESOURCE_LANG_NOT_FOUND)
				ret = GetSystemMessage(dwMessageId, 0);
			else
				ret = lpMsgBuf;


			LocalFree( lpMsgBuf ); 
			return ret;
		}

		Public static __declspec(noreturn) void ThrowComErrorImpl(HRESULT hr, CString description, CStringA sourceFileName, int sourceLineNo, CStringA sourceFunctionName, bool debugAssert=true)
		{
			if (debugAssert && hr!=E_ABORT && HRESULT_CODE(hr)!=ERROR_CANCELLED)
			{
				CString msg;
				if (!AppTraceName.IsEmpty())
					msg += AppTraceName + L": ";
				if (!description.IsEmpty())
					msg += description + L"; ";
				msg += L"AtlThrow: hr = 0x" + Convert::ToString((int)hr, L"%x") + L"\n";
				OutputDebugString(msg);
				bool static lock = false;
				if (!lock)
				{
					lock = true;
					ATLASSERT( false );
					lock = false;
				}
			}
			ComErrorInfo* perror = new ComErrorInfo(hr, description, (LPCTSTR)CA2T(sourceFileName), sourceLineNo, (LPCTSTR)CA2T(sourceFunctionName));
			perror->AddRef();
			throw _com_error(hr, (IErrorInfo*)perror);
		}

		Public static __declspec(noreturn) void ThrowComErrorImpl(CString description, CStringA sourceFileName, int sourceLineNo, CStringA sourceFunctionName, bool debugAssert=true)
		{
			ThrowComErrorImpl(E_FAIL, description, sourceFileName, sourceLineNo, sourceFunctionName, debugAssert);
		}
	};

	__declspec(selectany) CString WinError::AppTraceName;

} //namespace