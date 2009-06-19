#include "xuilib.h"
#include "XUIChecksList.h"
#include "xmltools.h"
#include <CommCtrl.h>
#include <WindowsX.h>

CXUIChecksList::CXUIChecksList(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_lockChanged = FALSE;
}

CXUIChecksList::~CXUIChecksList(void)
{
}

void CXUIChecksList::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_NOCOLUMNHEADER | LVS_SHOWSELALWAYS;

	if(get_disabled())			wStyle |= WS_DISABLED;
	if(!get_hidden())			wStyle |= WS_VISIBLE;

	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, TEXT(""), wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
	SetWindowFont(m_hWnd, GetStockObject(DEFAULT_GUI_FONT), TRUE);

	ListView_SetExtendedListViewStyle(m_hWnd, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

	LVCOLUMN lvc;
	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = 0;
	ListView_InsertColumn(m_hWnd, 0, &lvc);

	m_minWidth = 0;
	m_minHeight = 0;

	int count = 0;
	for(int i=0; i < m_childCount; i++)
	{
		CXUICheckItem* check = NULL;
		if(m_childs[i]->QueryElement(L"checkitem", (LPVOID*) &check))
		{
			LVITEM lvi;
			ZeroMemory(&lvi, sizeof(lvi));

			lvi.mask = LVIF_TEXT | LVIF_PARAM;
			lvi.pszText = check->get_label();
			lvi.lParam	= (LPARAM) check->get_value();
			lvi.iItem	= i;
			count++;
			ListView_InsertItem(m_hWnd, &lvi);
		}
	}
	ListView_SetColumnWidth(m_hWnd, 0, LVSCW_AUTOSIZE);
	m_minWidth = ListView_GetColumnWidth(m_hWnd, 0);

	int idx = m_visibleItems - 1;
	if(idx >= count)
	{
		idx = count - 1;
	}
	RECT rcItem;
	ZeroMemory(&rcItem, sizeof(rcItem));
	ListView_GetItemRect(m_hWnd, idx, &rcItem, LVIR_BOUNDS);

	m_minHeight = rcItem.bottom + (rcItem.bottom - rcItem.top) / 2;

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
		RECT rcDlg = {0, 0, 14, 14};
		MapDialogRect(m_parent->get_parentWnd(), &rcDlg);

		m_minWidth	+= GetSystemMetrics(SM_CXVSCROLL) + rcDlg.right;
	}
}

BOOL CXUIChecksList::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	m_visibleItems = xmlGetAttributeValueNonSTR<int>(node,	TEXT("visibleItems"),	10);
	return TRUE;
}

void CXUIChecksList::render( int x, int y, int width, int height )
{
	if(m_hWnd && m_parent)
	{
		SetWindowPos(m_hWnd, NULL, x, y, width, height, SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
}

int CXUIChecksList::getItemIndex( int value )
{
	int count = ListView_GetItemCount(m_hWnd);
	for(int i=0; i < count; i++)
	{
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask	= LVIF_PARAM;
		lvi.iItem	= i;
		ListView_GetItem(m_hWnd, &lvi);
		int val = (int) lvi.lParam;
		if(val == value)
		{
			return i;
		}
	}
	return -1;
}

void CXUIChecksList::checkItem( int value, BOOL bChecked )
{
	m_lockChanged = TRUE;
	int count = ListView_GetItemCount(m_hWnd);
	for(int i=0; i < count; i++)
	{
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask	= LVIF_PARAM;
		lvi.iItem	= i;
		ListView_GetItem(m_hWnd, &lvi);
		int val = (int) lvi.lParam;
		if(val == value)
		{
			ListView_SetCheckState(m_hWnd, i, bChecked);
		}
	}
	m_lockChanged = FALSE;
}

void CXUIChecksList::setItemIndex( int value, int idx )
{
	int count = ListView_GetItemCount(m_hWnd);
	for(int i=0; i < count; i++)
	{
		TCHAR txt[255];
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask		= LVIF_PARAM | LVIF_TEXT | LVIF_STATE;
		lvi.iItem		= i;
		lvi.stateMask	= (UINT) -1;
		lvi.cchTextMax	= 255;
		lvi.pszText		= txt;
		ListView_GetItem(m_hWnd, &lvi);
		int val = (int) lvi.lParam;
		if(val == value)
		{
			int checkState = ListView_GetCheckState(m_hWnd, i);
			ListView_DeleteItem(m_hWnd, i);
			lvi.iItem = idx;
//			if(idx > i) lvi.iItem--;
			if(lvi.iItem < 0) lvi.iItem = 0;
			int newIDX = ListView_InsertItem(m_hWnd, &lvi);
			ListView_SetCheckState(m_hWnd, newIDX, checkState);
			return;
		}
	}
}

BOOL CXUIChecksList::isItemChecked( int value )
{
	int count = ListView_GetItemCount(m_hWnd);
	for(int i=0; i < count; i++)
	{
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask	= LVIF_PARAM;
		lvi.iItem	= i;
		ListView_GetItem(m_hWnd, &lvi);
		int val = (int) lvi.lParam;
		if(val == value)
		{
			return ListView_GetCheckState(m_hWnd, i);
		}
	}
	return 0;
}

int CXUIChecksList::getCount()
{
	return ListView_GetItemCount(m_hWnd);
}

int CXUIChecksList::getItemValue( int idx )
{
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask	= LVIF_PARAM;
	lvi.iItem	= idx;
	ListView_GetItem(m_hWnd, &lvi);
	int val = (int) lvi.lParam;
	return val;
}

int CXUIChecksList::getSelected()
{
	int count = ListView_GetItemCount(m_hWnd);
	for(int i=0; i < count; i++)
	{
		LVITEM lvi;
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask		= LVIF_PARAM | LVIF_STATE;
		lvi.iItem		= i;
		lvi.stateMask	= (UINT)-1;
		ListView_GetItem(m_hWnd, &lvi);
		int val = (int) lvi.lParam;
		if(lvi.state & LVIS_SELECTED)
		{
			return val;
		}
	}
	return -1;
}

void CXUIChecksList::addItem( LPWSTR text, int data )
{
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(lvi));

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = text;
	lvi.lParam	= (LPARAM) data;
	lvi.iItem	= getCount() + 1;
	m_lockChanged = TRUE;
	ListView_InsertItem(m_hWnd, &lvi);
	m_lockChanged = FALSE;
}

void CXUIChecksList::clearItems()
{
	ListView_DeleteAllItems(m_hWnd);
}

BOOL CXUIChecksList::onNotify( int idCtrl, LPNMHDR pnmh )
{
	switch(pnmh->code)
	{
	case LVN_ITEMCHANGED:
		if(!m_lockChanged)
		{
			LPNMLISTVIEW pnmv = (LPNMLISTVIEW) pnmh;
			BOOL oldCheckState = FALSE;
			BOOL newCheckState = FALSE;
			if(((pnmv->uOldState & LVIS_STATEIMAGEMASK) >> 12) - 1)
			{
				oldCheckState = TRUE;
			}
			if(((pnmv->uNewState & LVIS_STATEIMAGEMASK) >> 12) - 1)
			{
				newCheckState = TRUE;
			}
			if(oldCheckState != newCheckState)
			{
				return raiseEvent(XUI_EVENT_CHANGED, NULL, NULL);
			}
		}
		break;
	}
	return FALSE;
}
