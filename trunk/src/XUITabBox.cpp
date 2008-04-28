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
	DWORD wStyle = WS_CHILD | WS_VISIBLE | TCS_SINGLELINE;

	m_hWnd = CreateWindowEx(0, WC_TABCONTROL, NULL, wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
	SetWindowFont(m_hWnd, GetStockObject(DEFAULT_GUI_FONT), TRUE);
	m_minWidth = 0;
	m_minHeight = 0;
	for(int i=0; i < get_childsCount(); i++)
	{
		CXUITabPanel* tp = NULL;
		if(get_childIDX(i)->QueryElement(L"tabpanel", (LPVOID*) &tp))
		{
			TCITEM tci;
			tci.mask = TCIF_PARAM | TCIF_TEXT;
			tci.lParam = (LPARAM) tp;
			tci.pszText = (LPWSTR) tp->get_label();
			TabCtrl_InsertItem(m_hWnd, i, &tci);
		}
	}
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
	get_childIDX(tabID)->set_hidden(FALSE);
	for(int i=0; i < get_childsCount(); i++)
	{
		if(i != tabID)
		{
			get_childIDX(i)->set_hidden(TRUE);
		}
	}
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
