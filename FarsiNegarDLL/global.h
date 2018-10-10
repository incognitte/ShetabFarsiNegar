#pragma once

#define FONTMAP_FILEEXT		L"fontmap"
#define REGKEY_APP			L"Software\\Shetab\\Farsi Negar\\4.0"
#define REGISTER_DAY	0x0000FA85
#define FILTER_FONTMAP		L"Fontmap Files (*.FontMap)|*.FontMap|"
#define FILTER_ALLFILES		L"All files (*.*)|*.*|"


enum GrabMessage{
	gmDoAutoInsert,
	gmNeedConvertorMenu,
	gmDoConvertor,
};

enum NeedData{
	needEnableAI,
	needEnableFastConvertor,
};

//global declear
const UINT g_uMessageGrab = RegisterWindowMessage(L"GlypherGrab");
const UINT g_uMessageShow = RegisterWindowMessage(L"GlypherShow");
const UINT g_uMessageGetData = RegisterWindowMessage(L"GlypherGetData");
