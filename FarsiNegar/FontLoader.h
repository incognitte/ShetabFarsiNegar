#pragma once

class CFontLoader
{
	Public CFontLoader(void)
	{
	}

	Public ~CFontLoader(void)
	{
		RemoveAllFonts();
		PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	}
	
	// @param fontFolder folder that other font folder exist in it
	// @param fontLoader folder that need to load; format: foldername=0; foldername=1;
	void AddRemoveFonts(Path fontFolder, CString fontLoader)
	{
		//remove all added fonts if fontfolder is changed
		if ( fontFolder.CompareNoCase(FontFolderField)!=0 )
			RemoveAllFonts();

		//set new font folder
		FontFolderField = fontFolder;

		//remove fonts that loaded but currently must removed
		PathListPtr pfolders = GetFontFolders(FontFolderField);
		POSITION pos = pfolders->GetHeadPosition();
		while (pos!=NULL)
		{
			Path folder = pfolders->GetNext(pos);
			CString folderName = folder.GetFileName();
			bool bLoaded = Param::Get(folderName, 0, LoadedFontsField)!=0; //default is not loaded
			bool bChecked = Param::Get(folderName, 1, fontLoader)!=0; //default checked
			//unload fonts in folder if already loaded
			if ( bLoaded && !bChecked )
			{
				UnloadFonts(folder);
				Param::Set(folderName, 0, LoadedFontsField);
			}
			
			//load fonts in folder if not loaded
			if ( !bLoaded && bChecked )
			{
				LoadFonts(folder);
				Param::Set(folderName, 1, LoadedFontsField);
			}
		}

		//call all windows that font changed
		PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	}

	// @note dont forget send WM_FONTCHANGE
	Public static void LoadFonts(Path fontFolder)
	{
		PathListPtr pfonts = new PathList();
		pfonts->AddTailList( m::File::Find(fontFolder, L"*.ttf", m::File::FindFlagFile) );
		pfonts->AddTailList( m::File::Find(fontFolder, L"*.fon", m::File::FindFlagFile) );
		POSITION pos = pfonts->GetHeadPosition();
		while (pos!=NULL)
			AddFontResource( pfonts->GetNext(pos) );
	}

	// @note dont forget send WM_FONTCHANGE
	Public static void UnloadFonts(Path fontFolder)
	{
		PathListPtr pfonts = new PathList();
		pfonts->AddTailList( m::File::Find(fontFolder, _T("*.ttf"), m::File::FindFlagFile) );
		pfonts->AddTailList( m::File::Find(fontFolder, _T("*.fon"), m::File::FindFlagFile) );
		POSITION pos = pfonts->GetHeadPosition();
		while (pos!=NULL)
			RemoveFontResource( pfonts->GetNext(pos) );
	}

	Public static PathListPtr GetFontFolders(Path fontFolder)
	{
		return m::File::Find(fontFolder, L"*.*", m::File::FindFlagFolder);
	}


	// add new fonts, 
	// @note dont forget send WM_FONTCHANGE
	Private void AddAllFonts()
	{
		CWaitCursor wc;

		//get all font folders
		PathListPtr pfolders = GetFontFolders(FontFolderField);
		POSITION pos = pfolders->GetHeadPosition();
		while (pos!=NULL)
		{
			Path folder = pfolders->GetNext(pos);
			Path folderName = folder.GetFileName();
			bool bLoaded = Param::Get(folderName, 0, LoadedFontsField)!=0;
			if (!bLoaded)
			{
				LoadFonts(folder);
				Param::Set(folderName, 1, LoadedFontsField);
			}
		}
	}

	// remove all loaded fonts
	// @note dont forget send WM_FONTCHANGE
	Private void RemoveAllFonts()
	{
		//get all font folders
		PathListPtr pfolders = GetFontFolders(FontFolderField);
		POSITION pos = pfolders->GetHeadPosition();
		while (pos!=NULL)
		{
			Path folder = pfolders->GetNext(pos);
			CString folderName = folder.GetFileName();
			bool bLoaded = Param::Get(folderName, 0, LoadedFontsField)!=0;
			if (bLoaded)
			{
				Param::Set(folderName, 0, LoadedFontsField);
				UnloadFonts(folder);
			}
		}
	}

	// @param folders full path of each fonts
	Path FontFolderField; //save current font folder
	CString LoadedFontsField; //save font folder that already loaded
};
