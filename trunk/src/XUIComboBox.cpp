#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>

CXUIComboBox::CXUIComboBox(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_string				= NULL;
	m_autoCompleteTXT[0]	= 0;
}

CXUIComboBox::~CXUIComboBox(void)
{
	if(m_string) delete m_string;
}

BOOL CXUIComboBox::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;

	m_editable		=	xmlGetAttributeValueBOOL(node,			TEXT("editable"),		VARIANT_FALSE);
	m_sorted		=	xmlGetAttributeValueBOOL(node,			TEXT("sorted"),			VARIANT_FALSE);
	m_bHasStrings	=	xmlGetAttributeValueBOOL(node,			TEXT("hasstrings"),		VARIANT_TRUE);
	m_bAutoComplete	=	xmlGetAttributeValueBOOL(node,			TEXT("autoComplete"),	FALSE);
	m_size			=	xmlGetAttributeValueNonSTR<int>(node,	TEXT("size"),			10);
	m_ownerDraw		=	xmlGetAttributeValueSTRArray(node,		TEXT("ownerdraw"), XUI_COMBO_OWNERDRAW_NO, L"no\0fixed\0variable\0");

	return TRUE;
}

void CXUIComboBox::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP | CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_VSCROLL;
	if(!m_editable) 
	{
		wStyle |= CBS_DROPDOWNLIST;
	} else
	{
		wStyle |= CBS_DROPDOWN;
	}
	if(m_sorted)		wStyle |= CBS_SORT;
	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	switch(m_ownerDraw)
	{
	case XUI_COMBO_OWNERDRAW_FIXED:
		wStyle |= CBS_OWNERDRAWFIXED;
		break;
	case XUI_COMBO_OWNERDRAW_VARIABLE:
		wStyle |= CBS_OWNERDRAWVARIABLE;
		break;
	}

	m_hWnd = CreateWindowEx(0, TEXT("COMBOBOX"), NULL, wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
	SetWindowFont(m_hWnd, GetStockObject(DEFAULT_GUI_FONT), TRUE);
	m_minWidth = 0;
	m_minHeight = 0;

	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT) SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
	for(int i=0; i < m_childCount; i++)
	{
		CXUIListItem* cbi = NULL;
		if(m_childs[i]->QueryElement(L"listitem", (LPVOID*) &cbi))
		{
			int idx = (int) SendMessage(m_hWnd, CB_ADDSTRING, NULL, (LPARAM) cbi->get_label());
			SendMessage(m_hWnd, CB_SETITEMDATA, idx, (LPARAM) cbi->get_value());
			
			RECT rcDraw = {0, 0, 3, 3};
			DrawText(hdc, cbi->get_label(), -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
			if(m_minWidth < rcDraw.right) m_minWidth = rcDraw.right;
		}
	}
	SelectObject(hdc, oldFont);
	ReleaseDC(NULL, hdc);
	if(!m_minWidth)
	{
		HDC hdc = GetDC(m_hWnd);
		HFONT oldFont = (HFONT) SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
		RECT rcDraw = {0, 0, 3, 3};
		LPSTR label = new CHAR[m_size + 1];
		memset(label, 'W', m_size);
		label[m_size] = 0;
		DrawTextA(hdc, label, -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
		delete label;
		m_minHeight = rcDraw.bottom - rcDraw.top;
		m_minWidth = rcDraw.right - rcDraw.left;
		SelectObject(hdc, oldFont);
		ReleaseDC(m_hWnd, hdc);
	}
	RECT rcDlg = {0, 0, 1, 14};
	if(m_width)	rcDlg.right = m_width;
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minHeight = rcDlg.bottom;
	if(m_width)
	{
		m_minWidth = rcDlg.right;
	} else
	{
		m_minWidth += 26;
	}
}

INT CXUIComboBox::value_INT()
{
	int idx = (int) SendMessage(m_hWnd, CB_GETCURSEL, 0, 0);
	if(idx >= 0)
	{
		return (INT) SendMessage(m_hWnd, CB_GETITEMDATA, idx, 0);
	}
	return -1;
}

void CXUIComboBox::value_INT( INT val )
{
	int count = (int) SendMessage(m_hWnd, CB_GETCOUNT, 0, NULL);
	for(int i=0; i < count; i++)
	{
		int v = (int) SendMessage(m_hWnd, CB_GETITEMDATA, i, NULL);
		if(v == val)
		{
			SendMessage(m_hWnd, CB_SETCURSEL, i, NULL);
			processDefaultAction();
			return;
		}
	}
	processDefaultAction();
}

LPCWSTR CXUIComboBox::value_STR()
{
	if(m_string) delete m_string;
	m_string = NULL;

	if(m_editable)
	{
		int idx = (int) SendMessage(m_hWnd, CB_GETCURSEL, 0, NULL);
		if(idx >= 0)
		{
			int txtLen = (int) SendMessage(m_hWnd, CB_GETLBTEXTLEN, idx, NULL);
			if(txtLen)
			{
				m_string = new WCHAR[txtLen + 1];
				SendMessage(m_hWnd, CB_GETLBTEXT, idx, (LPARAM) m_string);
			}
		} else
		{
			int txtLen = (int) SendMessage(m_hWnd, WM_GETTEXTLENGTH, 0, 0);
			if(txtLen)
			{
				m_string = new TCHAR[txtLen + 1];
				SendMessage(m_hWnd, WM_GETTEXT, (WPARAM) (txtLen + 1), (LPARAM) m_string);
			}
		}
	} else
	{
		int idx = (int) SendMessage(m_hWnd, CB_GETCURSEL, 0, NULL);
		if(idx >= 0)
		{
			int txtLen = (int) SendMessage(m_hWnd, CB_GETLBTEXTLEN, idx, NULL);
			if(txtLen)
			{
				m_string = new WCHAR[txtLen + 1];
				SendMessage(m_hWnd, CB_GETLBTEXT, idx, (LPARAM) m_string);
			}
		}
	}
	return m_string;
}

void CXUIComboBox::value_STR( LPCWSTR val )
{
	if(m_editable)
	{
		if(m_string) delete m_string;
		m_string = NULL;
		SendMessage(m_hWnd, CB_SETCURSEL, -1, NULL);
		if(val)
		{
			SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM) val);
		} else
		{
			SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM) TEXT(""));
		}
		processDefaultAction();
	} else
	{
		int cnt = (int) SendMessage(m_hWnd, CB_GETCOUNT, 0, NULL);
		for(int i=0; i < cnt; i++)
		{
			int txtLen = (int) SendMessage(m_hWnd, CB_GETLBTEXTLEN, i, NULL);
			if(txtLen)
			{
				LPWSTR txt = (LPWSTR) _alloca((txtLen + 1) * sizeof(WCHAR));
				SendMessage(m_hWnd, CB_GETLBTEXT, i, (LPARAM) txt);
				if(!lstrcmpi(val, txt))
				{
					SendMessage(m_hWnd, CB_SETCURSEL, i, NULL);
					processDefaultAction();
					break;
				}
			}
		}
	}
}

void CXUIComboBox::addItem( int id, LPCWSTR str )
{
	int idx = (int) SendMessage(m_hWnd, CB_ADDSTRING, 0, (LPARAM) str);
	SendMessage(m_hWnd, CB_SETITEMDATA, idx, (LPARAM) id);
}

void CXUIComboBox::clearItems()
{
	SendMessage(m_hWnd, CB_RESETCONTENT, NULL, NULL);
}

void CXUIComboBox::render( int x, int y, int width, int height )
{
	if(m_hWnd && m_parent)
	{
		SetWindowPos(m_hWnd, NULL, x, y, width, height * 5, SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
}

BOOL CXUIComboBox::onCommnd( UINT code, UINT id, HWND hWnd )
{
	if(code == CBN_EDITCHANGE && m_bAutoComplete && m_editable)
	{
		TCHAR txt[255];
		GetWindowText(m_hWnd, txt, 255);
		if(lstrcmpi(txt, m_autoCompleteTXT))
		{
			int newIDX = SendMessage(m_hWnd, CB_FINDSTRING, -1, (LPARAM) txt);
			if(newIDX >= 0)
			{
				SendMessage(m_hWnd, CB_SETCURSEL, newIDX, 0);
				SendMessage(m_hWnd, CB_SETEDITSEL, 0, MAKELONG(lstrlen(txt), -1));
			}
			lstrcpy(m_autoCompleteTXT, txt);
		}
	}

	if(code == CBN_EDITCHANGE || code == CBN_SELCHANGE)
	{
		processDefaultAction();
		return raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
	}
	return FALSE;
}