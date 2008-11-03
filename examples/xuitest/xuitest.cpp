// xuitest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "xuitest.h"
#include "xuilib.h"
#include <commctrl.h>
#include <ximage.h>
#include "arrays.h"

#define FREE_CLEAR_STR(str) if(str) delete str; str = NULL;
#define MAKE_STR(str, cpstr) FREE_CLEAR_STR(str); if(cpstr) { str = new TCHAR[lstrlen(cpstr) + 1]; lstrcpy(str, cpstr); }


class MyXUIEngine : public CXUIEngine
{
public:
	MyXUIEngine(HINSTANCE hInst);
	~MyXUIEngine();
	void DrawImage(HDC hdc, int x, int y, int width, int height, LPWSTR imgUrl);
};

MyXUIEngine::MyXUIEngine(HINSTANCE hInst) : CXUIEngine(hInst)
{
	
}

MyXUIEngine::~MyXUIEngine()
{

}

void MyXUIEngine::DrawImage(HDC hdc, int x, int y, int width, int height, LPWSTR imgUrl)
{
	CxImage img;
	img.LoadResource(FindResource(m_hInst, imgUrl, TEXT("XUILIB")), CXIMAGE_FORMAT_PNG, m_hInst);
	if(img.IsValid())
	{
		img.Draw(hdc, x, y, width, height);
	}
}

class CTlbSettingsDlg : public CXUIDialog
{
public:
	CTlbSettingsDlg(CXUIEngine* engine);
	virtual ~CTlbSettingsDlg();

	XUI_BEGIN_DATA_MAP
		XUI_DATA_MAP_PSTR(L"pwd1",			m_pwd1)
		XUI_DATA_MAP_STR (L"pwd2",			m_pwd2)
		XUI_DATA_MAP_INT (L"lockbtns",		m_lockButtons)
		XUI_DATA_MAP_INT (L"loadInBg",		m_bgLoad)
		XUI_DATA_MAP_INT (L"enableAccel",	m_enableAccelerators)
		XUI_DATA_MAP_INT (L"recentSize",	m_recentSize)
		XUI_DATA_MAP_INT (L"lang",			m_lang)
		
		XUI_DATA_MAP_STR (L"menufont",		m_MenuFont)
		XUI_DATA_MAP_STR (L"captionfont",	m_CaptionFont)
		XUI_DATA_MAP_STR (L"titlefont",		m_TitlesFont)
	XUI_END_DATA_MAP

	XUI_BEGIN_BIND_MAP
		XUI_BIND_ITEM(L"lang",			L"combobox",	m_langs)
		XUI_BIND_ITEM(L"apply",			L"button",		m_apply)

		XUI_BIND_ITEM(L"menufont",		L"combobox",	m_fntMenu)
		XUI_BIND_ITEM(L"captionfont",	L"combobox",	m_fntCaption)
		XUI_BIND_ITEM(L"titlefont",		L"combobox",	m_fntTitle)
	XUI_END_BIND_MAP

	XUI_BEGIN_MESSAGE_MAP
		XUI_COMMAND_HANDLER(L"help", BN_CLICKED, OnHelp)
	XUI_END_MESSAGE_MAP

	XUI_BEGIN_EVENT_MAP
		XUI_HANDLE_CHANGED_ALL(OnChanged)
		XUI_HANDLE_HSCROLL_ALL(OnScrollTransparency)

		XUI_HANDLE_TABINIT_ALL(OnTabInit)

		XUI_HANDLE_MEASUREITEM(L"menufont",		OnMeasureFontItem)
		XUI_HANDLE_MEASUREITEM(L"captionfont",	OnMeasureFontItem)
		XUI_HANDLE_MEASUREITEM(L"titlefont",	OnMeasureFontItem)

		XUI_HANDLE_DRAWITEM(L"menufont",	OnDrawFontItem)
		XUI_HANDLE_DRAWITEM(L"captionfont",	OnDrawFontItem)
		XUI_HANDLE_DRAWITEM(L"titlefont",	OnDrawFontItem)
	XUI_END_EVENT_MAP
	
	virtual void OnInitDialog();
	virtual BOOL OnEndDialog(UINT code);
public:
	// Data
	LPWSTR	m_pwd1;
	WCHAR	m_pwd2[30];
	BOOL	m_lockButtons;
	BOOL	m_bgLoad;
	BOOL	m_enableAccelerators;
	INT		m_recentSize;
	INT		m_lang;
	WCHAR	m_CaptionFont[32];
	WCHAR	m_MenuFont[32];
	WCHAR	m_TitlesFont[32];

	// Controls
	CXUIComboBox*	m_langs;
	CXUIButton*		m_apply;

	CXUIComboBox*	m_fntMenu;
	CXUIComboBox*	m_fntTitle;
	CXUIComboBox*	m_fntCaption;

	// Handlers
	BOOL OnHelp(CXUIElement* el, UINT code);
	BOOL OnChanged(CXUIElement* el);
	BOOL OnScrollTransparency(CXUIElement* el, LPARAM lParam);
	BOOL OnTabInit(CXUITab* tab);
	BOOL OnMeasureFontItem(CXUIElement* el, UINT dlgID, LPMEASUREITEMSTRUCT lpMeasureItem);
	BOOL OnDrawFontItem(CXUIElement* el, UINT dlgID, LPDRAWITEMSTRUCT lpDrawItem);
	
	void fillFonts();
};


CTlbSettingsDlg::CTlbSettingsDlg(CXUIEngine* engine) : CXUIDialog(L"res:tlbsettings.xml", engine)
{

}
CTlbSettingsDlg::~CTlbSettingsDlg()
{

}

BOOL CTlbSettingsDlg::OnHelp(CXUIElement* el, UINT code)
{
	MessageBox(m_hWnd, L"HELP", L"HELP", MB_OK);
	return TRUE;
}

void CTlbSettingsDlg::OnInitDialog()
{
	CXUIDialog::OnInitDialog();

	m_langs->addItem(0, L"English");
	m_langs->addItem(1, L"Russian");
}

BOOL CTlbSettingsDlg::OnTabInit(CXUITab* tab)
{
	if(!lstrcmp(tab->get_id(), L"tabFonts"))
	{
		fillFonts();
	}
	dataExchange(FALSE, tab);
	return TRUE;
}


BOOL CTlbSettingsDlg::OnEndDialog(UINT code)
{
	if(code == IDOK)
	{
		dataExchange(TRUE, NULL);
	}
	return TRUE;
}

BOOL CTlbSettingsDlg::OnChanged(CXUIElement* el)
{
	m_apply->set_disabled(FALSE);
	return TRUE;
}

BOOL CTlbSettingsDlg::OnScrollTransparency( CXUIElement* el, LPARAM lParam )
{
	CXUISlider* slider = NULL;
	CXUILabel* label = NULL;
	el->QueryElement(L"slider", (LPVOID*) &slider);
	if(slider)
	{
		if(el->get_id())
		{
			WCHAR lID[100] = L"l_";
			lstrcat(lID, el->get_id());
			find(lID)->QueryElement(L"label", (LPVOID*) &label);
			if(label)
			{
				int minValue = slider->minValue();
				int maxValue = slider->maxValue();
				int pos = slider->value_INT();
				int sz = maxValue - minValue;
				int percent = pos * 100 / sz;
				WCHAR str[100];
				swprintf(str, L"%d%%", percent);
				label->value_STR(str);
			}
		}
		return OnChanged(NULL);
	}
	return FALSE;
}

BOOL CTlbSettingsDlg::OnMeasureFontItem( CXUIElement* el, UINT dlgID, LPMEASUREITEMSTRUCT lpMeasureItem )
{
	if((INT) lpMeasureItem->itemID >= 0)
	{
		lpMeasureItem->itemHeight = GetSystemMetrics(SM_CYMENU);
	}

	return TRUE;
}

BOOL CTlbSettingsDlg::OnDrawFontItem( CXUIElement* el, UINT dlgID, LPDRAWITEMSTRUCT lpDrawItem )
{
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	SendMessage(el->get_wnd(), CB_GETLBTEXT, lpDrawItem->itemID, (LPARAM) lf.lfFaceName);
	lf.lfHeight			= lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top;
	lf.lfWeight			= FW_NORMAL;
	lf.lfCharSet		= DEFAULT_CHARSET;
	lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	lf.lfQuality		= 5;
	HFONT hFont = CreateFontIndirect(&lf);
	HFONT oldFont = (HFONT) SelectObject(lpDrawItem->hDC, hFont);

	COLORREF txtColor;
	if(lpDrawItem->itemState & ODS_SELECTED)
	{
		FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, (HBRUSH) (COLOR_HIGHLIGHT + 1));
		txtColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
	} else
	{
		FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, (HBRUSH) (COLOR_WINDOW + 1));
		txtColor = GetSysColor(COLOR_WINDOWTEXT);
	}

	SetBkMode(lpDrawItem->hDC, TRANSPARENT);
	SetTextColor(lpDrawItem->hDC, txtColor);
	RECT rcText = lpDrawItem->rcItem;
	rcText.left += 3;
	DrawText(lpDrawItem->hDC, lf.lfFaceName, -1, &rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	if(lpDrawItem->itemState & ODS_FOCUS) 
	{
		//DrawFocusRect(lpDrawItem->hDC, &lpDrawItem->rcItem);
	}

	SelectObject(lpDrawItem->hDC, oldFont);
	DeleteObject(hFont);

	return TRUE;
}

int CALLBACK MyEnumFonts(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam)
{
	CTArray<LPTSTR>* arr = (CTArray<LPTSTR>*) lParam;
	LPTSTR fontName = NULL;
	MAKE_STR(fontName, lpelfe->elfLogFont.lfFaceName);
	arr->Add(&fontName);
	return TRUE;
}


void CTlbSettingsDlg::fillFonts()
{
	CTArray<LPTSTR> fonts;
	HDC hdc = GetDC(m_hWnd);
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(lf));
	lf.lfCharSet = ANSI_CHARSET;
	EnumFonts(hdc, NULL, (FONTENUMPROC) MyEnumFonts, (LPARAM) &fonts);
	ReleaseDC(m_hWnd, hdc);

	m_fntCaption->addItem(-1, L" <default>");
	m_fntMenu->addItem(-1, L" <default>");
	m_fntTitle->addItem(-1, L" <default>");

	for(int i=0; i < fonts.GetCount(); i++)
	{
		m_fntCaption->addItem(i, fonts[i]);
		m_fntMenu->addItem(i, fonts[i]);
		m_fntTitle->addItem(i, fonts[i]);

		delete fonts[i];
	}
	fonts.Clear();
}

class CSpanTestDlg : public CXUIDialog
{
public:
	CSpanTestDlg(CXUIEngine* engine);
	~CSpanTestDlg();
};

CSpanTestDlg::CSpanTestDlg(CXUIEngine* engine) : CXUIDialog(L"res:span.xml", engine)
{

}

CSpanTestDlg::~CSpanTestDlg()
{

}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	CoInitialize(NULL);

	InitCommonControls();
	
	MyXUIEngine engine(hInstance);
	//engine.loadStrings(TEXT("res\\tlbsettings-ru.xml"));

	CTlbSettingsDlg dlg(&engine);
	dlg.m_pwd1					= NULL;
	lstrcpy(dlg.m_pwd2, L"hello");
	dlg.m_lockButtons			= TRUE;
	dlg.m_bgLoad				= TRUE;
	dlg.m_enableAccelerators	= TRUE;
	dlg.m_recentSize			= 10;
	dlg.m_lang					= 1;

	lstrcpy(dlg.m_TitlesFont,	L"Tahoma");
	lstrcpy(dlg.m_CaptionFont,	L"Verdana");
	lstrcpy(dlg.m_MenuFont,		L"Arial");

//	CSpanTestDlg dlg(&engine);

	dlg.DoModal(NULL);

	return 0;
}

