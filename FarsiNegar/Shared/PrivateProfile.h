#pragma once
#include "m.h"

namespace Mad{

//Bitmap Class
class PrivateProfile
{
	Public static void Flush(Path fileName)
	{
		TESTLASTERR( WritePrivateProfileString(NULL, NULL, NULL, fileName) );
	}

	Public static void Write(CString appName, CString keyName, CString strString, Path fileName)
	{
		TESTLASTERR( WritePrivateProfileString(appName, keyName, strString, fileName) );
	}
	
	Public static void WriteSection(CString appName, CString strString, Path fileName)
	{
		strString += (TCHAR)L'\0';
		TESTLASTERR( WritePrivateProfileSection(appName, strString, fileName) );
	}

	Public static void Write(CString appName, CString keyName, int value, Path fileName)
	{
		CString str;
		str.Format(L"%d", value);
		Write(appName, keyName, str, fileName);
	}

	Public static void Write(CString appName, CString keyName, double fValue, Path fileName)
	{
		CString str;
		str.Format(L"%g", fValue);
		Write(appName, keyName, str, fileName);
	}

	Public static double Get(CString appName, CString keyName, double fDefault, Path fileName)
	{
		CString str;
		str.Format(L"%g", fDefault);
		CString res = Get(appName, keyName, str, fileName, 10);
		TCHAR* endptr;
		return _tcstod(res, &endptr);
	}

	Public static int Get(CString appName, CString keyName, int defaultValue, Path fileName)
	{
		CString defaultValueString;
		defaultValueString.Format(L"%d", defaultValue);
		CString res = Get(appName, keyName, defaultValueString, fileName, 10);
		TCHAR* endptr;
		return _tcstol(res, &endptr, 0);
	}

	Public static CString Get(CString appName, CString keyName, CString defaultValue, Path fileName, int maxBuffer=255)
	{
		CString str;
		GetPrivateProfileString( appName, keyName, defaultValue, str.GetBuffer(maxBuffer), maxBuffer, fileName);
		str.ReleaseBuffer();
		return str;
	}
	
	// Localized  method that work for both Ansi and Unicode; fileName must be Ansi
	Public static CString Get(CString appName, CString keyName, CString defaultValue, Path fileName, UINT codePage, int maxBuffer=255)
	{
		CStringA retA;
		GetPrivateProfileStringA( CT2A(appName), CT2A(keyName), CT2A(defaultValue), retA.GetBuffer(maxBuffer), maxBuffer, CT2A(fileName));
		retA.ReleaseBuffer();
		CString ret = CA2T(retA, codePage);
		return ret;
	}

	Public static void Write(CString appName, CString keyName, CString strString, Path fileName, UINT CodePage)
	{
		bool ret;
		int cbBuf = strString.GetLength()*2+1;
		CHAR* pBuf = new CHAR[cbBuf];
		pBuf[cbBuf-1]=0;
		WideCharToMultiByte(CodePage, 0, strString, strString.GetLength()+1, pBuf, cbBuf, NULL, NULL);
		ret = WritePrivateProfileStringA(CT2A(appName), CT2A(keyName), pBuf, CT2A(fileName))!=0;
		delete pBuf;
		TESTLASTERR(ret);
	}
};

}//namespace Mad