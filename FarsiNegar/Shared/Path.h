#pragma once
#include "Base.h"

namespace Mad
{
	//Path class
	class Path : public CPath
	{
	public:
		Path()
		{
		}

		Path(const Path& path)
			: CPath(path)
		{
		}

		explicit Path(const CPath& path)
			: CPath(path)
		{
		}

		explicit Path(LPCTSTR path)
			: CPath(path)
		{
		}

		CString& ToString()
		{
			return m_strPath;
		}

		//same as ToString; for compatibility
		__declspec(deprecated) CString& GetString()
		{
			return m_strPath;
		}

		bool IsURL()
		{
			return PathIsURL(*this)!=FALSE;
		}

		bool IsEmpty()
		{
			return CString(*this).IsEmpty();
		}

		Public static bool IsFilenameHasInvalidChar(Path fileName)
		{
			return (CString(fileName).FindOneOf(L"\\/:*?\"<>|") !=-1 )
				? true : false;
		}

		Public static CString RemoveInvalidCharOfFileName(CString fileName, WCHAR replaceChar='-')
		{	
			//replace all invalid character with replaceChar
			for (int i=0; i<fileName.GetLength(); i++)
			{
				UINT charType = PathGetCharType(fileName.GetAt(i));
				if ( charType==GCT_INVALID || charType==GCT_WILD)
					fileName.SetAt(i, replaceChar);
			}
			return fileName;
		}

		Path Replace(CString oldString, CString newString)
		{
			Path newPath = *this;
			newPath.ToString().Replace(oldString, newString);
			return newPath;
		}

		Path Replace(TCHAR oldChar, TCHAR newChar)
		{
			Path newPath = *this;
			newPath.ToString().Replace(oldChar, newChar);
			return newPath;
		}

		int CompareNoCase(Path path)
		{
			return ToString().CompareNoCase(path.ToString());
		}

		bool HasBackslash()
		{
			return ToString().GetLength()>0 && ToString().Right(1)==L"\\";
		}

		Path FindFirstComponent(Path* prestComponent=NULL)
		{
			CString path = *this;
			path.Replace(L"/",L"\\");
			path.TrimLeft(L"\\"); //if has first backslash fount \file, first component is file
			
			LPCWSTR pszPath = path;
			LPCWSTR pszRest = PathFindNextComponent(pszPath);
			if (pszRest==NULL) return Path(); //not found

			CString ret(path, (int)(pszRest-pszPath));
			ret.Trim(L"\\");
			if (prestComponent!=NULL) *prestComponent = Path(pszRest);
			return Path(ret);
		}


		Path GetUnique(CString* paddedString=NULL)
		{
			return GetUnique(*this, paddedString);
		}

		Public static Path GetUnique(Path pathName, CString* paddedString=NULL)
		{
			if (pathName.FileExists())
			{
				CString extension = pathName.GetExtension();
				CString pathNameString = pathName.GetRemoveExtension();

				//find previous counter
				int count = pathNameString.GetLength();
				CString counterString;
				for (int i=count-1; i>=0; i--)
				{
					if ( pathNameString[i]>='0' && pathNameString[i]<='9' )
						counterString = pathNameString[i] + counterString;
					else
						break;
				}
				
				//remove old counter from path
				pathNameString.Delete( count - counterString.GetLength(), counterString.GetLength() );
				int counter = Convert::ToInt(counterString)+1;
				if (counter==1) counter=2;
				CString addedString;
				addedString.Format(L"%d", counter);
				if (paddedString!=NULL) *paddedString = addedString;

				Path pathName2 = Path(pathNameString + addedString + extension);
				return GetUnique(pathName2, paddedString);
			}

			return pathName;
		}

		//return fileName from fullpath of file 
		//(eg: if strPathName is "c:\myfile.ext" return "myfile.ext"; if "c:\" return ""
		Path GetFileName()
		{
			CString path = *this;
			if (path.GetLength()>0 && (path.Right(1)==L"\\" || path.Right(1)==L"/"))
				return Path();
			return Path(PathFindFileName(*this));
		}

		CString ToURL()
		{
			if (IsURL())
				return ToString();

			CString str = ToString();			
			str.Replace('\\', '/');
			return L"file:///" + str;
		}

		bool IsRelativeToFolder(Path folderName)
		{
			Path fileName = *this;
			fileName.Canonicalize();
			folderName.Canonicalize();

			//add backslash to prevent match half of file eg: c:\tempFile instead of c:\temp
			fileName.AddBackslash();
			folderName.AddBackslash();
			
			return fileName.ToString().Left(folderName.ToString().GetLength()).CompareNoCase(folderName.ToString())==0;
		}

		bool IsDirectoryEmpty()
		{
			return PathIsDirectoryEmpty(*this)!=FALSE;
		}

		bool IsFile()
		{
			return FileExists() && !IsDirectory();
		}

		Path GetFolder()
		{
			if (IsRoot())
				return Path(); //parent of root such c:\

			Path fileName = *this;
			int slash = fileName.ToString().ReverseFind('\\');
			if (slash!=-1)
			{
				fileName.ToString().Delete(slash, fileName.ToString().GetLength()-slash);
				return fileName;
			}
			return Path();
		}

		Path GetSkipRoot()
		{
			Path ret = Path(PathSkipRoot(*this));
			return ret;
		}

		Path GetRemoveBackslash()
		{
			Path ret = *this;
			ret.RemoveBackslash();
			return ret;
		}

		Path GetAddBackslash()
		{
			Path ret = *this;
			ret.AddBackslash();
			return ret;
		}

		Path GetRenameExtension(PCXSTR pszExtension)
		{
			Path ret = *this;
			ret.RenameExtension(pszExtension);
			return ret;
		}

		//@param pszExtension should have dot such as ".htm"
		//@note If there is already a file extension present, no extension will be added 
		Path GetAddExtension(PCXSTR pszExtension)
		{
			Path ret = *this;
			ret.AddExtension(pszExtension);
			return ret;
		}

		Path GetRemoveExtension()
		{
			Path ret = *this;
			ret.RemoveExtension();
			return ret;
		}

		Path GetRoot()
		{
			Path ret = *this;
			ret.StripToRoot();
			return ret;
		}
		
		//return FileTitle. FileTitle is fileName whitout extension
		//(eg: if strPathName is "c:\myfile.ext" return "myfile"
		CString GetTitle()
		{
			Path ret = GetFileName();
			ret.RemoveExtension();
			return ret;
		}

		//CPath has box when Path is empty
		void Canonicalize()
		{
			if (IsEmpty())
				return;
			__super::Canonicalize();
		}

		// @see PathCanonicalize
		Path GetCanonicalize()
		{
			Path ret = *this;
			ret.Canonicalize();
			return ret;
		}

		// @param strFrom must folder
		// @return empty string if could not make relative path; relative path from strFrom; return "." if strTo same folder as strFrom
		// eg: if current path is "c:\temp\folder\file.ext" and fromPath is "c:\temp" then return "folder\file.ext" 
		Path GetRelativePathFrom(Path fromPath)
		{
			Path ret;
			ret.RelativePathTo(fromPath, FILE_ATTRIBUTE_DIRECTORY, *this, FILE_ATTRIBUTE_DIRECTORY);
			CString retString = ret.ToString();
			if (retString.GetLength()>2 && retString.Left(2)==L".\\")
				retString.Delete(0, 2);
			return Path(retString);
		}
	};

	inline Path operator+(const Path& path1, const Path& path2)
	{
		Path ret(path1);
		ret.Append(path2);
		return ret;
	}

	inline bool operator==( const Path& path1, const Path& path2 )
	{
		return path1.m_strPath.Compare( path2.m_strPath ) == 0;
	}

	inline bool operator!=( const Path& path1, const Path& path2 )
	{
		return path1.m_strPath.Compare( path2.m_strPath ) != 0;
	}

	//PathList class
	DEFINE_COMPTR(PathList);
	class PathList : public IUnknownImpl<PathList>, public CAtlList<Path>
	{
	public:
		virtual PathListPtr Clone()
		{
			PathListPtr pnew = new PathList();
			POSITION pos = GetHeadPosition();
			while (pos!=NULL)
			{
				pnew->AddTail(GetNext(pos));
			}
			return pnew;
		}

		virtual bool Equals(PathListPtr obj)
		{
			if (obj==NULL || GetCount()!=obj->GetCount())
				return false;

			POSITION pos1 = GetHeadPosition();
			POSITION pos2 = obj->GetHeadPosition();
			while (pos1!=NULL)
			{
				if ( GetNext(pos1)!=obj->GetNext(pos2) )
					return false; //object not Equals
			}
			return true;
		}

		virtual POSITION FindNoCase(Path path)
		{
			POSITION pos = GetHeadPosition();
			while (pos!=NULL)
			{
				POSITION curPos = pos;
				if (GetNext(pos).CompareNoCase(path)==0)
					return curPos;
			}
			return NULL;
		}

	protected:
		virtual ~PathList()
		{
		}
	};
}