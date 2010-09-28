#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>
#include <commctrl.h>

CXUITabBox::CXUITabBox(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
}

CXUITabBox::~CXUITabBox(void)
{
}

void CXUITabBox::Init()
{
	DWORD wStyle = WS_CHILD | TCS_RAGGEDRIGHT;
	if(!get_hidden()) wStyle |= WS_VISIBLE;

	m_hWnd = CreateWindowEx(0, WC_TABCONTROL, NULL, wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
	SetWindowFont(m_hWnd, getFont(), TRUE);
	m_minWidth = 0;
	m_minHeight = 0;
	for(int i=0; i < get_childsCount(); i++)
	{
		CXUITabPanel* tp = NULL;
		if(get_childIDX(i)->QueryElement(L"tabpanel", (LPVOID*) &tp))
		{
			TCITEM tci;
			ZeroMemory(&tci, sizeof(tci));
			tci.mask = TCIF_PARAM | TCIF_TEXT;
			tci.lParam = (LPARAM) tp;
			tci.pszText = (LPWSTR) tp->get_label();
			TabCtrl_InsertItem(m_hWnd, i, &tci);
			if(!i)
			{
				tp->set_hidden(FALSE);
			} else
			{
				tp->set_hidden(TRUE);
			}
		}
	}

	RECT rcDisplay;
	rcDisplay.left		= 0;
	rcDisplay.top		= 0;
	rcDisplay.right		= 500;
	rcDisplay.bottom	= 500;
	TabCtrl_AdjustRect(m_hWnd, FALSE, &rcDisplay);

	m_marginLeft	= rcDisplay.left;
	m_marginTop		= rcDisplay.top;
	m_marginRight	= 500 - rcDisplay.right;
	m_marginBottom	= 500 - rcDisplay.bottom;

	m_minHeight = 0;
	m_minWidth = 0;

	CXUIElement::Init();
}

HWND CXUITabBox::get_parentWnd()
{
	if(m_parent) return m_parent->get_parentWnd();
	return NULL;
}

void CXUITabBox::selectTab(int tabID)
{
	for(int i=0; i < get_childsCount(); i++)
	{
		if(i != tabID)
		{
			get_childIDX(i)->set_hidden(TRUE);
		}
	}
	get_childIDX(tabID)->set_hidden(FALSE);
}

BOOL CXUITabBox::onNotify(int idCtrl, LPNMHDR pnmh)
{
	if(pnmh->hwndFrom == m_hWnd)
	{
		switch(pnmh->code)
		{
		case TCN_SELCHANGE:
			selectTab(TabCtrl_GetCurSel(m_hWnd));
			break;
		}
	}
	return CXUIElement::onNotify(idCtrl, pnmh);
}

void CXUITabBox::getMinSize( SIZE& minSize )
{
	int width	= 0;
	int height	= 0;
	for(int i=0; i < m_childCount; i++)
	{
		SIZE sz = {0};
		m_childs[i]->getMinSize(sz);
		height = max(height, sz.cy);
		width  = max(width,  sz.cx);
	}
	RECT rcDisplay;
	rcDisplay.left		= 0;
	rcDisplay.top		= 0;
	rcDisplay.right		= width;
	rcDisplay.bottom	= height;
	TabCtrl_AdjustRect(m_hWnd, TRUE, &rcDisplay);
	minSize.cx = rcDisplay.right - rcDisplay.left;
	minSize.cy = rcDisplay.bottom - rcDisplay.top;
}

void CXUITabBox::render( int x, int y, int width, int height )
{
	if(m_hWnd)
	{
		SetWindowPos(m_hWnd, NULL, x, y, width, height, SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
	RECT rcDisplay;
	rcDisplay.left		= x;
	rcDisplay.top		= y;
	rcDisplay.right		= x + width;
	rcDisplay.bottom	= y + height;
	TabCtrl_AdjustRect(m_hWnd, FALSE, &rcDisplay);

	for(int i=0; i < m_childCount; i++)
	{
		m_childs[i]->render(rcDisplay.left, 
			rcDisplay.top, 
			rcDisplay.right - rcDisplay.left, 
			rcDisplay.bottom - rcDisplay.top);
	}
}

void CXUITabBox::value_INT( INT val )
{
	TabCtrl_SetCurSel(m_hWnd, val);
	selectTab(val);
}

INT CXUITabBox::value_INT()
{
	return TabCtrl_GetCurSel(m_hWnd);
}

BOOL CXUITabBox::processAccelerator( WCHAR accChr )
{
	for(int i=0; i < get_childsCount(); i++)
	{
		CXUITabPanel* tp = NULL;
		if(get_childIDX(i)->QueryElement(L"tabpanel", (LPVOID*) &tp))
		{
			LPWSTR txt = (LPWSTR) tp->get_label();
			LPWSTR amp = StrStrI(txt, L"&");
			if(amp)
			{
				if(!ChrCmpI(amp[1], accChr))
				{
					value_INT(i);
					tp->set_TabStopFocus();
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
