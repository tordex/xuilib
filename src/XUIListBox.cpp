#include "xuilib.h"
#include "XUIListBox.h"
#include "xmltools.h"
#include <WindowsX.h>

CXUIListBox::CXUIListBox(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_selection				= XUI_LB_SELECTION_SINGLE;
	m_bMulticolumn			= FALSE;
	m_colWidth				= 0;
	m_bSort					= FALSE;
	m_bNoIntegralHeight		= TRUE;
}

CXUIListBox::~CXUIListBox(void)
{
}

BOOL CXUIListBox::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	m_selection			= xmlGetAttributeValueSTRArray(node, TEXT("selection"), XUI_LB_SELECTION_SINGLE, L"single\0multiple\0extended\0none\0");
	m_bMulticolumn		= xmlGetAttributeValueBOOL(node,	TEXT("multicolumn"),		FALSE);
	m_bSort				= xmlGetAttributeValueBOOL(node,	TEXT("sort"),				FALSE);
	m_bNoIntegralHeight	= xmlGetAttributeValueBOOL(node,	TEXT("nointegralheight"),	TRUE);
	m_colWidth			= xmlGetAttributeValueNonSTR<int>(node,	TEXT("colWidth"),	0);
	return TRUE;
}

void CXUIListBox::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP | LBS_NOTIFY;

	if(get_disabled())		wStyle |= WS_DISABLED;
	if(!get_hidden())		wStyle |= WS_VISIBLE;
	if(m_bSort)				wStyle |= LBS_SORT;
	if(m_bNoIntegralHeight)	wStyle |= LBS_NOINTEGRALHEIGHT;

	switch(m_selection)
	{
	case XUI_LB_SELECTION_MULTIPLE:
		wStyle |= LBS_MULTIPLESEL;
		break;
	case XUI_LB_SELECTION_EXTENDED:
		wStyle |= LBS_EXTENDEDSEL;
		break;
	case XUI_LB_SELECTION_NONE:
		wStyle |= LBS_NOSEL;
		break;
	}

	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("ListBox"), TEXT(""), wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
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
			int idx = (int) SendMessage(m_hWnd, LB_ADDSTRING, NULL, (LPARAM) cbi->get_label());
			SendMessage(m_hWnd, LB_SETITEMDATA, idx, (LPARAM) cbi->get_value());

			RECT rcDraw = {0, 0, 3, 3};
			DrawText(hdc, cbi->get_label(), -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
			if(m_minWidth < rcDraw.right) m_minWidth = rcDraw.right;
			m_minHeight += rcDraw.bottom;
		}
	}
	SelectObject(hdc, oldFont);
	ReleaseDC(NULL, hdc);
	if(!m_minWidth)
	{
		RECT rcDlg = {0, 0, 48, 40};
		if(m_width) rcDlg.right = m_width;
		if(m_height) rcDlg.bottom = m_height;
		MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
		m_minWidth = rcDlg.right;
		m_minHeight = rcDlg.bottom;
	} else
	{
		m_minWidth	+= 10;
		m_minHeight += 20;
	}
}

INT CXUIListBox::value_INT()
{
	int idx = (int) SendMessage(m_hWnd, LB_GETCURSEL, 0, 0);
	if(idx >= 0)
	{
		return (INT) SendMessage(m_hWnd, LB_GETITEMDATA, idx, 0);
	}
	return -1;
}

void CXUIListBox::value_INT( INT val )
{
	int count = (int) SendMessage(m_hWnd, LB_GETCOUNT, 0, NULL);
	for(int i=0; i < count; i++)
	{
		int v = (int) SendMessage(m_hWnd, LB_GETITEMDATA, i, NULL);
		if(v == val)
		{
			SendMessage(m_hWnd, LB_SETCURSEL, i, NULL);
			processDefaultAction();
			return;
		}
	}
	processDefaultAction();
}

void CXUIListBox::clearItems()
{
	SendMessage(m_hWnd, LB_RESETCONTENT, NULL, NULL);
}

void CXUIListBox::addItem(int id, LPCWSTR str)
{
	int idx = (int) SendMessage(m_hWnd, LB_ADDSTRING, 0, (LPARAM) str);
	SendMessage(m_hWnd, LB_SETITEMDATA, idx, (LPARAM) id);
}

void CXUIListBox::setItemText( int id, LPCWSTR str )
{
	int count = (int) SendMessage(m_hWnd, LB_GETCOUNT, 0, NULL);
	for(int i=0; i < count; i++)
	{
		int v = (int) SendMessage(m_hWnd, LB_GETITEMDATA, i, NULL);
		if(v == id)
		{
			BOOL bIsSelected = FALSE;
			if(SendMessage(m_hWnd, LB_GETCURSEL, 0, NULL) == i)
			{
				bIsSelected = TRUE;
			}
			SendMessage(m_hWnd, LB_DELETESTRING, i, NULL);
			int idx = (int) SendMessage(m_hWnd, LB_INSERTSTRING, i, (LPARAM) str);
			SendMessage(m_hWnd, LB_SETITEMDATA, idx, (LPARAM) id);
			if(bIsSelected)
			{
				SendMessage(m_hWnd, LB_SETCURSEL, idx, NULL);
			}
			return;
		}
	}
}

BOOL CXUIListBox::onCommnd( UINT code, UINT id, HWND hWnd )
{
	if(code == LBN_SELCHANGE)
	{
		processDefaultAction();
		return raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
	}
	return FALSE;
}

void CXUIListBox::delItem( int id )
{
	int count = (int) SendMessage(m_hWnd, LB_GETCOUNT, 0, NULL);
	for(int i=0; i < count; i++)
	{
		int v = (int) SendMessage(m_hWnd, LB_GETITEMDATA, i, NULL);
		if(v == id)
		{
			SendMessage(m_hWnd, LB_DELETESTRING, i, NULL);
			int count = (int) SendMessage(m_hWnd, LB_GETCOUNT, 0, NULL);
			int idx = i;
			if(idx >= count) idx = count - 1;
			SendMessage(m_hWnd, LB_SETCURSEL, idx, NULL);
			processDefaultAction();
			return;
		}
	}
}
