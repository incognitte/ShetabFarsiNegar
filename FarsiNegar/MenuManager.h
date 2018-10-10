#pragma once

class CMenuItem
{
public:
	enum Command{
		cmdCut,
		cmdCopy,
		cmdPaste,
		cmdConvertCopyToWin, 
		cmdConvertCopyFromWin, 
		cmdConvertPasteToWin,
		cmdConvertPasteFromWin,
		cmdConvert,
		cmdFont,
		cmdAI,
		cmdDownloadFont,
	};

public:
	CMenuItem() { m_nCommand=0; }
	UINT m_nCommand; // one of the Command
	FontMapPtr pSrcFontMap; // source FontMap id
	FontMapPtr pDesFontMap; // destination FontMap id
	CString m_string;
};

DEFINE_COMPTR(MenuManager);
class MenuManager : public IUnknownImpl<MenuManager>
{
	Public enum ShMenuType{
		ShMenuPaste2,
		ShMenuCut2,
		ShMenuCopy2,
		ShMenuCount,
	};

	Public MenuManager()
	{
		FontIndex = 0;
		AIIndex = 0;
		DWFontIndex = 0;
		ConvertIndex = 0;
	}

	Public BOOL InsertItem(CMenu* pmenu, CMenuItem* pmenuItem, UINT menuId)
	{
		MENUITEMINFO mi={0};
		mi.cbSize = sizeof MENUITEMINFO;
		mi.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_ID | MIIM_STRING;
		//mi.fType = MFT_RIGHTJUSTIFY | MFT_RIGHTORDER; //mainframe will reorder when have RTL stlye; if MFT_RIGHTJUSTIFY set it will back to LTR
		mi.wID = menuId;
		mi.dwItemData = (ULONG_PTR)pmenuItem;
		mi.dwTypeData = pmenuItem->m_string.GetBuffer();
		mi.cch = pmenuItem->m_string.GetLength();
		BOOL ret = pmenu->InsertMenuItem((UINT)-1, &mi, TRUE);
		pmenuItem->m_string.ReleaseBuffer();
		return ret;
	}

	Public CMenuItem* GetCutItem(UINT nMenuId)
	{
		UINT nIndex = nMenuId-MENUID_CUT_FIRST;
		return &Cut[nIndex];
	}

	Public CMenuItem* GetCopyItem(UINT nMenuId)
	{
		UINT nIndex = nMenuId-MENUID_COPY_FIRST;
		return &Copy[nIndex];
	}

	Public CMenuItem* GetPasteItem(UINT nMenuId)
	{
		UINT nIndex = nMenuId-MENUID_PASTE_FIRST;
		return &Paste[nIndex];
	}

	Public CMenuItem* GetFontItem(UINT nMenuId)
	{
		UINT nIndex = nMenuId-MENUID_FONT_FIRST;
		return &Font[nIndex];
	}

	Public CMenuItem* GetAIItem(UINT nMenuId)
	{
		UINT nIndex = nMenuId-MENUID_FONT_FIRST;
		return &Font[nIndex];
	}

	Public CMenuItem* GetDWFontItem(UINT nMenuId)
	{
		UINT nIndex = nMenuId-MENUID_DWFONT_FIRST;
		return &DWFont[nIndex];
	}

	Public CMenu* GetContextView() 
	{ 
		return ContextView.GetSubMenu(0); 
	}

	Public CMenu* GetShMenu(ShMenuType value) 
	{ 
		return ShMenu[value].GetSubMenu(0); 
	}

	Public bool GetItemData(HMENU menuHandle, UINT id, ULONG_PTR& dwItemData)
	{
		MENUITEMINFO inf={0};
		inf.cbSize = sizeof MENUITEMINFO;
		inf.fMask = MIIM_DATA;
		bool res = GetMenuItemInfo(menuHandle, id, FALSE, &inf)!=FALSE;
		dwItemData = inf.dwItemData;
		return res;
	}

	// @return -1 if not found
	Public static int FindPosById(HMENU menuHandle, UINT uId)
	{
		for (int i=0; i<(int)GetMenuItemCount(menuHandle); i++)
		{
			if (GetMenuItemID(menuHandle, i)==uId)
				return i;
		}
		return -1;
	}


	UINT FontIndex;
	UINT AIIndex;
	UINT DWFontIndex;
	UINT ConvertIndex;
	CMenuItem Cut[50];
	CMenuItem Copy[50];
	CMenuItem Paste[50];
	CMenuItem Font[50];
	CMenuItem AI[50];
	CMenuItem DWFont[50];
	CMenuItem ConvertItem[1000];
	CMenu Main;
	CMenu ContextView;
	CMenu ToolCut;
	CMenu toolCopy;
	CMenu toolPaste;
	CMenu toolFont;
	CMenu toolAI;
	CMenu Convert;
	CMenu ShMenu[ShMenuCount];

	Protected ~MenuManager()
	{
	}
};
