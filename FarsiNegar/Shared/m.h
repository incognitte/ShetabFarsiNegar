#pragma once
#define _MAD_M_
#include "Base.h"
#include "Path.h"
#include "Objects.h"

namespace Mad {

	#define REG_SESSION_MANAGER L"System\\CurrentControlSet\\Control\\Session BoxManager"
	#define REG_PENDINGFILERENAMEOPERATIONS L"PendingFileRenameOperations"
	#define RUNONCE L"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
	#define RUNONCEEX L"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx"
	#define PACKVERSION(major,minor) MAKELONG(minor,major)

	//quick functions
	#undef _L
	#define _L(a) m::LoadString(a)

	//class Param
	class Param
	{
		Public static void Set(CString key, CString value, CString& source)
		{
			Set(key, value, source, L"#$SetParam_NoRemoveValue$#");
		}

		Public static void Set(CString key, int value, CString& source)
		{
			Set(key, Convert::ToString(value), source);
		}

		Public static void Set(CString key, int value, CString& source, int removeIfValue)
		{
			if (value == removeIfValue)
			{
				RemoveKey(key, source);
			}
			else
			{
				Set(key, value, source);
			}
		}

		Public static bool Get(CString key, bool defaultValue, CString source)
		{
			CString value = Get(key, Convert::ToString(defaultValue), source);
			return Convert::ToBoolean(value);
		}

		Public static int Get(CString key, int defaultValue, CString source)
		{
			CString value = Get(key, Convert::ToString(defaultValue), source);
			return Convert::ToInt(value);
		}

		// key is case sensitive
		// source should not have additional space around = and delimiter
		Public static CString FastGet(CString key, LPCTSTR defaultValue, CString& source, CString delimiter=L";")
		{
			int start = source.Find(key + L"=");
			if (start==-1)
				return defaultValue;

			start = start + key.GetLength() + 1;
			int end = delimiter.GetLength()==1 ? source.Find(delimiter.GetAt(0), start) : source.Find(delimiter, start);
			if (end==-1) end = source.GetLength();
			return source.Mid(start, end-start);
		}

		// Find key in source and return it's value; this version use the following format 
		// Name1=Value;Name2=Value;
		// @param defaultValue default value if key not found
		// @return if found return value; otherwise return defaultValue
		Public static CString Get(CString key, LPCTSTR defaultValue, CString source, CString delimiter=L";")
		{
			CString ret = defaultValue;
			if (key.IsEmpty())
				return ret;

			//modify source
			source.TrimLeft();
			source.TrimRight();
			source.Replace(delimiter + L" ", delimiter);
			source.Replace(L"= ", L"=");
			source.Replace(L" =", L"=");
			
			//create copy of source and make it lowercase
			CString string = source;
			string.MakeLower();

			//prepare key and make it lowercase
			key.MakeLower();
			key = key + L"=";

			//find indexes
			int iKeyStart = string.Find(delimiter + key);
			if (iKeyStart!=-1) iKeyStart++;
			if ( key==string.Left(key.GetLength()) ) iKeyStart = 0;

			int iValueStart = iKeyStart + key.GetLength();
			int iValueEnd = string.Find(delimiter, iValueStart);
			iValueEnd = (iValueEnd==-1) ? string.GetLength() : iValueEnd;

			//return value
			if ( iKeyStart!=-1 )
			{
				ret = source.Mid(iValueStart, iValueEnd-iValueStart);
			}
			return ret;
		}

		Public static void RemoveEmptyKeys(CString& source)
		{
			int iStart=0;
			CString key = GetNextKey(source, iStart);
			while ( !key.IsEmpty() )
			{
				CString value = Get(key, L"", source);
				if ( value.IsEmpty() )
					RemoveKey(key, source);
			
				key = GetNextKey(source, iStart);
			}
		}

		Public static void RemoveKey(CString key, CString& source)
		{
			Set(key, L"", source, L"");
		}

		// update param string with another param string
		Public static void Update(CString newParam, CString& source)
		{
			int iStart = 0;
			while(true)
			{
				//get newParam key
				CString key = GetNextKey(newParam, iStart);
				if (key.IsEmpty())
					break;

				//update source
				CString value = Get(key, L"", newParam);
				Set(key, value, source);
			}
		}

		// add Semicolon in end of source if not exist
		Public static void AddSemicolon(CString& source)
		{
			if (source.Right(1)!=L";") source+=L";";
		}

		// @param iStart to 0 to get key from start
		// @return key at iStart; empty string if not any key exist
		// @note to enumurate all key set iStart to zero then loop until method return empty string
		Public static CString GetNextKey(CString source, int& iStart)
		{
			//find next equal sign (=)
			int iEnd = source.Find(L'=', iStart);
			if (iEnd==-1)
			{
				iStart = source.GetLength();
				return L"";
			}

			//ignore additonal simiColon (;;;)
			while (true)
			{
				int iColon = source.Find(L';', iStart);
				if (iColon>=iEnd || iColon==-1) break;
				iStart = iColon+1;
			}

			//find key
			CString ret;
			ret = source.Mid(iStart, iEnd-iStart);
			ret.TrimLeft();
			ret.TrimRight();
			
			//set next iStart
			iStart = source.Find(L';', iEnd)+1;
			if (iStart==0) iStart = source.GetLength();

			return ret;
		}


		// @param removeIfValue if value equal to removeIfValue, then Key removed from source;
		// this is usefull when value used as a default value when using Get
		Public static void Set(CString key, CString value, CString& source, CString removeIfValue)
		{
			if (key.IsEmpty())
				return;

			//create new phrase "key=value;"
			CString strPhrase;
			strPhrase.Format(L"%s=%s;", (LPCTSTR)key, (LPCTSTR)value);

			//modify source
			source.TrimLeft();
			source.TrimRight();
			source.Replace(L"; ", L";");
			source.Replace(L"= ", L"=");
			source.Replace(L" =", L"=");
			
			//create copy of source and make it lowercase
			CString string = source;
			string.MakeLower();

			//prepare key and make it lowercase
			key.MakeLower();
			key = key + L"=";

			//find indexes
			int iKeyStart = string.Find(L";" + key);
			if (iKeyStart!=-1) iKeyStart++;
			if ( key==string.Left(key.GetLength()) ) iKeyStart = 0;

			int iValueStart = iKeyStart + key.GetLength();
			int iValueEnd = string.Find(L";", iValueStart);
			iValueEnd = (iValueEnd==-1) ? string.GetLength() : iValueEnd+1; //+1 for ";" (remove it for Convert::ToString)

			//remove phrase
			if ( value.CompareNoCase(removeIfValue)==0 )
			{
				//if iKeyStart!=-1 then remove previous phrase; otherwise do nothing
				if (iKeyStart!=-1) 
					source.Delete(iKeyStart, iValueEnd - iKeyStart);
			}
			//add value to source
			else if ( iKeyStart==-1 )
			{
				//	if strPhrase not started at first and source not has trailing ";"), add ";"
				if ( source.GetLength()>0 && source.Right(1) != L";" )
					source += L";" + strPhrase;
				else
					source += strPhrase;
			}
			//update value
			else
			{
				source = source.Left(iKeyStart) + strPhrase + source.Mid(iValueEnd);
			}
		}
	};


	//m class
	class m
	{
		Public enum GetRootModes{
			grmRead,
			grmWrite,
			grmReadWrite,
		};

		Public enum PlayMetaFileMode{
			pmfNormal,
			pmfFlipH, //flip horizontally
			pmfFlipV, //flip vertically
			pmfFlipHV, //flip horizontally and vertically; same as pmfRotate180
			pmfRotate90,
			pmfRotate180, //same as pmfFlipHV
			pmfRotate270 ,
		};

		//Reg class
		Public class Reg
		{
			Public static bool IsKeyExists(HKEY keyHandle, CString subKey)
			{
				CRegKey key;
				LONG res = key.Open(keyHandle, subKey, KEY_READ);
				return res==ERROR_SUCCESS;
			}

			// Get all entries in sepecified subkey
			Public static StringListPtr GetEntries(HKEY keyHandle, CString subKey)
			{
				CRegKey key;
				LONG res = key.Open(keyHandle, subKey, KEY_ALL_ACCESS);
				if ( res!=ERROR_SUCCESS )
					AtlThrowWin32(res);

				StringListPtr pentries = new StringList();
				CString valueString;
				DWORD dwValueName = 1000;
				LPTSTR pszValueName = valueString.GetBuffer(dwValueName);
				int nIndex = 0;

				while ( RegEnumValue(keyHandle, nIndex++, pszValueName, &dwValueName, 0, NULL, NULL, NULL) == ERROR_SUCCESS)
				{
					pentries->AddTail( pszValueName );
					dwValueName = 500;
				}

				valueString.ReleaseBuffer();
				return pentries;
			}

			// @param keys [out] retrun subkeys under subKey
			Public static StringListPtr GetSubKeys(HKEY keyHandle, CString subKey)
			{
				CRegKey key;
				LONG res = key.Open(keyHandle, subKey, KEY_ALL_ACCESS);
				if ( res!=ERROR_SUCCESS )
					AtlThrowWin32(res);

				StringListPtr pret = new StringList();

				CString keyString;
				DWORD cbKey = 1000;
				LPTSTR pszKey = keyString.GetBuffer(cbKey);
				FILETIME fileTime;
				int nIndex = 0;
				while ( RegEnumKeyEx(keyHandle, nIndex++, pszKey, &cbKey, 0, NULL, NULL, &fileTime) == ERROR_SUCCESS)
				{
					pret->AddTail( pszKey );
					cbKey = 1000;
				}

				keyString.ReleaseBuffer();
				return pret;
			}
			
			Public static void DelValue(HKEY keyHandle, CString subKey, CString entry)
			{
				CRegKey key;
				LONG res = key.Create(keyHandle, subKey, 0, 0, KEY_ALL_ACCESS);
				if ( res!=ERROR_SUCCESS )
					AtlThrowWin32(res);

				res = key.DeleteValue(entry);
				if ( res!=ERROR_SUCCESS )
					AtlThrowWin32(res);
			}
			
			Public static void Write(HKEY keyHandle, CString subKey, CString entry, CString value)
			{
				CRegKey key;
				LONG res = key.Create(keyHandle, subKey, 0, 0, KEY_ALL_ACCESS);
				if ( res!=ERROR_SUCCESS )
					AtlThrowWin32(res);
				
				res = key.SetStringValue(entry, value);
				if ( res!=ERROR_SUCCESS )
					AtlThrowWin32(res);
			}

			Public static CString GetString(HKEY keyHandle, CString subKey, CString entry)
			{
				return Get(keyHandle, subKey, entry, L"");
			}

			Public static CString Get(HKEY keyHandle, CString subKey, CString entry, LPCTSTR defaultValue)
			{
				CRegKey key;
				LONG res = key.Open(keyHandle, subKey, KEY_READ);
				if ( res!=ERROR_SUCCESS )
					return defaultValue;

				DWORD cbData=0;
				DWORD type = REG_SZ;

				res = RegQueryValueEx(key, entry, 0, &type , NULL, &cbData); //get string size
				if ( res!=ERROR_SUCCESS ) return defaultValue;
				
				CString str;
				res = RegQueryValueEx(key, entry, 0, &type, (LPBYTE)str.GetBuffer(cbData), &cbData);
				str.ReleaseBuffer();
				if ( res!=ERROR_SUCCESS ) return defaultValue;
				
				return str;
			}

			Public static void Write(HKEY keyHandle, CString subKey, CString entry, int value)
			{
				CRegKey key;
				LONG res = key.Create(keyHandle, subKey, 0, 0, KEY_WRITE);
				if ( res!=ERROR_SUCCESS )
					AtlThrowWin32(res);
				
				res = key.SetDWORDValue(entry, (DWORD)value);
				if ( res!=ERROR_SUCCESS )
					AtlThrowWin32(res);
			}

			Public static int GetInt(HKEY keyHandle, CString subKey, CString entry)
			{
				return Get(keyHandle, subKey, entry, 0);
			}

			Public static bool GetBoolean(HKEY keyHandle, CString subKey, CString entry, bool defaultValue)
			{
				return Get(keyHandle, subKey, entry, defaultValue)!=0;
			}

			Public static int Get(HKEY keyHandle, CString subKey, CString entry, int defaultValue)
			{
				CRegKey key;
				LONG res = key.Open(keyHandle, subKey, KEY_ALL_ACCESS);
				if ( res!=ERROR_SUCCESS )
					return defaultValue;

				DWORD value;
				res = key.QueryDWORDValue(entry, value);
				if ( res!=ERROR_SUCCESS )
					return defaultValue;

				return value;
			}
			
			//Deletes a subkey and all its descendants. The function will remove the key and all of the key's values from the registry
			Public static void DelKey(HKEY keyHandle, CString subKey)
			{
				LONG res = SHDeleteKey(keyHandle, subKey);
				if ( res!=ERROR_SUCCESS )
					AtlThrowWin32(res);
			}
		};

		//Clipboard class
		Public class Clipboard
		{
			Public static PathListPtr GetPath(HDROP hdrop)
			{
				return GetPath((HANDLE)hdrop);
			}

			Public static PathListPtr GetPath(HANDLE hdrop)
			{
				int size = (int)GlobalSize(hdrop);
				LPDROPFILES pdropFiles = (LPDROPFILES)GlobalLock((HGLOBAL)hdrop);
				TESTLASTERR(pdropFiles!=NULL);
				PathListPtr ret = MultiPath_FromBuffer((LPBYTE)pdropFiles+pdropFiles->pFiles, size-pdropFiles->pFiles);
				TESTLASTERR( GlobalUnlock(hdrop) || GetLastError()==NO_ERROR );
				return ret;
			}

			//@param dropEffect return drop effect; can be NULL
			//@return NULL if clibboard not contain file
			Public static PathListPtr GetPath(DWORD* pdropEffect=NULL)
			{
				if (!IsClipboardFormatAvailable(CF_HDROP))
					return NULL;

				TESTLASTERR(OpenClipboard(NULL));
				try
				{
					//set files
					HANDLE hdrop = GetClipboardData(CF_HDROP);
					PathListPtr pret = GetPath(hdrop);

					//get drop effect
					if (pdropEffect!=NULL)
					{
						UINT dropEffectFormat = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
						if ( IsClipboardFormatAvailable(dropEffectFormat) )
						{
							HANDLE data = GetClipboardData(dropEffectFormat);
							TESTLASTERR(data!=NULL);
							*pdropEffect = *(DWORD*)GlobalLock((HGLOBAL)data);
							TESTLASTERR( GlobalUnlock(data) || GetLastError()==NO_ERROR );
						}
					}

					//close clipboard
					TESTLASTERR(CloseClipboard());
					return pret;
				}
				catch(...)
				{
					CloseClipboard();
					throw;
				}
			}

			Public static void SetPath(PathListPtr ppaths, DWORD dropEffect=DROPEFFECT_COPY, bool emptyClipboard=true)
			{
				ByteArrayPtr ppathBuffer = MultiPath_ToBuffer(ppaths);

				//add sizeof DROPFILES + two zero in end of buffer
				int cb = sizeof DROPFILES + (int)ppathBuffer->GetCount();
				
				//create buffer
				HGLOBAL hglobal = GlobalAlloc(GMEM_MOVEABLE, cb); 
				TESTLASTERR(hglobal!=NULL);
				LPDROPFILES pdropFiles = (LPDROPFILES)GlobalLock(hglobal);
				ZeroMemory(pdropFiles, cb);
				
				//add DROPFILES
				pdropFiles->pFiles = sizeof DROPFILES;
				pdropFiles->fWide = TRUE;
				pdropFiles->fNC = TRUE;
				pdropFiles->pt.x = pdropFiles->pt.y = 0;
				CopyMemory((BYTE*)pdropFiles+pdropFiles->pFiles, ppathBuffer->GetData(), ppathBuffer->GetCount()); 
				
				TESTLASTERR( GlobalUnlock(hglobal) || GetLastError()==NO_ERROR );

				//create DropEffect
				HGLOBAL dropEffectMem = GlobalAlloc(GMEM_MOVEABLE, sizeof DWORD); 
				TESTLASTERR(dropEffectMem!=NULL);
				*((DWORD*)GlobalLock(dropEffectMem))=dropEffect;
				TESTLASTERR( GlobalUnlock(dropEffectMem) || GetLastError()==NO_ERROR );
				
				TESTLASTERR(OpenClipboard(NULL));
				try
				{
					//empty clipboard
					if (emptyClipboard)
						TESTLASTERR(EmptyClipboard());

					//set files
					TESTLASTERR(SetClipboardData(CF_HDROP, hglobal)!=NULL);

					//set drop effect
					UINT dropEffectFormat = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
					TESTLASTERR(SetClipboardData(dropEffectFormat, dropEffectMem)!=NULL);

					//close clipboard
					TESTLASTERR(CloseClipboard());
				}
				catch(...)
				{
					CloseClipboard();
					throw;
				}

				//copy texts
				CString text;
				POSITION pos = ppaths->GetHeadPosition();
				while (pos!=NULL)
					text += ppaths->GetNext(pos).ToString() + L"\r\n";
				text.Trim();
				CopyText(text, false);
			}
			
			Public static bool CopyText(CString text, bool bEmptyClipboard=true)
			{
				return CopyText(CStringA(text), text, bEmptyClipboard);
			}

			// copy text to clibboard with CF_TEXT and CF_UNICODETEXT format
			Public static bool CopyText(CStringA textA, CStringW textW, bool bEmptyClipboard=true)
			{
				bool ret = false;

				if ( OpenClipboard(NULL) )
				{
					if (bEmptyClipboard)
						EmptyClipboard();

					if (!textA.IsEmpty())
					{
						//set ansi string
						HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, textA.GetLength()+1); 
						ATLASSERT( hMem );
						CHAR* pstr = (CHAR*)GlobalLock(hMem);
						ATLASSERT( pstr );
						memcpy(pstr, textA, textA.GetLength()+1); //copy string to memory
						SetClipboardData(CF_TEXT, hMem); //set data
						GlobalUnlock(hMem); 
					}

					if (!textW.IsEmpty())
					{
						//set unicode string
						HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, (textW.GetLength()+1)*2); 
						ATLASSERT( hMem );
						WCHAR* pwstr = (WCHAR*)GlobalLock(hMem);
						ATLASSERT( pwstr );
						memcpy(pwstr, (LPCWSTR)textW, (textW.GetLength()+1)*2); //copy string to memory
						SetClipboardData(CF_UNICODETEXT, hMem);
						GlobalUnlock(hMem); 
					}

					ret = true;
					CloseClipboard();
				}

				return ret;
			}
			
			//!return 0 if not any text; 1 for asi text; 2 for unicode; 3 for both
			Public static int PasteText(CStringA& textA, CStringW& textW)
			{
				int ret = 0;
				if ( !OpenClipboard(NULL) )
					return ret;

				//get ansi text
				if ( IsClipboardFormatAvailable(CF_TEXT) )
				{
					HANDLE hMem = GetClipboardData(CF_TEXT);
					if (hMem!=NULL)
					{
						LPCSTR psz = (LPCSTR)GlobalLock(hMem);
						ATLASSERT(psz);
						textA = psz;
						GlobalUnlock(hMem);
						ret |= 1;
					}
				}

				//get unicode string if available
				if ( IsClipboardFormatAvailable(CF_UNICODETEXT) )
				{
					HANDLE hMem = GetClipboardData(CF_UNICODETEXT);
					if (hMem!=NULL)
					{
						LPCWSTR psz = (LPCWSTR)GlobalLock(hMem);
						ATLASSERT(psz);
						textW = psz;
						GlobalUnlock(hMem);
						ret |= 2;
					}
				}

				CloseClipboard();
				return ret;
			}

			// get text from clipbard and copy it to text
			// @param bUnicodeText if true get unicode text if false always get ansi text
			// @return 0 if failed to get any data; return 1 if get ansi string; return 2 if get unicode string
			Public static int PasteText(CString &text, bool bUnicodeText=true)
			{
				CStringA textA;
				CStringW textW;
				int res = PasteText(textA, textW);
				if (res==0)
					return 0;
				else if (bUnicodeText && Convert::ToBoolean(res&2))
				{
					text = textW;
					return 2;
				}
				else 
				{
					text = textA;
					return 1;
				}
			}

			//Get multipath in GetClipboardData format
			Public static PathListPtr MultiPath_FromBuffer(LPBYTE pbuffer, int bufferLength)
			{
				PathListPtr ppaths = new PathList();
				
				WCHAR* pbuf = (WCHAR*)pbuffer;
				Path file;
				for (int i=0; i<bufferLength/2; i++)
				{
					if (pbuf[i]!=0)
					{
						file.ToString().AppendChar(pbuf[i]);
					}
					else if (!file.IsEmpty())
					{
						ppaths->AddTail(file);
						file = Path();
					}
					else
					{
						break; //empty filename means two null terminated reached
					}
				}
				return ppaths;
			}

			Public static ByteArrayPtr MultiPath_ToBuffer(PathListPtr ppaths)
			{
				//add two zero in end of buffer
				int cb = 2*sizeof WCHAR;
				POSITION pos = ppaths->GetHeadPosition();
				while (pos!=NULL)
					cb += (ppaths->GetNext(pos).ToString().GetLength()+1)*sizeof WCHAR;

				//allocate buffer
				ByteArrayPtr pbuffer = new ByteArray(cb);
				LPBYTE pbuf = pbuffer->GetData();

				//add path
				DWORD offset = 0;
				pos = ppaths->GetHeadPosition();
				while (pos!=NULL)
				{
					//add path
					Path file = ppaths->GetNext(pos);
					StrCpy((LPWSTR)(pbuf+offset), (LPCWSTR)file);
					offset += (file.ToString().GetLength()*sizeof TCHAR);
					
					//add one null terminated zero unicode character
					*(pbuf + offset) = 0; offset +=1;
					*(pbuf + offset) = 0; offset +=1;
				}

				//add two null terminated zero unicode character
				*(pbuf + offset) = 0; offset +=1;
				*(pbuf + offset) = 0; offset +=1;
				*(pbuf + offset) = 0; offset +=1;
				*(pbuf + offset) = 0; offset +=1;
				return pbuffer;
			}

		};
		
		//FileVersion class
		Public class FileVersion
		{
			//strFileVersion could be 1,1,1,1 or 1.1.1.1 format
			Public static void FromString(CString strFileVersion, DWORD& dwFileVersionMS, DWORD& dwFileVersionLS)
			{
				strFileVersion.Replace(L',', L'.');
				int a[4] = {0};

				int iStart = 0;
				int iEnd=-1;
				for (int i=0; i<4 && iStart<strFileVersion.GetLength(); i++)
				{
					iEnd = strFileVersion.Find(L'.', iStart);
					if (iEnd==-1) iEnd = strFileVersion.GetLength();
					a[i] = Convert::ToInt( strFileVersion.Mid(iStart, iEnd-iStart) );
					iStart = iEnd + 1;
				}

				dwFileVersionMS = MAKELONG(a[1], a[0]);
				dwFileVersionLS = MAKELONG(a[3], a[2]);
			}

			// return a value in StringFileInfo; if languageId version not found; use last version that found in file
			Public static bool QueryStringFileInfo(CString& strOut, Path fileName, CString key, LANGID languageId=MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT))
			{
				bool ret = false;
				TCHAR pszFilename[MAX_PATH];
				StrCpy(pszFilename, fileName);

				//get file version size
				DWORD dwHandle;
				DWORD dwSize = GetFileVersionInfoSize(pszFilename, &dwHandle);
				if (dwSize==0)
					return ret;

				//get fileversion buffer
				LPVOID pbuffer = new BYTE[dwSize];
				BOOL res = GetFileVersionInfo(pszFilename, dwHandle, dwSize, pbuffer);
				if (res)
				{
					//Language codepage
					struct LANGANDCODEPAGE 
					{
						WORD wLanguage;
						WORD wCodePage;
					};

					// Read the list of languages and code pages.
					LANGANDCODEPAGE Translate={0};
					LANGANDCODEPAGE* lpTranslate=NULL;
					UINT cbTranslate = 0;
					VerQueryValue(pbuffer, 
						L"\\VarFileInfo\\Translation",
						(LPVOID*)&lpTranslate,
						&cbTranslate);

					for( UINT i=0; i < (cbTranslate/sizeof LANGANDCODEPAGE); i++ )
					{
						Translate = lpTranslate[i];
						if ( lpTranslate[i].wLanguage == languageId || 
							PRIMARYLANGID(lpTranslate[i].wLanguage)==languageId)
						{
							Translate = lpTranslate[i];
							break;
						}
					}

					//Get fileversion buffer
					CString strBlock;
					strBlock.Format(L"\\StringFileInfo\\%04x%04x\\%s", Translate.wLanguage, Translate.wCodePage, (LPCTSTR)key);
					TCHAR pszSubBlock[512];
					StrCpy(pszSubBlock, strBlock);
				
					LPVOID pInfo = NULL;
					UINT nLen = 0;
					if ( !strBlock.IsEmpty() && VerQueryValue(pbuffer, pszSubBlock, &pInfo, &nLen) )
					{
						strOut = (LPCTSTR)pInfo;
						ret = true;

					}
				}

				delete pbuffer;
				return ret;
			}

			Public static bool GetFixedFileInfo(VS_FIXEDFILEINFO* pFixedVersionInfo, Path fileName)
			{
				bool ret = false;
				TCHAR pszFilename[MAX_PATH];
				StrCpy(pszFilename, fileName);

				//get file version size
				DWORD dwHandle;
				DWORD dwSize = GetFileVersionInfoSize(pszFilename, &dwHandle);
				if (dwSize==0)
					return ret;

				LPVOID pbuffer = new BYTE[dwSize];
				BOOL res = GetFileVersionInfo(pszFilename, dwHandle, dwSize, pbuffer);
				if (res)
				{
					//get pFixedInfo
					if (pFixedVersionInfo!=NULL)
					{
						LPVOID pInfo = NULL;
						UINT nLen = 0;
						if (VerQueryValue(pbuffer, L"\\", &pInfo, &nLen))
						{
							*pFixedVersionInfo = *((VS_FIXEDFILEINFO*)pInfo);
							ret = true;
						}
					}
				}

				delete pbuffer;
				return ret;
			}

			// @return 0 The version identical.
			// @return <0 The version1 is less than version2
			// @return >0 The version1 is more than version2
			Public static int Compare(Path file1, Path file2) 
			{
				VS_FIXEDFILEINFO fixedVersionInfo1={0};
				VS_FIXEDFILEINFO fixedVersionInfo2={0};
				GetFixedFileInfo(&fixedVersionInfo1, file1);
				GetFixedFileInfo(&fixedVersionInfo2, file2);
				return Compare(&fixedVersionInfo1, &fixedVersionInfo2);
			}

			Public static int CompareString(CString fileVersion1, CString fileVersion2)
			{
				VS_FIXEDFILEINFO v1={0};
				VS_FIXEDFILEINFO v2={0};
				FromString(fileVersion1, v1.dwFileVersionMS, v1.dwFileVersionLS);
				FromString(fileVersion2, v2.dwFileVersionMS, v2.dwFileVersionLS);
				return Compare(&v1, &v2);
			}

			Public static int Compare(VS_FIXEDFILEINFO* pfixedVersionInfo1, VS_FIXEDFILEINFO* pfixedVersionInfo2)
			{
				int res = pfixedVersionInfo1->dwFileVersionMS - pfixedVersionInfo2->dwFileVersionMS;
				if (res!=0)
					return res;

				res = pfixedVersionInfo1->dwFileVersionLS - pfixedVersionInfo2->dwFileVersionLS;
				return res;
			}

		};

		//Wnd class
		Public class Wnd
		{
			Public static CRect GetWindowRect(HWND hWnd)
			{
				CRect rect;
				TESTLASTERR( ::GetWindowRect(hWnd, &rect) );
				return rect;
			}

			Public static CRect GetClientRect(HWND hWnd)
			{
				CRect rect;
				TESTLASTERR( ::GetClientRect(hWnd, &rect) );
				return rect;
			}

			Public static CString RealGetClassName(HWND hWnd)
			{
				CString name;
				TESTLASTERR( RealGetWindowClass(hWnd, name.GetBuffer(255), 255)!=0 );
				name.ReleaseBuffer();
				return name;
			}
			
			// @note before window destroyed, this property must removed by calling RemovePropString to free allocated string data;
			Public static bool SetPropString(HWND hWnd, CString name, CString value)
			{
				TCHAR* pBuf = new TCHAR[value.GetLength()+1];
				StrCpy(pBuf, value);
				return SetProp(hWnd, name, (HANDLE)pBuf)!=FALSE;
			}

			Public static CString GetPropString(HWND hWnd, CString name)
			{
				HANDLE hData = GetProp(hWnd, name);
				if (!hData) return L"";
				CString ret = (LPCTSTR)hData;
				return ret;
			}

			Public static CString RemovePropString(HWND hWnd, CString name)
			{
				HANDLE hData = RemoveProp(hWnd, name);
				if (!hData) return L"";
				CString ret = (LPCTSTR)hData;
				delete (TCHAR*)hData;
				return ret;
			}
			
			// @return empty string if could not get window class name
			Public static CString GetClassName(HWND hWnd)
			{
				TCHAR buf[255];
				buf[0]=0;
				::GetClassName(hWnd, buf, 255);
				return buf;
			}

			Public static CString GetDlgItemText(HWND hWnd, UINT nId)
			{
				ATLASSERT( IsWindow(hWnd) );
				return GetWindowText( GetDlgItem(hWnd, nId) );
			}

			Public static CString GetWindowText(HWND hWnd)
			{
				int nLength = GetWindowTextLength(hWnd) + 1;
				CString str;
				::GetWindowText(hWnd, str.GetBuffer(nLength), nLength);
				str.ReleaseBuffer();
				return str;
			}
		};

		//File class
		Public class File
		{
			Public enum FindFlagEnum{
				FindFlagFile		=0x0001,
				FindFlagFolder		=0x0002,
				FindFlagRecursive	=0x0004, //when this flag set, subFolser will be searched recurcively
				FindFlagIgnoreHiddenFile =0x0008, //when this flag set, subFolser will be searched recurcively
				FindFlagIgnoreHiddenFolder =0x0010, //when this flag set, subFolser will be searched recurcively
			};

			//make sure, path exist and throw exception if failed
			//use SHCreateDirectoryEx for more return value
			Public static void CreateDirectory(Path path)
			{
				if (path.IsDirectory())
					return; //root such c:\ will raise error in SHCreateDirectoryEx

				int ret = SHCreateDirectoryEx(NULL, path, NULL);
				if (ret!=ERROR_SUCCESS && ret!=ERROR_FILE_EXISTS && ret!=ERROR_ALREADY_EXISTS)
					AtlThrowWin32(ret);
			}

			// remove strDirectory and all of file in it. this method remove all readonly flags
			// @param strOK set "ok" to confirm in compile time
			Public static bool RemoveDirectory(Path folderPath, CString strOK, bool removeReadOnlyFiles=true, CString wildCard=L"*.*")
			{
				if ( strOK.CompareNoCase(L"ok")!=0)
					AtlThrow( E_INVALIDARG );

				if (folderPath.IsRoot())
					AtlThrowWin32(ERROR_BAD_PATHNAME); //could not delete drive

				//return Shell_FileOperation(FO_DELETE, folderPath, NULL, FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI)==0;
				//return false if strDiretory is empty
				if ( !folderPath.IsDirectory() )
					AtlThrowWin32(ERROR_FILE_NOT_FOUND);

				bool ret = true;
				PathListPtr ppaths = Find(folderPath, wildCard, FindFlagFile | FindFlagFolder);
				POSITION pos = ppaths->GetHeadPosition();
				while (pos!=NULL)
				{
					Path path = ppaths->GetNext(pos);
					if (removeReadOnlyFiles) ModifyAttributes(path, FILE_ATTRIBUTE_READONLY, 0);
					if ( path.IsDirectory() )
						ret &= RemoveDirectory(path, strOK, removeReadOnlyFiles);
					else 
						ret &= (DeleteFile(path)!=FALSE);
				}

				//remove strDiretory
				if (removeReadOnlyFiles) ModifyAttributes(folderPath, FILE_ATTRIBUTE_READONLY, 0);
				ret &= (::RemoveDirectory(folderPath)!=FALSE);
				return ret;
			}

			//perfirm path mapping from psrcFiles to destFolder
			Public static PathListPtr CopyFiles(PathListPtr psrcFiles, Path srcFolder, Path destFolder)
			{
				PathListPtr poutDesFiles = new PathList();

				//ensure srcPath has a backslash
				srcFolder.AddBackslash(); 
				destFolder.AddBackslash();

				//build destpath
				Path desFileName;
				POSITION pos = psrcFiles->GetHeadPosition();
				while (pos!=NULL)
				{
					Path file = psrcFiles->GetNext(pos);
					desFileName = Path(file.ToString().Right( file.ToString().GetLength() - srcFolder.ToString().GetLength()));
					poutDesFiles->AddTail( destFolder + desFileName);
				}

				return poutDesFiles;
			}

			// prepare source file and destination file for CopyDirectory.
			// @param outSrcFiles [out] all of source file path will added to this array
			// @param outDestFiles [out] all of destination file path will added to this array
			Public static void CopyDirectory(PathListPtr poutSrcFiles, PathListPtr poutDesFiles, Path srcFolder, Path destFolder, CString wildCard=L"*.*", DWORD findFlags = FindFlagFile | FindFlagRecursive)
			{
				PathListPtr psrcFiles = Find(srcFolder, wildCard, findFlags);
				PathListPtr pdesFiles = CopyFiles(psrcFiles, srcFolder, destFolder);
				poutSrcFiles->AddTailList(psrcFiles);
				poutDesFiles->AddTailList(pdesFiles);
				
			}

			Private static void CopyDirectoryImpl(Path srcFolder, Path destFolder, CString wildCard, bool subFolder, bool move)
			{
				PathListPtr psrcFiles = new PathList();
				PathListPtr pdesFiles = new PathList();

				DWORD pathFindFlags = FindFlagFile;
				if (subFolder) pathFindFlags |= FindFlagFolder | FindFlagRecursive;

				CopyDirectory(psrcFiles, pdesFiles, srcFolder, destFolder, wildCard, pathFindFlags);

				POSITION pos1 = psrcFiles->GetHeadPosition();
				POSITION pos2 = pdesFiles->GetHeadPosition();
				while (pos1!=NULL && pos2!=NULL)
				{
					Path srcFile = psrcFiles->GetNext(pos1);
					Path desFile = pdesFiles->GetNext(pos2);
					if ( srcFile.IsFile() )
					{ 
						CreateDirectory( desFile.GetFolder() );
						if (move)
						{
							if (desFile.FileExists())
								DeleteFile(desFile);
							TESTLASTERR( MoveFile( srcFile, desFile) );
						}
						else
						{
							TESTLASTERR( CopyFile( srcFile, desFile, FALSE) );
						}
					}
					else
					{
						CreateDirectory(desFile);
					}
				}
			}

			Public static void CopyDirectory(Path srcFolder, Path destFolder, CString wildCard=L"*.*", bool subFolder=true)
			{
				return CopyDirectoryImpl(srcFolder, destFolder, wildCard, subFolder, false);
			}

			Public static void MoveDirectory(Path srcFolder, Path destFolder, CString wildCard=L"*.*", bool subFolder=true)
			{
				return CopyDirectoryImpl(srcFolder, destFolder, wildCard, subFolder, true);
			}

			Public static BOOL ModifyAttributes(CString strFilePath, DWORD dwRemove, DWORD dwAdd)
			{
				DWORD dwAttributes = GetFileAttributes( strFilePath );
				if ( dwAttributes==-1 )
					return FALSE;

				dwAttributes |= dwAdd;
				dwAttributes &= ~dwRemove;
				return SetFileAttributes(strFilePath, dwAttributes);

			}

			Public static DWORD GetSize(CString strFilePath)
			{
				WIN32_FIND_DATA findFileData;
				HANDLE hFind = FindFirstFile(strFilePath, &findFileData);
				if (hFind == INVALID_HANDLE_VALUE)
					return FALSE;
				TESTLASTERR(FindClose(hFind));
				return (DWORD)findFileData.nFileSizeLow;
			}

			Public static bool GetVersion(Path fileName, DWORD& dwFileVersionMS , DWORD& dwFileVersionLS)
			{
				dwFileVersionMS = 0;
				dwFileVersionLS = 0;
				VS_FIXEDFILEINFO version;
				bool ret = GetVersion(fileName, &version);
				if (ret)
				{
					dwFileVersionMS = version.dwFileVersionMS;
					dwFileVersionLS = version.dwFileVersionLS;
				}
				return ret;
			}

			Public static bool GetVersion(Path fileName, VS_FIXEDFILEINFO* pVersionInfo)
			{
				bool ret=false;

				DWORD dwHandle; 
				int size = GetFileVersionInfoSize((LPTSTR)(LPCTSTR)fileName, &dwHandle);
				if (size==0)
					return false;

				if (size)
				{
					LPBYTE lpData = new BYTE[size];
					if (GetFileVersionInfo((LPTSTR)(LPCTSTR)fileName, 0, size, lpData))
					{
						UINT dwBytes;
						VS_FIXEDFILEINFO *pVS;
						if (VerQueryValue(lpData, L"\\", (LPVOID*)&pVS, &dwBytes))
						{
							memcpy(pVersionInfo, pVS, sizeof VS_FIXEDFILEINFO);
							ret = true;
						}
					}
					delete lpData;
				}
				return ret;
			}

			Public static CString GetVersionAsString(Path fileName)
			{
				CString ret;
				VS_FIXEDFILEINFO version;
				bool res = GetVersion(fileName, &version);
				if (res)
				{
					ret.Format(L"%d.%d.%d.%d", 
						HIWORD(version.dwFileVersionMS),
						LOWORD(version.dwFileVersionMS),
						HIWORD(version.dwFileVersionLS),
						LOWORD(version.dwFileVersionLS));
				}

				return ret;
			}


			Public static bool DeleteWithErrMessage(Path fileName, CString messageFormat, bool delReadOnlyFile, LANGID languageId = 0)
			{
				bool ret = DeleteWithErrString(fileName, messageFormat, delReadOnlyFile, languageId);
				if (!ret)
				{
					if (!messageFormat.IsEmpty())
						MsgBox(messageFormat, NULL, MB_ICONSTOP);
				}
				return ret;
			}

			// @param messageFormat a string that show message if any error occur
			//  this method replace %1 with fileName and %2 with system error message
			//  if this parameter empty not error message will displayed
			// @param delReadOnlyFile if true do not fail if fileName is readonly
			// @param languageId used to show system error message
			// @return true if fileName deleted; false if delete failed.
			Public static bool DeleteWithErrString(Path fileName, CString& messageFormat, bool delReadOnlyFile, LANGID languageId = 0)
			{
				if (delReadOnlyFile)
					ModifyAttributes(fileName, FILE_ATTRIBUTE_READONLY, 0);

				if ( !DeleteFile(fileName) ) 
				{
					int lastErr = GetLastError();
					messageFormat.Replace(L"%1", fileName);
					messageFormat.Replace(L"%2", Mad::WinError::GetErrorMessage(lastErr, languageId) );
					return false;
				}
				return true;
			}
			
			Public static bool CopyWithErrMessage(Path existingFileName, Path newFileName, CString messageFormat, bool copyOnReadOnlyFile, bool failIfExists, LANGID languageId = 0)
			{
				bool ret = CopyWithErrString(existingFileName, newFileName, messageFormat, copyOnReadOnlyFile, failIfExists, languageId);
				if (!ret)
				{
					if (!messageFormat.IsEmpty())
						MsgBox(messageFormat, NULL, MB_ICONSTOP);
				}
				return ret;
			}

			// @param messageFormat a string that show message if any error occur
			//  this method replace %1 with existingFileName and %2 with newFileName and %3 with system error message
			//  if this parameter empty not error message will displayed
			Public static bool CopyWithErrString(Path existingFileName, Path newFileName, CString& messageFormat, bool copyOnReadOnlyFile, bool failIfExists, LANGID languageId = 0)
			{
				if (copyOnReadOnlyFile)
					ModifyAttributes(newFileName, FILE_ATTRIBUTE_READONLY, 0);
					
				if ( !CopyFile(existingFileName, newFileName, failIfExists) ) 
				{
					int lastErr = GetLastError();
					messageFormat.Replace(L"%1", existingFileName);
					messageFormat.Replace(L"%2", newFileName);
					messageFormat.Replace(L"%3", Mad::WinError::GetErrorMessage(lastErr, languageId) );
					return false;
				}
				return true;
			}

			Public static bool MoveWithErrMessage(Path existingFileName, Path newFileName, CString messageFormat, bool moveOnReadOnlyFile, bool failIfExists, LANGID languageId = 0)
			{
				bool ret = MoveWithErrString(existingFileName, newFileName, messageFormat, moveOnReadOnlyFile, failIfExists, languageId);
				if (!ret)
				{
					if (!messageFormat.IsEmpty())
						MsgBox(messageFormat, NULL, MB_ICONSTOP);
				}
				return ret;
			}

			// @param messageFormat a string that show message if any error occur
			//  this method replace %1 with existingFileName and %2 with newFileName and %3 with system error message
			//  if this parameter empty not error message will displayed
			Public static bool MoveWithErrString(Path existingFileName, Path newFileName, CString& messageFormat, bool moveOnReadOnlyFile, bool failIfExists, LANGID languageId = 0)
			{
				// remove destination file name if found
				if ( newFileName.IsFile() && !failIfExists )
				{
					if ( moveOnReadOnlyFile )
						ModifyAttributes(newFileName, FILE_ATTRIBUTE_READONLY, 0);
					DeleteFile(newFileName);
				}

				//move file
				if ( !MoveFile(existingFileName, newFileName) ) 
				{
					int lastErr = GetLastError();
					messageFormat.Replace(L"%1", existingFileName);
					messageFormat.Replace(L"%2", newFileName);
					messageFormat.Replace(L"%3", Mad::WinError::GetErrorMessage(lastErr, languageId) );
					return false;
				}
				return true;
			}

			Public static void WriteBuffer(ByteArrayPtr buffer, Path fileName)
			{
				CAtlFile file;
				TESTHR( file.Create(fileName, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS) );
				TESTHR( file.Write(buffer->GetData(), (DWORD)buffer->GetCount()) );
				file.Close();
			}

			Public static ByteArrayPtr ReadToBuffer(Path fileName, UINT readCount=-1)
			{
				ByteArrayPtr pbytes = new ByteArray();

				CAtlFile file;
				TESTHR( file.Create(fileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING) );
				ULONGLONG fileSize;
				TESTHR(file.GetSize(fileSize));
				if (readCount != -1) 
					fileSize = max(readCount, fileSize);

				pbytes->SetCount((ULONG)fileSize);
				TESTHR( file.Read(pbytes->GetData(), (DWORD)fileSize) );
				return pbytes;
			}

			Public static StringListPtr ReadAllLines(Path file)
			{
				CString str;
				m::String_LoadFromFile(str, file);
				str.Replace(L"\r\n", L"\n");
				str.Replace(L"\r", L"\n");

				StringListPtr res = new StringList();
				int curPos = 0;
				CString resToken = str.Tokenize(L"\n", curPos);
				while (resToken != L"")
				{
					res->AddTail(resToken);
					resToken = str.Tokenize(L"\n", curPos);
				};
				return res;
			}

			Public static void WriteAllLines(Path file, StringListPtr contents, UINT codePage=CP_UTF8)
			{
				CString delimiter=L"\r\n";
				CString str;
				POSITION pos = contents->GetHeadPosition();
				while (pos!=NULL)
				{
					CString line = contents->GetNext(pos);
					str += line + delimiter;
				}
				str.TrimRight(delimiter);
				m::String_SaveToFile(str, file, codePage);
			}

			Public static PathListPtr Find(Path srcFolder, CString wildCard = L"*.*", DWORD findFlags = FindFlagFile | FindFlagFolder | FindFlagRecursive, StringListPtr pexcludePatterns=NULL)
			{
				PathListPtr pret = new PathList();

				//check source folder
				if (!srcFolder.IsDirectory())
					return pret;

				// build a path with wildcards
				Path findName = srcFolder + Path(wildCard);

				// search in subFolders if findRecursive exist
				if ( Convert::ToBoolean(findFlags & FindFlagRecursive) )
				{
					PathListPtr pdirFolders = Find(srcFolder, L"*.*", FindFlagFolder);
					POSITION pos = pdirFolders->GetHeadPosition();
					while (pos!=NULL)
					{
						Path path = pdirFolders->GetNext(pos);
						pret->AddTailList(Find(path, wildCard, findFlags));
					}
				}

				// start working for files
				WIN32_FIND_DATA findData, *pfindData;
				pfindData = &findData;
				HANDLE findFileHandle = FindFirstFile(findName, pfindData);
				bool working = findFileHandle!=INVALID_HANDLE_VALUE;
				int counter=0;
				
				while (working)
				{
					//find next file in next loop
					counter++;
					if (counter>1)
					{
						working = FindNextFile(findFileHandle, pfindData)!=FALSE;
						if (!working) break;
					}

					// skip . and .. files; otherwise, we'd
					// recur infinitely!
					Path filePath = srcFolder + Path(pfindData->cFileName);
					bool isDirectory = Convert::ToBoolean(pfindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
					bool isDots = false;
					if ( isDirectory )
					{
						if (pfindData->cFileName[0] == '.')
						{
							if (pfindData->cFileName[1] == '\0' ||
								(pfindData->cFileName[1] == '.' &&
									pfindData->cFileName[2] == '\0'))
							{
								isDots = true;
							}
						}
					}
					if (isDots)
						continue;

					//check exclude
					bool excluded = false;
					POSITION posExclude = pexcludePatterns!=NULL ? pexcludePatterns->GetHeadPosition() : NULL;
					while(posExclude!=NULL && !excluded)
					{
						CString pattern = pexcludePatterns->GetNext(posExclude);
						excluded = filePath.MatchSpec(pattern)!=FALSE;
					}
					
					//exclude path
					if (excluded)
					{
					}
					//if it's a directory, recursively search it
					else if (isDirectory)
					{
						//add folder to outFiles if findFolders exist
						if (Convert::ToBoolean(findFlags & FindFlagFolder) && (!Convert::ToBoolean(findFlags & FindFlagIgnoreHiddenFolder) || !Convert::ToBoolean(GetFileAttributes(filePath)&FILE_ATTRIBUTE_HIDDEN)) )
							pret->AddTail( filePath );
					}
					//add file to outFiles if findFiles exist
					else if (Convert::ToBoolean(findFlags & FindFlagFile) && (!Convert::ToBoolean(findFlags & FindFlagIgnoreHiddenFile) || !Convert::ToBoolean(GetFileAttributes(filePath)&FILE_ATTRIBUTE_HIDDEN)) )
					{
						pret->AddTail( filePath );
					}

				}

				if ( findFileHandle!=INVALID_HANDLE_VALUE )
					FindClose(findFileHandle);
				return pret;
			}
		};

		//DDX class
		Public class DDX
		{
			Public void TextRichEdit(BOOL save, HWND richboxHandle, CString& string)
			{
				if (save)
				{
					string = RichEdit_GetTextEx(richboxHandle);
				}
				else
				{
					RichEdit_SetTextEx(richboxHandle, string);
				}
			}

			Public static void Check(BOOL save, HWND checkboxHandle, BOOL& value, bool inverse=false)
			{
				bool tempValue;
				Check(save, checkboxHandle, tempValue, inverse);
				value = tempValue;
			}

			Public static void Check(BOOL save, HWND checkboxHandle, bool& value, bool inverse=false)
			{
				if (save)
				{
					bool checked = SendMessage(checkboxHandle, BM_GETCHECK, 0, 0)==BST_CHECKED;
					value = checked;
					if (inverse) value = !checked;
				}
				else
				{
					bool checked = value;
					if (inverse) checked = !checked;
					SendMessage(checkboxHandle, BM_SETCHECK, checked, 0);
				}
			}

			Public static void CBIndex(BOOL save, HWND comboBoxHandle, int& index)
			{
				//save to data member
				if (save)
				{
					int selectedIndex = (int)SendMessage(comboBoxHandle, CB_GETCURSEL, 0, 0);
					if (selectedIndex!=-1)
						index = selectedIndex;
				}
				//load data to member
				else
				{
					SendMessage(comboBoxHandle, CB_SETCURSEL, (WPARAM)index, 0);
				}

			}

			Public static void CheckFlag(BOOL save, HWND checkboxHandle, DWORD flag, DWORD& flags, bool inverse=false)
			{
				//save to data member
				if (save)
				{
					bool checked = SendMessage(checkboxHandle, BM_GETCHECK, 0, 0)==BST_CHECKED;
					if ( (checked && !inverse) || (!checked && inverse))
					{
						flags |= flag;
					}
					else
					{
						flags &= ~flag;
					}
				}
				else
				{
					bool checked = Convert::ToBoolean(flags & flag) && !inverse;
					SendMessage(checkboxHandle, BM_SETCHECK, checked, 0);
				}
			}
		};

		//Methods

		//@param flags is ActivateKeyboardLayout Flags parameter
		Public static HKL Keyboard_SetLayoutByLangId(LANGID languageId, UINT flags=0)
		{
			HKL phkl[20];
			int count = GetKeyboardLayoutList(20, phkl);

			//search exact
			for (int i=0; i<count; i++)
			{
				LANGID itemLanguageId = LOWORD(phkl[i]);
				if ( itemLanguageId==languageId )
					return ActivateKeyboardLayout(phkl[i], flags);
			}

			//search primary
			for (int i=0; i<count; i++)
			{
				LANGID itemLanguageId = LOWORD(phkl[i]);
				if ( PRIMARYLANGID(itemLanguageId)==PRIMARYLANGID(languageId) )
					return ActivateKeyboardLayout(phkl[i], flags);
			}
			return NULL;
		}

		// @return true if determine that some setup in system request restart
		// add an item to RunOnce so reboot is pending for installers like InstallShield
		Public static bool Reboot_GetPending()
		{
			bool ret = false;  // Return value

			// In Windows 2000 and Windows NT, file rename operations are stored in
			// the PendingFileRenameOperations value.  If there is any data in that
			// value, a reboot is pending.
			CString svValue = Reg::GetString(HKEY_LOCAL_MACHINE, REG_SESSION_MANAGER, REG_PENDINGFILERENAMEOPERATIONS);
			svValue.Trim();
			if (svValue!=L"")
				ret = true;

			// Look for any values in the RunOnce key.  If any values exist, a reboot
			// is pending.
			StringListPtr pentries = Reg::GetEntries(HKEY_LOCAL_MACHINE, RUNONCE);
			if ( !pentries->IsEmpty() )
				ret = true;

			// Look for any values in the RunOnceEx key.  If any values exist, a reboot
			// is pending.
			StringListPtr pentriesEx = Reg::GetSubKeys(HKEY_LOCAL_MACHINE, RUNONCEEX);
			if ( !pentriesEx->IsEmpty() )
				ret = true;

			return ret;
		}

		Public static void Reboot_SetPending()
		{
			Reg::Write(HKEY_LOCAL_MACHINE, RUNONCE, L"ShetabReboot", L"rundll32.exe");
		}

		Public static Path GetWindowsDirectory()
		{
			TCHAR winDir[MAX_PATH];
			::GetWindowsDirectory(winDir, sizeof winDir);
			return Path(winDir);
		}

		__if_exists(MSXML2::IXMLDOMElementPtr){
		Public template<typename T> static T* SerializeClone(T* obj)
		{
			MSXML2::IXMLDOMDocumentPtr xmlDoc;
			TESTHR(xmlDoc.CreateInstance("Msxml2.DOMDocument.3.0"));
			MSXML2::IXMLDOMElementPtr rootElement = xmlDoc->createElement("Root");
			xmlDoc->appendChild(rootElement);
			
			obj->Serialize(rootElement, true);
			T* newObject = new T();
			newObject->Serialize(rootElement, false);
			return newObject;
		}}

		__if_exists(MSXML2::IXMLDOMElementPtr){
		Public template<typename T> static bool SerializeEquals(void* obj1, void* obj2)
		{
			//reference check for same object
			if (obj1==obj2)
				return true;
			//reference check for null
			if ( (obj1==NULL) != (obj2==NULL) )
				return false;
			
			//contents check
			MSXML2::IXMLDOMDocumentPtr xmlDoc;
			TESTHR(xmlDoc.CreateInstance("Msxml2.DOMDocument.3.0"));
			MSXML2::IXMLDOMElementPtr rootElement1 = xmlDoc->createElement("Root");
			MSXML2::IXMLDOMElementPtr rootElement2 = xmlDoc->createElement("Root");
			
			((T*)obj1)->Serialize(rootElement1, true);
			CString xml1 = rootElement1->xml;
			((T*)obj2)->Serialize(rootElement2, true);
			CString xml2 = rootElement2->xml;
			return xml1==xml2;
		}}

		__if_exists(MSXML2::IXMLDOMElementPtr){
		Public static MSXML2::IXMLDOMDocumentPtr Xml_LoadFile(Path file)
		{
			MSXML2::IXMLDOMDocumentPtr xmlDoc;
			TESTHR(xmlDoc.CreateInstance("Msxml2.DOMDocument.3.0"));
			if ( xmlDoc->load((LPCTSTR)file)==VARIANT_FALSE )
				ThrowComError(L"Could not load xml file!\nFile: " + file.ToString());
			return xmlDoc;
		}}
		

		Public static Path GetSystemDirectory()
		{
			TCHAR sysDir[MAX_PATH];
			::GetSystemDirectory(sysDir, sizeof sysDir);
			return Path(sysDir);
		}

		Public static Path GetCurrentDirectory()
		{
			TCHAR curDir[MAX_PATH];
			::GetCurrentDirectory(sizeof curDir, curDir);
			return Path(curDir); 
		}

		Public static Path GetTempFolder()
		{
			CString tempFolder;
			GetTempPath(MAX_PATH, tempFolder.GetBuffer(MAX_PATH));
			tempFolder.ReleaseBuffer();

			CString ret;
			GetLongPathName(tempFolder, ret.GetBuffer(32767), 32767);
			ret.ReleaseBuffer();

			return Path(ret);
		}

		// get root storage pointer to a COM compound files. if file not found try to create it
		// @param grfMode can be ONLY one of the GetRootModes
		// @note ensure call CoInitialize() called before calling this methos
		// @see StgCreateDocfile and StgOpenStorage SDK for more information
		// @throw AtlThrow
		Public static IStoragePtr Storage_GetRoot(Path fileName, GetRootModes grmMode=grmReadWrite)
		{
			IStoragePtr pstorage;
			HRESULT hr = 0;
			DWORD dwFlags =	0;
			if (grmMode==grmRead) dwFlags=STGM_READ | STGM_SHARE_DENY_WRITE;
			if (grmMode==grmWrite) dwFlags=STGM_WRITE | STGM_SHARE_EXCLUSIVE;
			if (grmMode==grmReadWrite) dwFlags=STGM_READWRITE | STGM_SHARE_EXCLUSIVE;

			//try to open Storage
			hr = StgOpenStorage(fileName, NULL, dwFlags, 0, 0, &pstorage);
			
			//try to create if failed to opened in write mode
			if ( FAILED(hr) && (grmMode==grmWrite || grmMode==grmReadWrite) )
			{
				hr = StgCreateDocfile(fileName, dwFlags | STGM_CREATE | STGM_SHARE_EXCLUSIVE , 0, &pstorage);
			}

			//throw if an error occur
			if (FAILED(hr))
				AtlThrow(hr);

			return pstorage;
		}
		
		Public static int MsgBox(LPCTSTR lpText, LPCTSTR lpCaption=NULL, UINT uType=MB_OK)
		{
			HWND hWnd = NULL;
			return MessageBox(hWnd, lpText, lpCaption, uType);
		}

		//eg: if(GetDllVersion(L"comctl32.dll") >= PACKVERSION(4,71)) then ...
		Public static DWORD GetDllVersion(LPCTSTR lpszDllName)
		{
			HINSTANCE hinstDll;
			DWORD dwVersion = 0;

			hinstDll = LoadLibrary(lpszDllName);
			
			if(hinstDll)
			{
				DLLGETVERSIONPROC pDllGetVersion;

				pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hinstDll, "DllGetVersion");

				// Because some DLLs may not implement this function, you
				// must test for it explicitly. Depending on the particular 
				// DLL, the lack of a DllGetVersion function may
				// be a useful indicator of the version.
				
				if(pDllGetVersion)
				{
					DLLVERSIONINFO dvi;
					HRESULT hr;

					ZeroMemory(&dvi, sizeof(dvi));
					dvi.cbSize = sizeof(dvi);

					hr = (*pDllGetVersion)(&dvi);

					if(SUCCEEDED(hr))
					{
						dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
					}
				}
		        
				FreeLibrary(hinstDll);
			}
			return dwVersion;
		}

		// seprate fileName that passed to commandline with rest of commandline
		// @param commandLine commandline that passed to WinMain
		// @param *pfileName return fileName that passed to commandline; can be NULL
		// @return rest of commandline without fileName
		Public static CString CommandLine_Analyse(CString commandLine, Path* pfileName)
		{
			commandLine.TrimLeft();
			if (commandLine.GetLength()==0)
				return L"";

			int iStart = 0;
			int iEnd = 0;
			bool fileFound = false;
			if (commandLine[iStart]==L'"')
			{
				iEnd = commandLine.Find(L'"', 1)+1;
				if (iEnd==0) iEnd = commandLine.GetLength();
				fileFound = true;
			}
			else
			{
				iEnd = commandLine.Find(L' ', 0);
				if (iEnd==-1) iEnd = commandLine.GetLength();
			}

			CString fileCommandString = commandLine.Mid(iStart, iEnd-iStart);
			fileCommandString.Replace(L"\"", L"");
			Path fileCommand(fileCommandString);
			fileCommand.ToString().Trim();

			//guest is fileCommand is file
			if (fileCommand.IsFile()) fileFound = true;
			if (fileCommandString.Left(3).Find(L"\\")!=-1) fileFound = true;
			if (fileCommand.IsURL()) fileFound = true;
			
			if (fileFound)
			{
				if (pfileName!=NULL) *pfileName = fileCommand;
				commandLine.Delete(iStart, iEnd-iStart);
				commandLine.TrimLeft();
			}

			return commandLine;
		}

		// @see EM_GETTEXTEX and EM_SETTEXTEX
		Public static CString RichEdit_GetTextEx(HWND hWndRich, DWORD dwFlags=GT_DEFAULT)
		{
			ATLASSERT( hWndRich != NULL );
			ATLASSERT( (dwFlags & GTL_NUMBYTES)==NULL );//invalid flag

			GETTEXTLENGTHEX gtl;
			gtl.codepage = 1200;
			gtl.flags = GTL_NUMCHARS | GTL_CLOSE | GTL_DEFAULT;
			int count = (int)SendMessage(hWndRich, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, 0);
			if (count == E_INVALIDARG)
				return L"";
			
			count +=1; //for null terminated

			CString ret;
			GETTEXTEX gt;
			gt.cb = count*2;
			gt.flags = dwFlags;
			gt.codepage = 1200;
			gt.lpDefaultChar = NULL;
			gt.lpUsedDefChar = NULL;
			SendMessage(hWndRich, EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)ret.GetBuffer(count));
			ret.ReleaseBuffer();
			return ret;
		}

		Public static LRESULT RichEdit_SetTextEx(HWND hWndRich, CString text, DWORD dwFlags=ST_DEFAULT)
		{
			ATLASSERT(hWndRich!=NULL);
			SETTEXTEX st;
			st.codepage = 1200;
			st.flags = dwFlags;
			return SendMessage(hWndRich, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)(LPCTSTR)text);
		}

		// replace text in RichEdit control
		// @param bCase Indicates if the search is case sensitive
		// @param bWord Indicates if the search should match whole words only, not parts of words.
		// @return number of text that replaced
		Public static int RichEdit_Replace(HWND hWndRich, LPCTSTR pszOld, LPCTSTR pszNew, bool bCase=true, bool bWord=true, bool bKeepUndo=false)
		{
			ATLASSERT( IsWindow(hWndRich)!=NULL ); //is not valid window
			ATLASSERT( _RICHEDIT_VER>=0x0200 );

			int ret = 0;
			FINDTEXTEXW ft;
			ft.chrg.cpMin = 0;
			ft.chrg.cpMax = -1;
			ft.lpstrText = pszOld;
			DWORD dwFlags = FR_DOWN;
			if (bCase) dwFlags |= FR_MATCHCASE;
			if (bWord) dwFlags |= FR_WHOLEWORD;
			UINT uMessage = EM_FINDTEXTEXW;

			while ( SendMessage(hWndRich, uMessage, (WPARAM)dwFlags, (LPARAM)&ft)!=-1 )
			{
				ret++;

				SendMessage(hWndRich, EM_EXSETSEL, 0, (LPARAM)&ft.chrgText);
				
				//update selection
				SETTEXTEX settext;
				settext.codepage = 1200;
				settext.flags = ST_SELECTION;
				if (bKeepUndo) settext.flags |= ST_KEEPUNDO;
				SendMessage(hWndRich, EM_SETTEXTEX, (WPARAM)&settext, (LPARAM)(LPCTSTR)pszNew);

				//continue search
				ft.chrg.cpMin = ft.chrgText.cpMin + (LONG)_tcslen(pszNew);
			}
			return ret;
		}

		// replace text in web document
		// @param flags see IHTMLTxtRange::findText
		// @return the number of item that replaced; return -1 if an error occured
		Public static int Html_Replace(IDispatchPtr pdocument, CStringW oldString, CStringW newString, long flags)
		{
			HRESULT hr;
			int ret = -1;

			//get document interface
			CComPtr<IHTMLDocument2> pdoc2;
			hr = pdocument->QueryInterface(IID_IHTMLDocument2, (void**)&pdoc2);
			if ( pdoc2!=NULL )
			{
				//get body interface
				CComPtr<IHTMLElement> pbody;
				hr = pdoc2->get_body(&pbody);
				if ( pbody!=NULL ) 
				{
					//get pbodyElement
					CComPtr<IHTMLBodyElement> pbodyElement;
					hr = pbody->QueryInterface(IID_IHTMLBodyElement, (void**)&pbodyElement);
					if ( pbodyElement!=NULL )
					{
						//get TextRange Interface
						CComPtr<IHTMLTxtRange> prange;
						hr = pbodyElement->createTextRange(&prange);
						if ( prange!=NULL )
						{
							//search and highligh keyword
							int find=0;
							while (true)
							{
								VARIANT_BOOL bSuccess= VARIANT_FALSE;
								prange->findText( CComBSTR(oldString )  , 1, flags, &bSuccess );
								if (!bSuccess)
									break;
								find++;

								prange->put_text( CComBSTR( newString.GetString() ) );
								prange->collapse(0);
							}
							ret = find;
						}
					}
				}
			}

			return ret;
		}

		Public static int Html_ReplaceInnerHtml(IDispatchPtr pdocument, CStringW oldString, CStringW newString)
		{
			HRESULT hr;
			int ret = -1;

			CComPtr<IHTMLDocument2> pdoc2;
			hr = pdocument->QueryInterface(IID_IHTMLDocument2, (void**)&pdoc2);
			if (SUCCEEDED(hr) && pdoc2!=NULL)
			{
				CComPtr<IHTMLElement> pbody;
				if (SUCCEEDED(pdoc2->get_body(&pbody)) && pbody!=NULL)
				{
					CComBSTR innerHtml;
					if (SUCCEEDED(pbody->get_innerHTML(&innerHtml)))
					{
						CStringW str(innerHtml);
						int result = str.Replace(oldString, newString);
						if (SUCCEEDED(pbody->put_innerHTML(str.AllocSysString())))
						{
							ret = result;
						}
					}
				}
			}
			return ret;
		}

		// @param flags see IHTMLTxtRange::findText
		// @param bSuccess can be NULL; return true if strFind founded and IHTMLTxtRange point to it; false if could not find text
		// @return pinter to IHTMLTxtRange with range strFind; caller must released this pointer; return NULL if could not get IHTMLTxtRange pointer
		Public static IHTMLTxtRange* Html_Find(LPDISPATCH pdocument, CStringW strFind, long Flags, VARIANT_BOOL* pbSuccess=NULL)
		{
			IHTMLTxtRange* ret = 0;

			//get document interface
			CComPtr<IHTMLDocument2> pdoc2;
			pdocument->QueryInterface(IID_IHTMLDocument2, (void**)&pdoc2);
			if ( pdoc2!=NULL )
			{
				//get body interface
				CComPtr<IHTMLElement> pbody;
				pdoc2->get_body(&pbody);
				if ( pbody!=NULL ) 
				{
					//get pbodyElement
					CComPtr<IHTMLBodyElement> pbodyElement;
					pbody->QueryInterface(IID_IHTMLBodyElement, (void**)&pbodyElement);
					if ( pbodyElement!=NULL )
					{
						//get TextRange Interface
						pbodyElement->createTextRange(&ret);
						if (ret!=NULL)
						{
							VARIANT_BOOL bSuccess=VARIANT_FALSE;
							ret->findText(CComBSTR(strFind.GetString() ), 1, Flags, (pbSuccess) ? pbSuccess : &bSuccess);
						}
					}
				}
			}

			return ret;
		}

		// see GetModuleFileName in PlatformSDK
		// @param if hModule is NULL; return the path and fileName of current process (exe file)
		Public static Path GetModuleFileName(HMODULE hModule = NULL)
		{
			CString ret;
			::GetModuleFileName(hModule, ret.GetBuffer(0x8000), 0x8000);
			ret.ReleaseBuffer();
			return Path(ret);
		}

		Public static Path GetModuleFileFolder(HMODULE hModule = NULL)
		{
			return GetModuleFileName(hModule).GetFolder();
		}

		// CheckBox and RadioButton helper function
		Public static void Button_SetCheck(HWND hWndOwner, UINT nId, int nCheck)
		{
			ATLASSERT( IsWindow( GetDlgItem(hWndOwner, nId) ) );
			SendDlgItemMessage(hWndOwner, nId, BM_SETCHECK, nCheck, 0);
		}

		Public static int Button_GetCheck(HWND hWndOwner, UINT nId)
		{
			ATLASSERT( IsWindow( GetDlgItem(hWndOwner, nId) ) );
			return (int)SendDlgItemMessage(hWndOwner, nId, BM_GETCHECK, 0, 0);
		}
		
		// @param nStart minimum prime number; the returned prime number must equal or greater of this value
		// @return a prime number; return nStart if nStart is prime number
		Public static int GetPrimeNumber(int nStart)
		{
			if (nStart<3) return 2;
			int i = (nStart%2) ? nStart: nStart+1;
			while(true)
			{
				int sqr = (int)sqrt((double)i);
				bool bIsPrime = true;
				
				//check if 'i' is prime number
				for (int j=3; j<=sqr; j++)
					if ((i%j)==0) 
					{
						bIsPrime = false;
						break;
					}
			   	
				if (bIsPrime)
					return i;// found prime number
				i+=2;
			}
		}

		#ifdef _CRT_RAND_S
		// _CRT_RAND_S must defined before #include <stdlib.h>
		// @return a random integer beetween nStart and nEnd; return value maybe euqals to nStart or nEnd
		Public static UINT GetRandomNumber(UINT nStart, UINT nEnd)
		{
			nEnd++;

			UINT randomt;
			if (rand_s(&randomt)!=0)
				AtlThrow(E_FAIL);

			double random = (double)randomt;
			double max_random = (double)UINT_MAX;
			UINT ret = (UINT)((random/max_random)*(nEnd-nStart));
			return ret + nStart;
		}
		#endif
		
		// load entire file to CStringW object
		// this method use 2 first of file to determine UNICODE or ANSI or UTF8
		// @param CodePage if AutoDetect CP_WINANSI; CP_WINUNICODE; CP_UTF8 failed then try to use this codepage
		// @return CodePage that used to read file
		// @throw error if an error occured
		Public static UINT String_LoadFromFile(CStringW& str, Path fileName, UINT CodePage=CP_ACP)
		{
			HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
			TESTLASTERR(hFile!=INVALID_HANDLE_VALUE);

			UINT dwFileSize = (UINT)GetFileSize(hFile, NULL);;
			LPBYTE buf = new BYTE[dwFileSize+3];
			buf[dwFileSize] = buf[dwFileSize+1] = buf[dwFileSize+2] = 0;
			DWORD dwNumberOfBytesRead;
			if (!ReadFile(hFile, buf, dwFileSize, &dwNumberOfBytesRead, NULL))
			{
				delete buf;
				AtlThrowLastWin32();
			}
				
			//find codepage
			LPBYTE pStartBuf = buf;
			if ( buf[0]==0xFF && buf[1]==0xFE ) //unicode
			{
				pStartBuf +=2 ;
				CodePage = CP_WINUNICODE;
			}
			else if ( buf[0]==0xEF && buf[1]==0xBB && buf[2]==0xBF ) //utf8
			{
				pStartBuf +=3 ;
				CodePage = CP_UTF8;
			}

			//read buffer
			str = (CodePage==CP_WINUNICODE) 
					? (LPCWSTR)pStartBuf
					: CA2W((LPCSTR)pStartBuf, CodePage);

			//cleanup
			delete buf;
			TESTLASTERR( CloseHandle(hFile) );
 			return CodePage;
		}

		Public static StringListPtr String_Split(CString str, CString separator)
		{
			StringListPtr ret = new StringList();
			int curPos = 0;
			CString resToken= str.Tokenize(separator, curPos);
			while (resToken != L"")
			{
				ret->AddTail(resToken);
				resToken = str.Tokenize(separator, curPos);
			}

			return ret;
		}

		// @throw error if an error occured
		Public static void String_SaveToFile(CString str, Path fileName, UINT CodePage=CP_ACP, bool bWriteUnicodeIndicator=true)
		{
			HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, 0);
			TESTLASTERR(hFile!=INVALID_HANDLE_VALUE);

			//write unicode indicator
			try
			{
				DWORD dwValue=0;
				if (bWriteUnicodeIndicator && CodePage==CP_WINUNICODE)
					TESTLASTERR( WriteFile(hFile, "\xFF\xFE", 2, &dwValue, 0) );


				if (bWriteUnicodeIndicator && CodePage==CP_UTF8)
					TESTLASTERR( WriteFile(hFile, "\xEF\xBB\xBF", 3, &dwValue, 0) );
			    
				//write string
				if (CodePage==CP_WINUNICODE)
				{
					TESTLASTERR( WriteFile(hFile, (LPCTSTR)str, str.GetLength()*2, &dwValue, 0) );
				}
				else
				{
					CStringA strA = CW2A(str, CodePage);
					TESTLASTERR( WriteFile(hFile, (LPCSTR)strA, strA.GetLength(), &dwValue, 0) );
				}

				//close handle
				TESTLASTERR( CloseHandle(hFile) );
			}
			catch(...)
			{
				TESTLASTERR( CloseHandle(hFile) );
				throw;
			}
		}
		

		// Show choose folder dialog box to user choose a folder
		// @param folderName [in/out] set initalize folder of dilaog and return folder that user selected (without backslash)
		// @param rootFolder if empty string set "My Computer" as root
		// @return true if user click ok and folderName return new selected folder;
		// @see BROWSEINFO in SDK
		Public static bool BrowseForFolder(Path& folderName, CString title = L"", HWND hWndonwer = NULL, UINT ulFlags = BIF_USENEWUI, Path rootFolder=Path(L"*desktop*"))
		{
			bool ret = false;

			TCHAR pszDisplayName[MAX_PATH]={0};
			Path initPath(folderName);
			if ( !folderName.IsDirectory())
				initPath = Path();

			BROWSEINFO bi;
			bi.hwndOwner		= hWndonwer;
			bi.pidlRoot			= (rootFolder==Path(L"*desktop*") || !rootFolder.FileExists()) ? NULL : ItemIdList_FromPath(rootFolder);
			bi.pszDisplayName	= pszDisplayName;
			bi.lpszTitle		= title;
			bi.ulFlags			= BIF_RETURNONLYFSDIRS | ulFlags;
			bi.lpfn				= BrowseCallbackProc;
			bi.lParam			= (LPARAM)(LPCTSTR)initPath;

			LPITEMIDLIST itemList = SHBrowseForFolder(&bi);

			//get system allocator
			IMallocPtr pmalloc;
			HRESULT hr = SHGetMalloc( &pmalloc );
			if ( FAILED(hr) )
			{
				ATLASSERT ( false );
				return false;
			}
			
			//release root pidl
			if ( bi.pidlRoot )
				pmalloc->Free((void*)bi.pidlRoot);

			//release selected pidl
			if (itemList)
			{
				TCHAR folder[MAX_PATH];
				SHGetPathFromIDList(itemList, folder);
				folderName = Path(folder);
				pmalloc->Free(itemList);
				ret =  true;
			}

			return ret;
		}

		// @return volume label of lpszDrive
		//If this parameter is NULL, the root of the current directory is used. A trailing backslash is required. For example, you would specify \\MyServer\MyShare as \\MyServer\MyShare\, or the C drive as "C:\".
		Public static CString GetDriveLable(CString rootPathName)
		{
			TCHAR volume[100];
			GetVolumeInformation(rootPathName, volume, sizeof volume, NULL, NULL, NULL, NULL,0);
			return volume;
		}

		
		// create process and execute it
		// @param pszWorkDirectory pointer to Workdirectory for child process; if NULL, the new process will have the same current drive and directory as the calling process
		// @pdwExitCode if not NULL proccss wait until child process terminated and set this pointer to return value of child process
		// @param dwPumpTime RunFile pump message after dwPumpTime (in milisecond) then wait again; if set this parameter to INFINITE, RunFile never pump message and wait until process terminated
		// @param lpProcessInformation if exist will filled. can be NULL
		// @return FALSE if failed
		Public static void RunFile(Path exeFile, CString commandLine, int nShow, LPCTSTR lpszWorkDirectory, LPDWORD lpExitCode, DWORD dwPumpTime, LPPROCESS_INFORMATION lpProcessInformation)
		{
			CString cmd = commandLine;
			if ( !exeFile.IsEmpty() )
				cmd.Format(L"\"%s\" %s", (LPCTSTR)exeFile, (LPCTSTR)commandLine);
			
			STARTUPINFO st;
			memset(&st,0,sizeof st);
			st.cb=sizeof STARTUPINFO;
			st.dwFlags = STARTF_USESHOWWINDOW;
			st.wShowWindow = (WORD) nShow;

			//add backslash to working directory if not exist
			Path workDir(lpszWorkDirectory);
			if (!workDir.IsEmpty())
			{
				workDir.AddBackslash();
				lpszWorkDirectory = workDir;
			}

			PROCESS_INFORMATION processInf = {0};
			TESTLASTERR(CreateProcess(NULL, cmd.GetBuffer(), NULL, NULL, FALSE,
						NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT | CREATE_PRESERVE_CODE_AUTHZ_LEVEL, NULL, lpszWorkDirectory, 
						&st, &processInf));
			cmd.ReleaseBuffer();


			if (lpProcessInformation!=NULL)
				memcpy(lpProcessInformation, &processInf, sizeof PROCESS_INFORMATION);

			if (lpExitCode!=NULL)
			{
				while(TRUE)
				{
					WaitForSingleObject(processInf.hProcess, dwPumpTime);
					GetExitCodeProcess(processInf.hProcess, lpExitCode);
					
					if ( *lpExitCode!=STILL_ACTIVE )
						break;
				
					//pump message////////////////////
					MSG msg;
					while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);  
					}
					//////////////////////////////////
				}
			}

			//close handles
			if (lpProcessInformation==NULL)
			{
				if (processInf.hProcess!=NULL) CloseHandle(processInf.hProcess);
				if (processInf.hThread!=NULL) CloseHandle(processInf.hThread);
			}
		}

		Public static void RunFile(Path exeFile, CString commandLine = L"", int nShow = SW_SHOWDEFAULT, LPCTSTR lpszWorkDirectory = NULL, LPDWORD lpExitCode = NULL, DWORD dwPumpTime = INFINITE)
		{
			RunFile(exeFile, commandLine, nShow, lpszWorkDirectory, lpExitCode, dwPumpTime, NULL);
		}

		// @param lphProcess return handle to opened process; if not NULL user must close handle after use it
		Public static void ShellExecute(Path fileName, CString commandLine=L"", int nShow=SW_SHOWNORMAL, LPCTSTR lpszWorkDirectory = NULL, LPHANDLE lphProcess=NULL, LPCTSTR lpVerb=NULL, LPDWORD lpExitCode = NULL, DWORD dwPumpTime = INFINITE, HWND hWnd=NULL)
		{
			SHELLEXECUTEINFO s;
			memset (&s,0,sizeof s);

			s.cbSize=sizeof SHELLEXECUTEINFO;
			s.fMask= 0;
			if (lphProcess!=NULL || lpExitCode!=NULL) s.fMask |= SEE_MASK_NOCLOSEPROCESS;
			s.hwnd=hWnd;
			s.lpVerb=lpVerb;
			s.lpFile = fileName;
			s.lpParameters = commandLine;
			s.lpDirectory = lpszWorkDirectory;
			s.nShow=nShow;

			TESTLASTERR( ShellExecuteEx(&s) );
			if (lphProcess)
				*lphProcess = s.hProcess;

			if (lpExitCode!=NULL)
			{
				while(TRUE)
				{
					WaitForSingleObject(s.hProcess, dwPumpTime);
					GetExitCodeProcess(s.hProcess, lpExitCode);
					
					if ( *lpExitCode!=STILL_ACTIVE )
						break;
				
					//pump message
					MSG msg;
					while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);  
					}
				}
			}

			//close process handle
			if (lphProcess==NULL && s.hProcess!=NULL)
				CloseHandle(s.hProcess);
		}

		// @return current BitPerPixel of windows(Desktop window)
		Public static int GetSystemBitCount()
		{
			HWND hWnd = GetDesktopWindow();
			HDC hdc = GetDC(hWnd);
			int ret = GetDeviceCaps(hdc, BITSPIXEL);
			ReleaseDC(hWnd, hdc);
			return ret;
		}

		//@see DrawText for uFormat
		Public static BOOL DC_DrawGrayString(HDC hdc, CString str, CRect rcRect, UINT uFormat)
		{
			COLORREF clrOld;
			int nPrvMode;
			BOOL ret = true;
			
			//Set light shadow color and draw it 
			clrOld = SetTextColor(hdc, GetSysColor(COLOR_3DHILIGHT));
			ret &= DrawText(hdc, str, str.GetLength(), rcRect, uFormat);

			// Draw gray face
			nPrvMode = SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc,GetSysColor(COLOR_GRAYTEXT));
			rcRect.OffsetRect(-1,-1);
			ret &= DrawText(hdc, str, str.GetLength(), rcRect, uFormat);
			
			//Restore previous mode
			SetTextColor(hdc, clrOld);	
			nPrvMode = SetBkMode(hdc, nPrvMode);

			return ret;
		}

		Public static void DC_FloodBlt(HDC hdc, int x, int y, int width, int height, HBITMAP hBmp, int xSrc, int ySrc, COLORREF rgbInColor)
		{
			ATLASSERT( hdc!=NULL );
			ATLASSERT( hBmp!=NULL );

			HDC memDC = CreateCompatibleDC(hdc);
			HBITMAP oldMemBitmap = (HBITMAP)SelectObject(memDC, hBmp);
			DC_FloodBlt(hdc, x, y, width, height, memDC, xSrc, ySrc, rgbInColor);

			//clean up
			SelectObject(memDC, oldMemBitmap);
			DeleteDC(memDC);
		}

		Public static void DC_FloodBlt(HDC hdc, int x, int y, int width, int height, HDC hSrcDC, int xSrc, int ySrc, COLORREF rgbInColor)
		{
			ATLASSERT(hdc!=NULL);
			ATLASSERT(hSrcDC!=NULL);
			
			COLORREF rgbWhite = RGB(255,255,255);
			COLORREF rgbBlack = 0;

			//prepare object
			HBITMAP hMaskBitmap = CreateBitmap(width, height, 1, 1, NULL);
			HBITMAP hMemBitmap = CreateCompatibleBitmap(hdc, width, height);
			HDC memDC = CreateCompatibleDC(hdc);
			HDC hMaskDC = CreateCompatibleDC(hdc);
			HBITMAP oldMemBitmap = (HBITMAP)SelectObject(memDC, hMemBitmap);
			HBITMAP hOldMaskBitmap = (HBITMAP)SelectObject(hMaskDC, hMaskBitmap);
			
			//copy srcDC to memDC.because srcDC should not change
			BitBlt(memDC, 0, 0, width, height, hSrcDC, xSrc, ySrc, SRCCOPY);

			//real work
			//create Mask color.
			//Black points indicate that bitmap can draw in it
			//White points indicate that bitmap cannot draw in it
			COLORREF oldBkColor = SetBkColor(hdc, rgbInColor);
			BitBlt(hMaskDC, 0, 0, width, height, hdc, x, y, NOTSRCCOPY);

			SetBkColor(memDC, rgbBlack );
			SetTextColor(memDC, rgbWhite );
			BitBlt(memDC, 0, 0, width, height, hMaskDC, 0, 0, SRCAND);

			SetBkColor(hdc, rgbWhite );
			COLORREF oldTextColor = SetTextColor(hdc, rgbBlack );

			//{fastDC use to decrease 1 BitBlt from hdc
			HDC hFastDC = CreateCompatibleDC(hdc);
			HBITMAP hFastBitmap = CreateCompatibleBitmap(hdc, width, height);
			HBITMAP hOldFastBitmap = (HBITMAP)SelectObject(hFastDC, hFastBitmap);
			
			BitBlt(hFastDC, 0, 0, width, height, hdc, x, y, SRCCOPY);
			BitBlt(hFastDC, 0, 0, width, height, hMaskDC, 0, 0, SRCAND);
			BitBlt(hFastDC,0, 0, width, height, memDC, 0, 0, SRCPAINT);
			
			BitBlt(hdc, x, y, width, height, hFastDC, 0, 0, SRCCOPY);
			//}
		 	
			//fastDC block can replace with follow/////
			//hdc->BitBlt(x, y, width, height, &maskDC, 0, 0, SRCAND);
			//hdc->BitBlt(x, y, width, height, &memDC, 0, 0, SRCPAINT);
			///////////////////////////////////////////
			
			
			//Clean up
			//Undo work for bmp
			//if this section not reached and memDC do work on bmp, bmp would be changed(i don't know why?.I see it in MSDN)
			SetBkColor(memDC, rgbInColor);
			SetTextColor(memDC, rgbBlack);
			BitBlt(memDC, 0, 0, width, height, hMaskDC, 0, 0, SRCPAINT);

			SelectObject(memDC, oldMemBitmap);
			DeleteObject(hMemBitmap); //comaptible
			DeleteDC(memDC);

			SelectObject(hMaskDC, hOldMaskBitmap);
			DeleteObject(hMaskBitmap);
			DeleteDC(hMaskDC);

			SelectObject(hFastDC, hOldFastBitmap);
			DeleteObject(hFastBitmap);
			DeleteDC(hFastDC);

			SetBkColor(hdc, oldBkColor);
			SetTextColor(hdc, oldTextColor);
		}

		Public static void DC_FillSolidRect(HDC hdc, CRect rect, COLORREF clr )
		{
			COLORREF clrOldColor = SetBkColor(hdc, clr);
			ExtTextOut(hdc, 0, 0, ETO_OPAQUE, rect, NULL, 0, NULL);
			SetBkColor(hdc, clrOldColor);
		}
		

		Public static CString LoadString(UINT nID)
		{
			CString string;
			ATLVERIFY(string.LoadString(nID));
			return string;
		}

		Public static int String_ReplaceNoCase(CString &string, LPCTSTR lpszOld, LPCTSTR lpszNew)
		{
			int ret = 0;
			
			int nOldLen = (int)_tcslen(lpszOld);
			//int nNewLen = (int)_tcslen(lpszNew);
			int nLength = (int)string.GetLength();
			if (nOldLen==0)
				return 0;

			CString newString;
			for (int i=0; i<nLength; i++)
			{
				if ( i+nOldLen<=nLength && string.Mid(i,nOldLen).CompareNoCase(lpszOld)==0 )
				{
					newString += lpszNew;
					i+=nOldLen-1;;
					ret+=nOldLen;
				}
				else
				{
					newString += string.GetAt(i);
				}
			}

			if (ret!=0)
				string = newString;

			return ret;
		}

		Public static CRect GetWorkArea()
		{
			CRect rc;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
			return rc;
		}

		Public static int List_GetCurSelection(HWND hWndList)
		{
			return ListView_GetNextItem(hWndList, -1, LVNI_SELECTED);
		}

		Public static void ComboBox_SetLayout(HWND comboBoxHandle, bool rtl)
		{
			DWORD styleAdd = rtl ? WS_EX_LAYOUTRTL : 0;
			DWORD styleRemove = rtl ? 0 : WS_EX_LAYOUTRTL;
			COMBOBOXINFO cbinfo = {0};
			cbinfo.cbSize = sizeof COMBOBOXINFO;
			TESTLASTERR( GetComboBoxInfo(comboBoxHandle, &cbinfo) );
			CWindow(cbinfo.hwndCombo).ModifyStyleEx(styleRemove, styleAdd);
			CWindow(cbinfo.hwndItem).ModifyStyleEx(styleRemove, styleAdd);
			CWindow(cbinfo.hwndList).ModifyStyleEx(styleRemove, styleAdd);
		}

		Public static void ComboBox_InitItems(HWND comboBoxHandle, CString items, CString tokens=L";")
		{
			int curPos= 0;
			items.Replace(L'\n', L';');
			CString resToken = items.Tokenize(tokens, curPos);
			while (resToken != L"")
			{
				SendMessage(comboBoxHandle, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)resToken);
				resToken = items.Tokenize(tokens, curPos);
			}
		}

		Public static void List_SetCurSelection(HWND hWndList, int itemIndex)
		{
			ListView_SetItemState(hWndList, itemIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			ListView_SetSelectionMark(hWndList, itemIndex);
		}

		// this method automatically add another null terminated string to pFrom and pTo
		// @see SHFileOperation API
		// @return 0 if successful; 1 if failed; 2 if any operation aborted
		Public static int Shell_FileOperation(UINT wFunc, LPCTSTR pFrom, LPCTSTR pTo, FILEOP_FLAGS fFlags, HWND hwnd=NULL, LPCTSTR lpszProgressTitle=NULL)
		{
			TCHAR* pfrom = NULL; 
			TCHAR* pto = NULL;
			
			if (pFrom)
			{
				int cbfrom = (int)_tcslen(pFrom) + 2;
				pfrom = new TCHAR[ cbfrom ];
				memset(pfrom, 0, cbfrom * sizeof (TCHAR));
				StrCpy(pfrom, pFrom);
			}
			if (pTo)
			{
				int cbto = (int)_tcslen(pTo) + 2;
				pto = new TCHAR[ cbto ];
				memset(pto, 0, cbto * sizeof (TCHAR));
				StrCpy(pto, pTo);
			}

			SHFILEOPSTRUCT shf={0};
			shf.hwnd = hwnd;
			shf.wFunc = wFunc;
			shf.pFrom = pfrom;
			shf.pTo = pto;
			shf.fFlags = fFlags;
			shf.lpszProgressTitle = lpszProgressTitle;
			int res = SHFileOperation(&shf);
			
			delete []pto;
			delete []pfrom;

			if ( res!=0 ) return 1;
			if (shf.fAnyOperationsAborted) return 2;
			return 0;
		}

		// @see SHGetSpecialFolderPath
		// @return empty if could not get
		Public static Path Shell_GetSpecialFolderPath(HWND hwndOwner , int nFolder , BOOL fCreate)
		{
			CString ret;
			BOOL res = SHGetSpecialFolderPath(hwndOwner, ret.GetBuffer(0x8000), nFolder, fCreate);
			ret.ReleaseBuffer();
			return (res!=FALSE) ? Path(ret) : Path();
		}

		// @param fileName can be file or folder
		Public static void Shell_OpenFolderAndSelectItem(Path fileName)
		{
			LPCITEMIDLIST pidlFolder = ItemIdList_FromPath(fileName);
			CoInitialize(0);
			HRESULT hr = SHOpenFolderAndSelectItems(pidlFolder, 0, NULL, 0);
			CoUninitialize();
			TESTHR(hr);
		}
		
		// failed ATLASSERT then return empty string
		// @see IShellLink::GetPath
		Public static Path ShellLink_GetPath(IShellLinkPtr pshellLink)
		{
			TCHAR ret[MAX_PATH*2];
			WIN32_FIND_DATA find_data;
			HRESULT hres = pshellLink->GetPath(ret, MAX_PATH*2, &find_data, SLGP_UNCPRIORITY);
			if (FAILED(hres))
			{
				ATLASSERT(false);
				return Path();
			}

			return Path(ret);
		}

		Public static Path ShellLink_GetPath(Path linkFile)
		{
			//create shelllink objecr
			IShellLinkPtr pshell = ShellLink_CreateObject(linkFile);
			Path ret = ShellLink_GetPath(pshell);
			return ret;
		}
		
		// Create an IShellLink object and get a pointer to the	IShellLink 
		// @return pointer to object of IShellLink if success; NULL if faile
		// caller must release this com object after use it; CoInitialize must called before
		Public static IShellLinkPtr ShellLink_CreateObject()
		{
			// interface (returned from CoCreateInstance).
			IShellLinkPtr psl;
			TESTHR( CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl) );
			return psl;
		}
		
		// Create an IShellLink object and get a pointer to the	IShellLink then open fileName
		// @return if success pointer to object of IShellLink that contatin fileName info; NULL if could not open fileName or create IShellLink object 
		// caller must release this com object after use it; CoInitialize must called before
		Public static IShellLinkPtr ShellLink_CreateObject(Path fileName)
		{
			IShellLinkPtr pshellLink = ShellLink_CreateObject();
			// Query IShellLink for the IPersistFile interface for 
			// saving the shortcut in persistent storage.
			IPersistFilePtr ppersistFile;
			TESTHR( pshellLink->QueryInterface(IID_IPersistFile, (void**)&ppersistFile) );
			//load link file
			TESTHR( ppersistFile->Load(fileName, TRUE) );
			return pshellLink;
		}

		Public static void ShellLink_Save(IShellLinkPtr pshellLink, Path fileName)
		{
			IPersistFilePtr ppersistFile;

			// Query IShellLink for the IPersistFile interface for 
			// saving the shortcut in persistent storage.
			TESTHR( pshellLink->QueryInterface (IID_IPersistFile, (void**)&ppersistFile) );
			TESTHR( ppersistFile->Save(fileName, TRUE) );
		}

		// if pathName is empty string reuturn pidl of my computer
		Public static LPITEMIDLIST ItemIdList_FromPath(Path pathName)
		{
			LPITEMIDLIST ret = NULL;
			// Get the Desktop's shell folder interface
			IShellFolderPtr psfDesktop;
			TESTHR( SHGetDesktopFolder(&psfDesktop) );

			// Request an ID list (relative to the desktop) for the short pathname
			ULONG chEaten = 0;
			TESTHR( psfDesktop->ParseDisplayName(NULL, NULL, (LPTSTR)(LPCTSTR)pathName, &chEaten, &ret, NULL) );
			return ret;
		}

		Public static Path ItemIdList_ToPath(LPCITEMIDLIST pidl)
		{
			TCHAR pbuf[MAX_PATH];
			if (SHGetPathFromIDList(pidl, pbuf))
			{
				return Path(pbuf);
			}
			return Path();
		}
		
		Public static GUID GUID_Create()
		{
			GUID guid = {0};
			TESTHR( CoCreateGuid( &guid ) );
			return guid;
		}

		Public static CString GUID_CreateString()
		{
			GUID guid = GUID_Create();
			return Convert::ToString(guid);
		}

		// @see PlayEnhMetaFile
		// @param playMode one of the PlayMetaFileMode value
		Public static void MetaPlay(HDC hdc, HENHMETAFILE hemf, CRect rcRect, PlayMetaFileMode playMode=pmfNormal)
		{
			CRect rcNew = rcRect;
			
 			XFORM xForm;
			bool usexForm = false;

			switch(playMode){
			case pmfFlipH: 
				rcNew.SwapLeftRight();
				break;

			case pmfFlipV:
				rcNew.top = rcRect.bottom;
				rcNew.bottom = rcRect.top;
				break;

			case pmfRotate180:
			case pmfFlipHV:
				rcNew.SwapLeftRight();
				rcNew.top = rcRect.bottom;
				rcNew.bottom = rcRect.top;
				break;
				
			case pmfRotate90:
				rcNew.right = rcRect.left + rcRect.Height();
				rcNew.bottom = rcRect.top + rcRect.Width();
				xForm.eM11 = 0;
				xForm.eM12 = +1;
				xForm.eM21 = -1;
				xForm.eM22 = 0;
				xForm.eDx = (float)rcNew.bottom + rcNew.left;
				xForm.eDy = (float)rcNew.top - rcNew.left;
				usexForm = true;
				break;

			case pmfRotate270:
				ATLASSERT(false); //not implemented at yet
				usexForm = true;
				break;
			}

			//use SetWorldTransform if xForm is set
			if ( usexForm )
			{
				XFORM oldxForm;
				int oldGraphicMode = SetGraphicsMode(hdc, GM_ADVANCED);
				TESTLASTERR( GetWorldTransform(hdc, &oldxForm) );
				TESTLASTERR( SetWorldTransform(hdc, &xForm) );
				TESTLASTERR( PlayEnhMetaFile(hdc, hemf, rcNew) );
				TESTLASTERR( SetWorldTransform(hdc, &oldxForm) );
				TESTLASTERR( SetGraphicsMode(hdc, oldGraphicMode) );
			}
			else
			{
				TESTLASTERR( PlayEnhMetaFile(hdc, hemf, rcNew) );
			}
		}

		Public static void MetaPlay(HDC hdc, HENHMETAFILE hemf, CPoint ptTopLeft, PlayMetaFileMode playMode=pmfNormal)
		{
			CSize size = MetaGetSize(hemf);
			TESTLASTERR( DPtoLP(hdc, (LPPOINT)&size, 1) );
			return MetaPlay(hdc, hemf, CRect(ptTopLeft, size), playMode);
		}

		Public static CSize MetaGetSize(HENHMETAFILE hemf)
		{
			CSize ret(0,0);
			ENHMETAHEADER emfheader={0};
			if (GetEnhMetaFileHeader(hemf, sizeof ENHMETAHEADER, &emfheader)!=0)
				ret = CSize(emfheader.rclBounds.right-emfheader.rclBounds.left, emfheader.rclBounds.bottom-emfheader.rclBounds.top);
			return ret;
		}

		Public static void QuickBox(int var)
		{
			//_itot(var, d, 10);
			CString str;
			str.Format(L"%d \t(Dec)\n%x \t(Hex)", var, var);
			MsgBox(str, L"Quick info value", 0);
		}

		__if_exists(CWinApp){
		Public static void MFC_SetRegistryKey(CString regKey)
		{
			ATLASSERT( regKey.Left(9).CompareNoCase(L"Software\\")==0 );
			regKey.Delete(0, 9);

			//{CWinApp::SetRegistryKey body
			free((void*)AfxGetApp()->m_pszRegistryKey);
			AfxGetApp()->m_pszRegistryKey = _tcsdup( regKey );
			free((void*)AfxGetApp()->m_pszProfileName);
			AfxGetApp()->m_pszProfileName = _tcsdup( L"" );
			//}{CWinApp::SetRegistryKey body
		}}//_if__exists

		__if_exists(CWinApp){
		Public static CString MFC_GetRegistryKey()
		{
			//make sure App_SetRegistryKey was called before
			CString strProfileName = AfxGetApp()->m_pszProfileName;
			ATLASSERT( strProfileName.IsEmpty() ); //App_SetRegistryKey must empty; make sure App_SetRegistryKey was called before
			return L"Software\\" + CString(AfxGetApp()->m_pszRegistryKey);
		}}//_if__exists

		__if_exists(CWinApp){
		Public static void MFC_SetAppName(CString appName)
		{
			free((void*)AfxGetApp()->m_pszAppName);
			AfxGetApp()->m_pszAppName = _tcsdup( appName );
			AfxGetModuleState()->m_lpszCurrentAppName = AfxGetApp()->m_pszAppName;
		}}//_if__exists

		Public static CString UrlUnescape(CString string)
		{
			DWORD length = string.GetLength() * 10;
			CString ret;
			::UrlUnescape((LPTSTR)(LPCTSTR)string, ret.GetBuffer(length), &length, URL_DONT_UNESCAPE_EXTRA_INFO);
			ret.ReleaseBuffer();
			return ret;
		}

		Public static CString UrlEscape(CString string)
		{
			DWORD length = string.GetLength() * 10;
			CString ret;
			::UrlEscape(string, ret.GetBuffer(length), &length, URL_DONT_UNESCAPE_EXTRA_INFO);
			ret.ReleaseBuffer();
			return ret;
		}

		Public static CString XmlEscape(CString string)
		{
			int length = ATL::EscapeXML(string, string.GetLength(), 0, 0, ATL_ESC_FLAG_ATTR);
			WCHAR* chr = new WCHAR[length+1];
			ZeroMemory(chr, (length+1) * sizeof WCHAR);
			ATL::EscapeXML(string, string.GetLength(), chr, length, ATL_ESC_FLAG_ATTR);
			CString ret = chr;
			delete chr;
			return ret;
		}

		Public static CString HexString_FromBuffer(const void* pbuffer, int count)
		{
			ByteArrayPtr pbuf = new ByteArray(pbuffer, count);
			return pbuf->ToHexString();
		}

		Public static ByteArrayPtr HexString_ToBuffer(CString hexString)
		{
			ByteArrayPtr pbuf = new ByteArray();
			pbuf->FromHexString(hexString);
			return pbuf;
		}

		Public static CString Base64String_FromBuffer(const void* pbuffer, int count)
		{
			ByteArrayPtr pbuf = new ByteArray(pbuffer, count);
			return pbuf->ToBase64String();
		}

		Public static ByteArrayPtr Base64String_ToBuffer(CString base64String)
		{
			ByteArrayPtr pbuf = new ByteArray();
			pbuf->FromBase64String(base64String);
			return pbuf;
		}

		Public static CString GetUserName()
		{
			CString text;
			DWORD textSize = 500;
			::GetUserName(text.GetBuffer(textSize), &textSize);
			text.ReleaseBuffer();
			return text;
		}
		
		Private static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
		{
			switch(uMsg) {
			case BFFM_INITIALIZED: 
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData);
				break;
			}
			return 0;
		}

		__if_exists(Mad::FontPtr){
		// Create font from resource string;
		// @param strFontFormat resource string that must has a following format or LOGFONT member without first lf letter
		//	 "Name=; Size=; Bold=; Italic=; Underline=; StrikeOut=; Weight="
		//   that size must in point: eg:12 = 12-point font
		Public static Mad::FontPtr Font_Create(CString fontFormat, HDC hdc=NULL)
		{
			//convert string parameter to LOGFONT
			LOGFONT lf = LogFont_FromString(fontFormat, hdc);
			Mad::FontPtr font = new Mad::Font();
			TESTLASTERR( font->CreateFontIndirect(&lf) );
			return font;
		}}

		Public static LOGFONT LogFont_FromString(CString value, HDC hdc=NULL)
		{
			LOGFONT lf = {0};
			lf.lfCharSet = DEFAULT_CHARSET;
			lf.lfQuality = PROOF_QUALITY;
			if (Param::Get(L"Bold", 0, value))
				lf.lfWeight = 700;
			CString strFontName = Param::Get(L"FaceName", L"", value);
			if ( strFontName.IsEmpty() ) strFontName = Param::Get(L"Name", L"", value);
			lf.lfHeight = Param::Get(L"Size", 10, value)*10;

			//{calculate width by point size (MFC)
			bool bGetDC = hdc == NULL;
			if (hdc == NULL)
				hdc = GetDC(NULL);

			// convert nPointSize to logical units based on pDC
			POINT pt = { 0, 0 };
			pt.y = ::MulDiv(::GetDeviceCaps(hdc, LOGPIXELSY), lf.lfHeight, 720);   // 72 points/inch, 10 decipoints/point
			TESTLASTERR( DPtoLP(hdc, &pt, 1) ); 
			POINT ptOrg = { 0, 0 };
			TESTLASTERR( DPtoLP(hdc, &ptOrg, 1) );
			lf.lfHeight = -abs(pt.y - ptOrg.y);
			if (bGetDC)
				ReleaseDC(NULL, hdc);
			//}calculate width by point size (WFC)


			//convert string parameter to LOGFONT
			lf.lfHeight = Param::Get(L"Height", lf.lfHeight, value);
			lf.lfWidth = Param::Get(L"Width", lf.lfWidth, value);
			lf.lfEscapement = Param::Get(L"Escapement", lf.lfEscapement, value);
			lf.lfOrientation = Param::Get(L"Orientation", lf.lfOrientation, value);
			lf.lfWeight = Param::Get(L"Weight", lf.lfWeight, value);
			lf.lfItalic = (BYTE) Param::Get(L"Italic", lf.lfItalic, value);
			lf.lfUnderline = (BYTE) Param::Get(L"Underline", lf.lfUnderline, value);
			lf.lfStrikeOut = (BYTE) Param::Get(L"StrikeOut", lf.lfStrikeOut, value);
			lf.lfCharSet = (BYTE) Param::Get(L"CharSet", lf.lfCharSet, value);
			lf.lfOutPrecision = (BYTE) Param::Get(L"OutPrecision", lf.lfOutPrecision, value);
			lf.lfClipPrecision = (BYTE) Param::Get(L"ClipPrecision", lf.lfClipPrecision, value);
			lf.lfQuality = (BYTE) Param::Get(L"Quality", lf.lfQuality, value);
			lf.lfPitchAndFamily = (BYTE)Param::Get(L"PitchAndFamily", lf.lfPitchAndFamily, value);
			StrCpy(lf.lfFaceName, strFontName);
			return lf;
		}

		Public static CString LogFont_ToString(LOGFONT* logFont)
		{
			ATLASSERT( logFont!=NULL );

			CString ret;
			Param::Set(L"Height", logFont->lfHeight, ret);
			Param::Set(L"Width", logFont->lfWidth, ret);
			Param::Set(L"Escapement", logFont->lfEscapement, ret);
			Param::Set(L"Orientation ", logFont->lfOrientation, ret);
			Param::Set(L"Weight", logFont->lfWeight, ret);
			Param::Set(L"Italic", logFont->lfItalic, ret);
			Param::Set(L"Underline", logFont->lfUnderline, ret);
			Param::Set(L"StrikeOut", logFont->lfStrikeOut, ret);
			Param::Set(L"CharSet", logFont->lfCharSet, ret);
			Param::Set(L"OutPrecision", logFont->lfOutPrecision, ret);
			Param::Set(L"ClipPrecision", logFont->lfClipPrecision, ret);
			Param::Set(L"Quality", logFont->lfQuality, ret);
			Param::Set(L"PitchAndFamily", logFont->lfPitchAndFamily, ret);
			Param::Set(L"FaceName", logFont->lfFaceName, ret);

			if ( logFont->lfWeight>=700 )
				Param::Set(L"Bold", 1, ret);

			return ret;
		}
	};
}//namespace mad

using namespace Mad; //for easy use it always included here
