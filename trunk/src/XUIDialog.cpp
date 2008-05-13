#include "xuilib.h"
#include "xmltools.h"

//local struct used for implementation
#pragma pack(push, 1)
struct DLGTEMPLATEEX
{
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;

	// Everything else in this structure is variable length,
	// and therefore must be determined dynamically

	// sz_Or_Ord menu;			// name or ordinal of a menu resource
	// sz_Or_Ord windowClass;	// name or ordinal of a window class
	// WCHAR title[titleLen];	// title string of the dialog box
	// short pointsize;			// only if DS_SETFONT is set
	// short weight;			// only if DS_SETFONT is set
	// short bItalic;			// only if DS_SETFONT is set
	// WCHAR font[fontLen];		// typeface name, if DS_SETFONT is set
};

struct DLGITEMTEMPLATEEX
{
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	short x;
	short y;
	short cx;
	short cy;
	WORD id;
	WORD   dummy;

	// Everything else in this structure is variable length,
	// and therefore must be determined dynamically

	// sz_Or_Ord windowClass;	// name or ordinal of a window class
	// sz_Or_Ord title;			// title string or ordinal of a resource
	// WORD extraCount;			// bytes following creation data
};
#pragma pack(pop)

CXUIDialog::CXUIDialog(LPCTSTR fileName, CXUIEngine* engine, CXUIElement* parent) : CXUIElement(parent, engine), m_contextHelp(engine)
{
	if(fileName)
	{
		m_fileName = new WCHAR[lstrlen(fileName) + 1];
		lstrcpy(m_fileName, fileName);
	} else
	{
		m_fileName = NULL;
	}
	m_hDlg				= NULL;
	m_caption			= NULL;
	m_style				= 0;
	m_exStyle			= 0;
	m_bContextHelp		= FALSE;
	m_minWindowWidth	= 0;
	m_minWindowHeight	= 0;

	m_nextDlgID			= 100;

	m_marginLeft		= 5;
	m_marginRight		= 5;
	m_marginTop			= 5;
	m_marginBottom		= 5;
	m_defButton			= NULL;
}

CXUIDialog::~CXUIDialog(void)
{
	if(m_hDlg) GlobalFree(m_hDlg);
	if(m_caption) delete m_caption;
	if(m_fileName) delete m_fileName;
}

UINT CXUIDialog::DoModal(HWND hWndParent)
{
	if(loadFile(m_fileName, TEXT("dialog"), m_engine->get_hInstance()))
	{
		if(!m_hDlg)
		{
			m_hDlg = createDialog(m_left, m_top, m_width, m_height);
		}
		if(m_hDlg)
		{
			return (UINT) DialogBoxIndirectParam(m_engine->get_hInstance(), m_hDlg, hWndParent, (DLGPROC) CXUIDialog::WndProc, (LPARAM)this); 
		}
	}
	return 0;
}

LPWORD lpwAlign ( LPWORD lpIn)
{
	ULONG ul;

	ul = (ULONG) lpIn;
	ul +=3;
	ul >>=2;
	ul <<=2;
	return (LPWORD) ul;
}

LPDLGTEMPLATE CXUIDialog::createDialog(int left, int top, int width, int height)
{
	HGLOBAL hgbl;
	DLGTEMPLATEEX* lpdt;
	LPWORD lpw;
	LPWSTR lpwsz;
	int nchar;

	hgbl = GlobalAlloc(GMEM_ZEROINIT, 2024);
	if (!hgbl)
		return NULL;

	lpdt = (DLGTEMPLATEEX*)GlobalLock(hgbl);
	lpdt->dlgVer	= 1;
	lpdt->signature = 0xFFFF;
	lpdt->helpID	= 0;
	lpdt->style		= m_style;
	lpdt->exStyle	= m_exStyle;
	lpdt->cDlgItems = 0;
	lpdt->x			= (short) CW_USEDEFAULT;  
	lpdt->y			= (short) CW_USEDEFAULT;
	lpdt->cx		= width; 
	lpdt->cy		= height;

	lpw = (LPWORD) (lpdt + 1);
	*lpw++ = 0;   // no menu
	*lpw++ = 0;   // predefined dialog box class (by default)

	if(m_caption)
	{
		lpwsz = (LPWSTR) lpw;
		lstrcpy(lpwsz, m_caption);
		nchar = (lstrlen(m_caption) + 1)/* * sizeof(WCHAR)*/;
		lpw   += nchar;
	} else
	{
		*lpw++ = 0;   // no caption
	}

	HFONT fnt = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT lf;
	GetObject(fnt, sizeof(LOGFONT), &lf);

	if(m_style & DS_SETFONT)
	{
		*lpw++ = (WORD) lf.lfHeight;	// pointsize
		*lpw++ = (WORD) lf.lfWeight;	// weight
		LPBYTE lpb = (LPBYTE) lpw;
		*lpb++ = lf.lfItalic;			// italic
		*lpb++ = lf.lfCharSet;			// charset
		lpw = (LPWORD) lpb;
		//font face
		lpwsz = (LPWSTR) lpw;
		lstrcpy(lpwsz, lf.lfFaceName);
		nchar = lstrlen(lf.lfFaceName) + 1;
		lpw   += nchar;
	}

	DLGITEMTEMPLATEEX* lpdit;
	lpw = lpwAlign (lpw); // align DLGITEMTEMPLATE on DWORD boundary
	lpdit = (DLGITEMTEMPLATEEX*) lpw;
	lpdit->x  = 7; lpdit->y  = 7;
	lpdit->cx = 20; lpdit->cy = 20;
	lpdit->id = 666;  // OK button identifier
	lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON;
	lpdit->helpID = 0;
	lpdit->exStyle = 0;

	lpw = (LPWORD) (lpdit + 1);
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0080;    // button class

	lpwsz = (LPWSTR) lpw;
	lstrcpy(lpwsz, L"Hello");
	nchar = (lstrlen(L"Hello") + 1)/* * sizeof(WCHAR)*/;
	lpw   += nchar;
	*lpw++ = 0;           // no creation data

	GlobalUnlock(hgbl); 
	return (LPDLGTEMPLATE) hgbl; 
}

BOOL CXUIDialog::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_caption) delete m_caption;
	m_caption		= getLocalizedString(node, TEXT("caption"));
	m_bCenterScreen	= xmlGetAttributeValueBOOL(node,	TEXT("centerScreen"),	FALSE);
	m_bContextHelp	= xmlGetAttributeValueBOOL(node,	TEXT("contextHelp"),	FALSE);
	m_bClipChildren	= xmlGetAttributeValueBOOL(node,	TEXT("clipChildren"),	FALSE);
	m_bClipSiblings	= xmlGetAttributeValueBOOL(node,	TEXT("clipSiblings"),	FALSE);
	m_bMinimizeBox	= xmlGetAttributeValueBOOL(node,	TEXT("minimizeBox"),	FALSE);
	m_bMaximizeBox	= xmlGetAttributeValueBOOL(node,	TEXT("maximizeBox"),	FALSE);
	m_bSysMenu		= xmlGetAttributeValueBOOL(node,	TEXT("sysMenu"),		TRUE);
	m_border		= xmlGetAttributeValueSTRArray(node, TEXT("border"), XUI_DLG_BORDER_DIALOGFRAME, L"none\0thin\0resizing\0dialogFrame\0");
	m_style = WS_VISIBLE | DS_SETFONT | WS_POPUP;

	if(m_bClipChildren)	m_style |= WS_CLIPCHILDREN;
	if(m_bClipSiblings)	m_style |= WS_CLIPSIBLINGS;
	if(m_bMaximizeBox)	m_style |= WS_MAXIMIZEBOX;
	if(m_bMinimizeBox)	m_style |= WS_MINIMIZEBOX;
	if(m_bSysMenu)		m_style |= WS_SYSMENU;

	switch(m_border)
	{
	case XUI_DLG_BORDER_THIN:
		m_style |= WS_CAPTION;
		break;
	case XUI_DLG_BORDER_RESIZING:
		m_style |= WS_THICKFRAME | WS_CAPTION;
		break;
	case XUI_DLG_BORDER_DIALOGFRAME:
		m_style |= DS_MODALFRAME | WS_CAPTION;
		break;
	}
	if(m_bContextHelp)
	{
		m_style |= DS_CONTEXTHELP;
	}

	IXMLDOMNode* ctxHelp = xmlOpenNode(node, L"contexthelp");
	if(ctxHelp)
	{
		m_contextHelp.loadDATA(ctxHelp);
		ctxHelp->Release();
	}

	return TRUE;
}

LRESULT CALLBACK CXUIDialog::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	CXUIDialog *pThis = (CXUIDialog*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if(pThis || uMessage == WM_INITDIALOG)
	{
		if(pThis)
		{
			LRESULT retVal = TRUE;
			if(pThis->PreprocessMessage(hWnd, uMessage, wParam, lParam, retVal))
			{
				return retVal;
			}
		}

		switch (uMessage)
		{
		case WM_SIZING:
			if(pThis->m_border == XUI_DLG_BORDER_RESIZING)
			{
				LPRECT newRect = (LPRECT) lParam;
				if(newRect->right - newRect->left < pThis->m_minWindowWidth)
				{
					switch(wParam)
					{
					case WMSZ_BOTTOMLEFT:
					case WMSZ_TOPLEFT:
						newRect->left = newRect->right - pThis->m_minWindowWidth;
						break;
					default:
						newRect->right = newRect->left + pThis->m_minWindowWidth;
						break;
					}
				}
				if(newRect->bottom - newRect->top < pThis->m_minWindowHeight)
				{
					switch(wParam)
					{
					case WMSZ_TOPRIGHT:
					case WMSZ_TOPLEFT:
						newRect->top = newRect->bottom - pThis->m_minWindowHeight;
						break;
					default:
						newRect->bottom = newRect->top + pThis->m_minWindowHeight;
						break;
					}
				}
			}
			break;
		case WM_SIZE:
			if(pThis->m_border == XUI_DLG_BORDER_RESIZING)
			{
				RECT rcClient;
				GetClientRect(pThis->m_hWnd, &rcClient);
				pThis->render(rcClient.left, rcClient.top, 
					rcClient.right - rcClient.left, 
					rcClient.bottom - rcClient.top);
			}
			break;
		case DM_GETDEFID:
			{
				if(pThis->m_defButton)
				{
					SetWindowLongPtr(hWnd, DWLP_MSGRESULT, MAKELONG(pThis->m_defButton->get_dlgid(), DC_HASDEFID));
					return TRUE;
				} else
				{
					CXUIElement* el = pThis->getDefaultControl();
					if(el)
					{
						SetWindowLongPtr(hWnd, DWLP_MSGRESULT, MAKELONG(el->get_dlgid(), DC_HASDEFID));
						return TRUE;
					}
				}
			}
			break;
		case WM_HELP:
			{
				LPHELPINFO hinf = (LPHELPINFO) lParam;
				CXUIElement* el = pThis->find((HWND) hinf->hItemHandle);
				if(el)
				{
					pThis->m_contextHelp.show(el->get_id(), (HWND) hinf->hItemHandle);
				}
			}
			return 0;
		case WM_DRAWITEM:
			{
				if(wParam)
				{
					CXUIElement* el = pThis->find((UINT) wParam);
					if(el)
					{
						el->raiseEvent(XUI_EVENT_DRAWITEM, wParam, lParam);
					}
				}
			}
			return 0;
		case WM_MEASUREITEM:
			{
				if(wParam)
				{
					CXUIElement* el = pThis->find((UINT) wParam);
					if(el)
					{
						el->raiseEvent(XUI_EVENT_MEASUREITEM, wParam, lParam);
					}
				}
			}
			return 0;
		case WM_HSCROLL:
			{
				HWND hCtl	= (HWND) lParam;
				CXUIElement* el = pThis->find(hCtl);
				if(el)
				{
					return el->raiseEvent(XUI_EVENT_HSCROLL, wParam, NULL);
				}
			}
			break;
		case WM_VSCROLL:
			{
				HWND hCtl	= (HWND) lParam;
				CXUIElement* el = pThis->find(hCtl);
				if(el)
				{
					return el->raiseEvent(XUI_EVENT_VSCROLL, wParam, NULL);
				}
			}
			break;
		case WM_NOTIFY:
			{
				LPNMHDR pnmh = (LPNMHDR) lParam;
				CXUIElement* el = pThis->find(pnmh->hwndFrom);
				BOOL processed = FALSE;
				if(el)
				{
					processed = el->onNotify((int) wParam, (LPNMHDR) lParam);
				}
				if(!processed)
				{
					if(!pThis->ProcessNotify(WM_NOTIFY, el, wParam, lParam))
					{
						pThis->OnNotify(el, (int) wParam, (LPNMHDR) lParam);
					}
				}
			}
			return TRUE;
		case WM_COMMAND:
			{
				if(LOWORD(wParam) == IDCANCEL)
				{
					if(pThis->OnEndDialog(IDCANCEL))
					{
						EndDialog(pThis->m_hWnd, IDCANCEL);
					}
				} else if(LOWORD(wParam) == IDOK)
				{
					if(pThis->OnEndDialog(IDOK))
					{
						EndDialog(pThis->m_hWnd, IDOK);
					}
				} else
				{
					HWND hCtl	= (HWND) lParam;
					CXUIElement* el = pThis->find(hCtl);
					BOOL processed = FALSE;
					if(el)
					{
						processed = el->onCommnd(HIWORD(wParam), LOWORD(wParam), hCtl);
					}
					if(!processed)
					{
						if(!pThis->ProcessNotify(WM_COMMAND, el, wParam, lParam))
						{
							pThis->OnCommand(el, HIWORD(wParam), LOWORD(wParam), (HWND) lParam);
						}
					}
				}
			}
			return TRUE;
		case WM_DESTROY:
			{
				RECT rcWindow;
				GetWindowRect(hWnd, &rcWindow);
				pThis->saveSizes(rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top);
			}
			break;
		case WM_CLOSE:
			{
				if(pThis->OnEndDialog(0))
				{
					EndDialog(pThis->m_hWnd, IDCANCEL);
				}
			}
			return TRUE;
		case WM_INITDIALOG:
			{
				pThis = (CXUIDialog*)(lParam);
				SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

				//set the window handle
				pThis->m_hWnd = hWnd;
				pThis->OnInitDialog();
			}
			return TRUE;
		}
	}

	return FALSE;
}

typedef HANDLE (WINAPI *MonitorFromWindowFNC)(HWND hwnd, DWORD dwFlags);
typedef BOOL (WINAPI *GetMonitorInfoFNC)(HANDLE hMonitor, LPMONITORINFO lpmi);

void CXUIDialog::GetDesktopRect(RECT* rcDsk, HWND hWnd)
{
	int nMonitors = GetSystemMetrics(80);

	HWND dskWnd = GetDesktopWindow();
	GetClientRect(dskWnd, rcDsk);

	HMODULE hUser32 = LoadLibrary(TEXT("user32.dll"));
	MonitorFromWindowFNC getMonitor = (MonitorFromWindowFNC) GetProcAddress(hUser32, "MonitorFromWindow");
	if(getMonitor)
	{
		HANDLE hMonitor = (*getMonitor)(hWnd, 0x00000002);
		if(hMonitor)
		{
			GetMonitorInfoFNC getMonitorInfo = (GetMonitorInfoFNC) GetProcAddress(hUser32, "GetMonitorInfoA");
			MONITORINFO mInf;
			mInf.cbSize = sizeof(MONITORINFO);
			(*getMonitorInfo)(hMonitor, &mInf);
			*rcDsk = mInf.rcWork;
		}
	}
	FreeLibrary(hUser32);
}

void CXUIDialog::OnInitDialog()
{
	bindControls();

	RECT rcMap = { 4, 4, 7, 7};
	MapDialogRect(m_hWnd, &rcMap);
	m_cellSpaceX	= rcMap.left;
	m_cellSpaceY	= rcMap.top;
	m_marginLeft	= rcMap.right;
	m_marginRight	= rcMap.right;
	m_marginTop		= rcMap.bottom;
	m_marginBottom	= rcMap.bottom;

	RECT rcDlgSize = {0};
	rcDlgSize.right		= m_width;
	rcDlgSize.bottom	= m_height;
	MapDialogRect(m_hWnd, &rcDlgSize);

	Init();
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	SIZE sz = {0, 0};

	getMinSize(sz);
	if(m_width)		sz.cx = rcDlgSize.right;
	if(m_height)	sz.cy = rcDlgSize.bottom;
	render(rcClient.left, rcClient.top, sz.cx, sz.cy);

	getMinSize(sz);
	if(m_width)		sz.cx = rcDlgSize.right;
	if(m_height)	sz.cy = rcDlgSize.bottom;
	render(rcClient.left, rcClient.top, sz.cx, sz.cy);

	RECT rcWindow;
	GetWindowRect(m_hWnd, &rcWindow);

	int X		= 0;
	int Y		= 0;
	int width	= sz.cx + ((rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left));
	int height	= sz.cy + ((rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top));

	m_minWindowWidth	= width;
	m_minWindowHeight	= height;

	width	= getSavedWidth(width);
	height	= getSavedHeight(height);

	RECT rcParent;
	GetWindowRect(GetParent(m_hWnd), &rcParent);
	X = rcParent.left;
	Y = rcParent.top;

	RECT rcDesk;
	GetDesktopRect(&rcDesk, GetParent(m_hWnd));
	if(!m_bCenterScreen)
	{
		if(X + width > rcDesk.right)
		{
			X = rcDesk.right - width;
		}
		if(Y + height > rcDesk.bottom)
		{
			Y = rcDesk.bottom - height;
		}

		if(X < rcDesk.left)
		{
			X = rcDesk.left;
		}
		if(Y < rcDesk.top)
		{
			Y = rcDesk.top;
		}
	} else
	{
		X = rcDesk.left + (rcDesk.right - rcDesk.left) / 2 - width / 2;
		Y = rcDesk.top + (rcDesk.bottom - rcDesk.top) / 2 - height / 2;
	}

	SetWindowPos(m_hWnd, NULL, X, Y, 
		width,
		height,
		SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}

BOOL CXUIDialog::OnEndDialog(UINT code)
{
	return TRUE;
}

void CXUIDialog::OnNotify( CXUIElement* el, int idCtrl, LPNMHDR pnmh )
{
}

BOOL CXUIDialog::OnCommand( CXUIElement* el, UINT code, UINT id, HWND hWnd )
{
	return FALSE;
}

void CXUIDialog::dataExchange( BOOL bSave, CXUIElement* parent )
{
}

void CXUIDialog::loadMapItem( DATA_MAP_ITEM* item, CXUIElement* parent )
{
	CXUIElement* el = NULL;
	if(parent)
	{
		el = parent->find(item->id);
	} else
	{
		el = find(item->id);
	}
	if(el)
	{
		switch(item->type)
		{
		case BIND_TYPE_INT:
			if(item->intVal)
			{
				if(!item->isChoice)
				{
					el->value_INT(*(item->intVal));
				} else
				{
					if(*(item->intVal) == item->intTrueVal)
					{
						el->value_INT(TRUE);
					} else
					{
						el->value_INT(FALSE);
					}
				}
			}
			break;
		case BIND_TYPE_WORD:
			if(item->wordVal)
			{
				if(!item->isChoice)
				{
					el->value_INT(*(item->wordVal));
				} else
				{
					if(*(item->wordVal) == item->intTrueVal)
					{
						el->value_INT(TRUE);
					} else
					{
						el->value_INT(FALSE);
					}
				}
			}
			break;
		case BIND_TYPE_PSTR:
			if(item->pstrVal && *(item->pstrVal))
			{
				el->value_STR(*(item->pstrVal));
			}
			break;
		case BIND_TYPE_STR:
			if(item->strVal)
			{
				el->value_STR(item->strVal);
			}
			break;
		}
	}
}

void CXUIDialog::saveMapItem( DATA_MAP_ITEM* item )
{
	CXUIElement* el = find(item->id);
	if(el && el->isInitialized())
	{
		switch(item->type)
		{
		case BIND_TYPE_INT:
			if(item->intVal)
			{
				if(!item->isChoice)
				{
					*item->intVal = el->value_INT();
				} else
				{
					if(el->value_INT())
					{
						*item->intVal = item->intTrueVal;
					} else
					{
						*item->intVal = item->intFalseVal;
					}
				}
			}
			break;
		case BIND_TYPE_WORD:
			if(item->wordVal)
			{
				if(!item->isChoice)
				{
					*item->wordVal = (WORD) el->value_INT();
				} else
				{
					if(el->value_INT())
					{
						*item->wordVal = (WORD) item->intTrueVal;
					} else
					{
						*item->wordVal = (WORD) item->intFalseVal;
					}
				}
			}
			break;
		case BIND_TYPE_PSTR:
			if(item->pstrVal)
			{
				if(*item->pstrVal)
				{
					delete *item->pstrVal;
					*item->pstrVal = NULL;
				}
				LPCWSTR val = el->value_STR();
				if(val)
				{
					*item->pstrVal = new WCHAR[lstrlen(val) + 1];
					lstrcpy(*item->pstrVal, val);
				}
			}
			break;
		case BIND_TYPE_STR:
			if(item->strVal)
			{
				LPCWSTR val = el->value_STR();
				if(val)
				{
					lstrcpy(item->strVal, val);
				} else
				{
					lstrcpy(item->strVal, L"");
				}
			}
			break;
		}
	}
}

void CXUIDialog::bindControls()
{

}

BOOL CXUIDialog::ProcessNotify(UINT uMsg, CXUIElement* el, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

UINT CXUIDialog::get_newDlgID()
{
	return m_nextDlgID++;
}

CXUIElement* CXUIDialog::getDefaultControl()
{
	return NULL;
}

BOOL CXUIDialog::PreprocessMessage( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam, LRESULT& retVal )
{
	return FALSE;
}

void CXUIDialog::set_defButton( LPCWSTR btnID )
{
	if(m_defButton)
	{
		m_defButton->set_default(FALSE);
		m_defButton = NULL;
	}
	CXUIElement* el = find((LPTSTR) btnID);
	if(el)
	{
		el->QueryElement(L"button", (LPVOID*) &m_defButton);
		if(m_defButton)
		{
			m_defButton->set_default(TRUE);
			SendMessage(m_hWnd, DM_SETDEFID, (LPARAM) m_defButton->get_dlgid(), NULL);
		}
	}
}

int CXUIDialog::getSavedWidth( int defWidth )
{
	return defWidth;
}

int CXUIDialog::getSavedHeight( int defHeight )
{
	return defHeight;
}

void CXUIDialog::saveSizes( int width, int height )
{

}