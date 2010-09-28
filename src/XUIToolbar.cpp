#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>
#include "XUIToolbar.h"
#include <vssym32.h>
#include <uxtheme.h>

CXUIToolbar::CXUIToolbar(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_icons			= NULL;
	m_iconsCount	= 0;
}

CXUIToolbar::~CXUIToolbar(void)
{
	for(int i=0; i < m_iconsCount; i++)
	{
		delete m_icons[i].icon;
		DeleteObject(m_icons[i].hBitmap);
	}
	if(m_icons)
	{
		free(m_icons);
	}
}

BOOL CXUIToolbar::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;

	m_bAltDrag		= xmlGetAttributeValueBOOL(node, L"altDrag",		FALSE);
	m_bFlat			= xmlGetAttributeValueBOOL(node, L"flat",			FALSE);
	m_bList			= xmlGetAttributeValueBOOL(node, L"list",			FALSE);
	m_bTooltips		= xmlGetAttributeValueBOOL(node, L"tooltips",		FALSE);
	m_bDrawDDArrows	= xmlGetAttributeValueBOOL(node, L"drawDDArrows",	TRUE);
	m_bMixedButtons	= xmlGetAttributeValueBOOL(node, L"mixedButtons",	FALSE);
	m_btnWidth		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("btnWidth"),		20);
	m_btnHeight		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("btnHeight"),		20);

	return TRUE;
}

void CXUIToolbar::Init()
{
	DWORD dwStyle = WS_CHILD | CCS_ADJUSTABLE | WS_VISIBLE | TBSTYLE_AUTOSIZE;

	if(m_bAltDrag)	dwStyle |= TBSTYLE_ALTDRAG;
	if(m_bFlat)		dwStyle |= TBSTYLE_FLAT;
	if(m_bList)		dwStyle |= TBSTYLE_LIST;
	if(m_bTooltips)	dwStyle |= TBSTYLE_TOOLTIPS;

	// Create a toolbar. 
	m_hWnd = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, 
		dwStyle, 0, 0, 0, 0, m_parent->get_parentWnd(), 
		(HMENU) m_id, m_engine->get_hInstance(), NULL); 

	DWORD exStyle = 0;

	if(m_bDrawDDArrows)
	{
		exStyle |= TBSTYLE_EX_DRAWDDARROWS;
	}
	if(m_bMixedButtons)
	{
		exStyle |= TBSTYLE_EX_MIXEDBUTTONS;
	}
	if(exStyle)
	{
		SendMessage(m_hWnd, TB_SETEXTENDEDSTYLE, 0, exStyle);
	}
	SendMessage(m_hWnd, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);
	SendMessage(m_hWnd, TB_SETBITMAPSIZE, 0, MAKELONG(m_btnWidth, m_btnHeight));

	for(int i=0; i < m_childCount; i++)
	{
		CXUIToolbarButton* btn = NULL;
		if(m_childs[i]->QueryElement(L"toolbarbutton", (LPVOID*) &btn))
		{
			btn->setCmdID(i + 100);

			TBBUTTON btnData;
			btn->getButtonData(&btnData);
			SendMessage(m_hWnd, TB_INSERTBUTTON, i, (LPARAM) &btnData);
		}
	}
	//SendMessage(m_hWnd, TB_SETBUTTONSIZE, 0, MAKELONG(32, 32));

	//RECT rcToolbar;
	//GetWindowRect(m_hWnd, &rcToolbar);
	//m_minHeight = rcToolbar.bottom - rcToolbar.top;
	SIZE szToolbar = {0};
	SendMessage(m_hWnd, TB_GETMAXSIZE, 0, (LPARAM) &szToolbar);
	m_minHeight = szToolbar.cy;

	CXUIElement::Init();
}

BOOL CXUIToolbar::onNotify( int idCtrl, LPNMHDR pnmh )
{
	switch(pnmh->code)
	{
	case TBN_QUERYDELETE:
	case TBN_QUERYINSERT:
		SetWindowLong(m_parent->get_parentWnd(), DWLP_MSGRESULT, TRUE);
		break;
/*
	case NM_CUSTOMDRAW:
		{
			LPNMCUSTOMDRAW cd = (LPNMCUSTOMDRAW) pnmh;
			switch(cd->dwDrawStage)
			{
			case CDDS_PREPAINT:
				SetWindowLongPtr(m_parent->get_parentWnd(), DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
				break;
			case CDDS_ITEMPREPAINT:
				SetWindowLongPtr(m_parent->get_parentWnd(), DWLP_MSGRESULT, CDRF_NOTIFYPOSTPAINT / *| CDRF_SKIPDEFAULT* /);
				break;
			case CDDS_ITEMPOSTPAINT:
				{
					CXUIToolbarButton* btn = (CXUIToolbarButton*) cd->lItemlParam;
					btn->drawButton(cd->hdc, &cd->rc, cd->uItemState, m_btnWidth, m_btnHeight);
				}
				break;
			}
		}
		break;
*/
	case TBN_GETBUTTONINFO:
		{
			LPNMTOOLBAR dt = (LPNMTOOLBAR) pnmh; 

			CXUIToolbarButton* btn = NULL;
			for(int i=0; i < m_childCount; i++)
			{
				if(m_childs[i]->QueryElement(L"toolbarbutton", (LPVOID*) &btn))
				{
					if(i == dt->iItem)
					{
						btn->getButtonData(&dt->tbButton, FALSE);
						break;
					}
				}
				btn = NULL;
			}

			if(btn)
			{
				if(btn->getLabel())
				{
					StringCchCopy(dt->pszText, dt->cchText, btn->getLabel());
				}
				SetWindowLongPtr(m_parent->get_parentWnd(), DWLP_MSGRESULT, TRUE);
			}
		}
		break;
	case TBN_GETINFOTIP:
		{
			LPNMTBGETINFOTIP tipInfo = (LPNMTBGETINFOTIP) pnmh; 
			CXUIToolbarButton* btn = (CXUIToolbarButton*) tipInfo->lParam;
			if(btn->getTip())
			{
				StringCchCopy(tipInfo->pszText, tipInfo->cchTextMax, btn->getTip());
			}
		}
		break;
	case TBN_TOOLBARCHANGE:
		{
			return raiseEvent(XUI_EVENT_TOOLBAR_CHANGED, 0, NULL);
		}
		break;
	case TBN_DROPDOWN:
		{
			LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR) pnmh;
			RECT rcButton;
			SendMessage(lpnmtb->hdr.hwndFrom, TB_GETRECT, (WPARAM)lpnmtb->iItem, (LPARAM)&rcButton);
			MapWindowPoints(lpnmtb->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rcButton, 2);                         

			CXUIToolbarButton* btn = NULL;
			for(int i=0; i < m_childCount; i++)
			{
				if(m_childs[i]->QueryElement(L"toolbarbutton", (LPVOID*) &btn))
				{
					if(btn->getCmdID() == lpnmtb->iItem)
					{
						break;
					}
				}
				btn = NULL;
			}
			if(btn)
			{
				btn->raiseEvent(XUI_EVENT_TOOLBAR_DROPDOWN, 0, (LPARAM) &rcButton);
			}
			return TRUE;
		}
		break;
	}
	return TRUE;
}

BOOL CXUIToolbar::onCommnd( UINT code, UINT id, HWND hWnd )
{
	CXUIToolbarButton* btn = NULL;
	for(int i=0; i < m_childCount; i++)
	{
		if(m_childs[i]->QueryElement(L"toolbarbutton", (LPVOID*) &btn))
		{
			if(btn->getCmdID() == id)
			{
				break;
			}
		}
		btn = NULL;
	}
	if(btn)
	{
		btn->raiseEvent(XUI_EVENT_CLICKED, 0, NULL);
	}
	return TRUE;
}

void CXUIToolbar::saveToolbar( HKEY hkr, LPCWSTR pszSubKey, LPCWSTR pszValueName )
{
	TBSAVEPARAMS tbsp;
	tbsp.hkr = hkr;
	tbsp.pszSubKey = pszSubKey;
	tbsp.pszValueName = pszValueName;
	SendMessage(m_hWnd, TB_SAVERESTORE, TRUE, (LPARAM) &tbsp);
}

void CXUIToolbar::restoreToolbar( HKEY hkr, LPCWSTR pszSubKey, LPCWSTR pszValueName )
{
	TBSAVEPARAMS tbsp;
	tbsp.hkr = hkr;
	tbsp.pszSubKey = pszSubKey;
	tbsp.pszValueName = pszValueName;
	SendMessage(m_hWnd, TB_SAVERESTORE, FALSE, (LPARAM) &tbsp);
}

int CXUIToolbar::addIcon( LPCWSTR icon )
{
	if(!icon)
	{
		return -1;
	}
	for(int i=0; i < m_iconsCount; i++)
	{
		if(!StrCmpI(icon, m_icons[i].icon))
		{
			return m_icons[i].idx;
		}
	}
	if(!m_iconsCount)
	{
		m_icons = (ICONS_MAP*) malloc(sizeof(ICONS_MAP));
	} else
	{
		m_icons = (ICONS_MAP*) realloc(m_icons, sizeof(ICONS_MAP) * (m_iconsCount + 1));
	}

	BITMAPINFOHEADER bmpInfo;
	ZeroMemory(&bmpInfo, sizeof(bmpInfo));
	bmpInfo.biSize				= sizeof(bmpInfo);
	bmpInfo.biWidth				= m_btnWidth;
	bmpInfo.biHeight			= m_btnHeight;
	bmpInfo.biPlanes			= 1;
	bmpInfo.biBitCount			= 32;
	bmpInfo.biCompression		= BI_RGB;
	bmpInfo.biSizeImage			= m_btnWidth * m_btnHeight * 4;

	HDC hdc = GetDC(NULL);
	LPVOID pBit32;
	HBITMAP bmp = CreateDIBSection(hdc, (LPBITMAPINFO) &bmpInfo, DIB_RGB_COLORS, &pBit32, NULL, 0);
	ZeroMemory(pBit32, m_btnWidth * m_btnHeight * 4);

	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP oldBmp = (HBITMAP) SelectObject(memDC, bmp);
	m_engine->DrawImage(memDC, 0, 0, m_btnWidth, m_btnHeight, (LPWSTR) icon);
	SelectObject(memDC, oldBmp);
	DeleteDC(memDC);
	ReleaseDC(NULL, hdc);

	TBADDBITMAP tbmp;
	tbmp.hInst	= NULL;
	tbmp.nID	= (UINT_PTR) bmp;

	int idx = (int) SendMessage(m_hWnd, TB_ADDBITMAP, 1, (LPARAM) &tbmp);

	m_icons[m_iconsCount].hBitmap	= bmp;
	m_icons[m_iconsCount].idx		= idx;
	m_icons[m_iconsCount].icon		= new WCHAR[lstrlen(icon) + 1];
	lstrcpy(m_icons[m_iconsCount].icon, icon);
	m_iconsCount++;
	return idx;
}
//////////////////////////////////////////////////////////////////////////

CXUIToolbarButton::CXUIToolbarButton( CXUIElement* parent, CXUIEngine* engine ) : CXUIElement(parent, engine)
{
	m_icon		= NULL;
	m_label		= NULL;
	m_tip		= NULL;
}

CXUIToolbarButton::~CXUIToolbarButton()
{
	if(m_icon)	delete m_icon;
}

BOOL CXUIToolbarButton::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;

	if(m_icon)	delete m_icon;
	if(m_label)	delete m_label;
	if(m_tip)	delete m_tip;
	m_fsStyle	= 0;
	m_icon		= NULL;
	m_label		= NULL;
	m_tip		= NULL;
	
	m_fsStyle	|= xmlGetAttributeValueBOOL(node, L"button",		TRUE)	? BTNS_BUTTON			: 0;
	m_fsStyle	|= xmlGetAttributeValueBOOL(node, L"check",			FALSE)	? BTNS_CHECK			: 0;
	m_fsStyle	|= xmlGetAttributeValueBOOL(node, L"checkgroup",	FALSE)	? BTNS_CHECKGROUP		: 0;
	m_fsStyle	|= xmlGetAttributeValueBOOL(node, L"dropdown",		FALSE)	? BTNS_DROPDOWN			: 0;
	m_fsStyle	|= xmlGetAttributeValueBOOL(node, L"group",			FALSE)	? BTNS_GROUP			: 0;
	m_fsStyle	|= xmlGetAttributeValueBOOL(node, L"noprefix",		FALSE)	? BTNS_NOPREFIX 		: 0;
	m_fsStyle	|= xmlGetAttributeValueBOOL(node, L"separator",		FALSE)	? BTNS_SEP 				: 0;
	m_fsStyle	|= xmlGetAttributeValueBOOL(node, L"showtext",		FALSE)	? BTNS_SHOWTEXT			: 0;
	m_fsStyle	|= xmlGetAttributeValueBOOL(node, L"wholedropdown",	FALSE)	? BTNS_WHOLEDROPDOWN	: 0;

	m_icon			= xmlGetAttributeSTR(node, TEXT("icon"));
	m_label			= getLocalizedString(node, TEXT("label"));
	m_tip			= getLocalizedString(node, TEXT("tip"));
	m_iconWidth		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("iconWidth"),		16);
	m_iconHeight	= xmlGetAttributeValueNonSTR<int>(node,	TEXT("iconHeight"),		16);

	return TRUE;
}

void CXUIToolbarButton::Init()
{
	CXUIElement::Init();
}

void CXUIToolbarButton::getButtonData( LPTBBUTTON lpButton, BOOL withLabel )
{
	CXUIToolbar* toolbar = NULL;
	m_parent->QueryElement(L"toolbar",	(LPVOID*) &toolbar);

	ZeroMemory(lpButton, sizeof(TBBUTTON));
	lpButton->fsState	= TBSTATE_ENABLED;
	lpButton->fsStyle	= m_fsStyle;
	lpButton->dwData	= (DWORD_PTR) this;
	if(toolbar)
	{
		lpButton->iBitmap	= toolbar->addIcon(m_icon);
	}
	if((m_fsStyle & BTNS_SHOWTEXT) && withLabel)
	{
		lpButton->iString	= (INT_PTR) m_label;
	}
	if(m_fsStyle & TBSTYLE_SEP)
	{
		lpButton->idCommand	= 0;
	} else
	{
		lpButton->idCommand	= m_cmdID;
	}
}

void CXUIToolbarButton::drawButton( HDC hdc, LPRECT rcDraw, UINT uItemState, int btnWidth, int btnHeight )
{
	if(m_icon)
	{
		HTHEME hTheme = NULL;
/*
		__try
		{
			hTheme = OpenThemeData(NULL, L"Toolbar");
		} __except(1)
		{
		}

		if(hTheme)
		{
			UINT state = TS_NORMAL;
			if(uItemState & CDIS_HOT)
			{
				state = TS_HOT;
			}
			if(uItemState & CDIS_SELECTED)
			{
				state = TS_PRESSED;
			}
			DrawThemeBackground(hTheme, hdc, TP_BUTTON, state, rcDraw, rcDraw);
		} else
		{
			if(uItemState & CDIS_HOT)
			{
				DrawEdge(hdc, rcDraw, BDR_RAISEDINNER, BF_RECT);
			}
			if(uItemState & CDIS_SELECTED)
			{
				DrawEdge(hdc, rcDraw, BDR_SUNKENOUTER, BF_RECT);
			}
		}
*/

		int x = rcDraw->left + (rcDraw->bottom - rcDraw->top) / 2 - m_iconWidth / 2;
		int y = rcDraw->top + (rcDraw->bottom - rcDraw->top) / 2 - m_iconHeight / 2;

		if(uItemState & CDIS_SELECTED)
		{
			x++; y++;
		}
		BOOL isDisabled = FALSE;
		if((uItemState & CDIS_GRAYED) || (uItemState & CDIS_DISABLED))
		{
			m_engine->DrawDisabledImage(hdc, x, y, m_iconWidth, m_iconHeight, m_icon);
		} else
		{
			m_engine->DrawImage(hdc, x, y, m_iconWidth, m_iconHeight, m_icon);
		}


		if(hTheme)
		{
			CloseThemeData(hTheme);
		}
	}
}

void CXUIToolbarButton::updateDisabledState()
{
	SendMessage(m_parent->get_wnd(), TB_ENABLEBUTTON, m_cmdID, MAKELONG((get_disabled() ? FALSE : TRUE), 0));
}

void CXUIToolbarButton::setIcon( LPCWSTR icon )
{
	if(m_icon)
	{
		delete m_icon;
	}
	m_icon = new WCHAR[lstrlen(icon) + 1];
	lstrcpy(m_icon, icon);

	CXUIToolbar* toolbar = NULL;
	m_parent->QueryElement(L"toolbar",	(LPVOID*) &toolbar);
	
	if(toolbar)
	{
		TBBUTTONINFO btnInfo;
		ZeroMemory(&btnInfo, sizeof(TBBUTTONINFO));
		btnInfo.cbSize	= sizeof(TBBUTTONINFO);
		btnInfo.dwMask	= TBIF_IMAGE;
		btnInfo.iImage	= toolbar->addIcon(icon);
		SendMessage(toolbar->get_wnd(), TB_SETBUTTONINFO, m_cmdID, (LPARAM) &btnInfo);
	}
}

void CXUIToolbarButton::value_STR( LPCWSTR val )
{
	if(m_label)
	{
		delete m_label;
		m_label = NULL;
	}
	if(val)
	{
		m_label = new WCHAR[lstrlen(val) + 1];
		lstrcpy(m_label, val);
	}

	if(m_fsStyle & BTNS_SHOWTEXT)
	{
		CXUIToolbar* toolbar = NULL;
		m_parent->QueryElement(L"toolbar",	(LPVOID*) &toolbar);

		if(toolbar)
		{
			TBBUTTONINFO btnInfo;
			ZeroMemory(&btnInfo, sizeof(TBBUTTONINFO));
			btnInfo.cbSize	= sizeof(TBBUTTONINFO);
			btnInfo.dwMask	= TBIF_TEXT;
			btnInfo.pszText	= m_label;
			btnInfo.cchText	= lstrlen(m_label) + 1;
			SendMessage(toolbar->get_wnd(), TB_SETBUTTONINFO, m_cmdID, (LPARAM) &btnInfo);
		}
	}
}
