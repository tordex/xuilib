#include "xuilib.h"
#include "XUIList.h"
#include <CommCtrl.h>
#include "xmltools.h"
#include <WindowsX.h>

CXUIList::CXUIList(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
}

CXUIList::~CXUIList(void)
{
}

void CXUIList::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP;

	if(get_disabled())			wStyle |= WS_DISABLED;
	if(!get_hidden())			wStyle |= WS_VISIBLE;
	if(m_bAutoArrange)			wStyle |= LVS_AUTOARRANGE;
	if(m_bNoColHeader)			wStyle |= LVS_NOCOLUMNHEADER;
	if(m_bAlwaysShowSelection)	wStyle |= LVS_SHOWSELALWAYS;
	if(m_bEditLabels)			wStyle |= LVS_EDITLABELS;
	if(m_bNoSortHeader)			wStyle |= LVS_NOSORTHEADER;
	if(m_bNoLabelWrap)			wStyle |= LVS_NOLABELWRAP;
	if(m_bNoScroll)				wStyle |= LVS_NOSCROLL;
	if(m_bSingleSelection)		wStyle |= LVS_SINGLESEL;

	switch(m_view)
	{
	case XUI_LIST_VIEW_ICON:
		wStyle |= LVS_ICON;
		break;
	case XUI_LIST_VIEW_SMALLICON:
		wStyle |= LVS_SMALLICON;
		break;
	case XUI_LIST_VIEW_LIST:
		wStyle |= LVS_LIST;
		break;
	case XUI_LIST_VIEW_REPORT:
		wStyle |= LVS_REPORT;
		break;
	}

	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, TEXT(""), wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
	SetWindowFont(m_hWnd, GetStockObject(DEFAULT_GUI_FONT), TRUE);

	DWORD exStyle = 0;
	if(m_bCheckBoxes)		exStyle |= LVS_EX_CHECKBOXES;
	if(m_bFullRowSelect)	exStyle |= LVS_EX_FULLROWSELECT;
	if(m_bGridLines)		exStyle |= LVS_EX_GRIDLINES;
	if(m_bHeaderDragDrop)	exStyle |= LVS_EX_HEADERDRAGDROP;
	if(m_bLabelTip)			exStyle |= LVS_EX_LABELTIP;

	ListView_SetExtendedListViewStyle(m_hWnd, exStyle);

	m_minWidth = 0;
	m_minHeight = 0;
	m_bAutoSizeCols = FALSE;

	if(m_childCount)
	{
		for(int i=0; i < m_childCount; i++)
		{
			CXUIListCol* col = NULL;
			if(m_childs[i]->QueryElement(L"listcol", (LPVOID*) &col))
			{
				LVCOLUMN lvc;
				ZeroMemory(&lvc, sizeof(lvc));
				lvc.mask = 0;
				if(col->get_caption())
				{
					lvc.mask |= LVCF_TEXT;
					lvc.pszText = col->get_caption();
				}
				if(col->get_size())
				{
					lvc.mask |= LVCF_WIDTH;
					lvc.cx = col->get_size();
				}
				ListView_InsertColumn(m_hWnd, i, &lvc);
				if(col->get_flex())
				{
					m_bAutoSizeCols = TRUE;
				}
			}
		}



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
}

BOOL CXUIList::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	m_view					= xmlGetAttributeValueSTRArray(node,	TEXT("view"), XUI_LIST_VIEW_ICON, L"icon\0smallicon\0list\0report\0");
	m_bNoColHeader			= xmlGetAttributeValueBOOL(node,		TEXT("NoColHeader"),			FALSE);
	m_bAutoArrange			= xmlGetAttributeValueBOOL(node,		TEXT("AutoArrange"),			FALSE);
	m_bAlwaysShowSelection	= xmlGetAttributeValueBOOL(node,		TEXT("AlwaysShowSelection"),	TRUE);
	m_bEditLabels			= xmlGetAttributeValueBOOL(node,		TEXT("EditLabels"),				FALSE);
	m_bNoSortHeader			= xmlGetAttributeValueBOOL(node,		TEXT("NoSortHeader"),			TRUE);
	m_bNoLabelWrap			= xmlGetAttributeValueBOOL(node,		TEXT("NoLabelWrap"),			TRUE);
	m_bNoScroll				= xmlGetAttributeValueBOOL(node,		TEXT("NoScroll"),				FALSE);
	m_bSingleSelection		= xmlGetAttributeValueBOOL(node,		TEXT("SingleSelection"),		TRUE);

	m_bCheckBoxes			= xmlGetAttributeValueBOOL(node,		TEXT("CheckBoxes"),				FALSE);
	m_bFullRowSelect		= xmlGetAttributeValueBOOL(node,		TEXT("FullRowSelect"),			FALSE);
	m_bGridLines			= xmlGetAttributeValueBOOL(node,		TEXT("GridLines"),				FALSE);
	m_bHeaderDragDrop		= xmlGetAttributeValueBOOL(node,		TEXT("HeaderDragDrop"),			FALSE);
	m_bLabelTip				= xmlGetAttributeValueBOOL(node,		TEXT("LabelTip"),				FALSE);

	return TRUE;
}

void CXUIList::render( int x, int y, int width, int height )
{
	if(m_hWnd && m_parent)
	{
		SetWindowPos(m_hWnd, NULL, x, y, width, height, SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
	if(m_bAutoSizeCols)
	{
		RECT rcClient = {0};
		GetClientRect(m_hWnd, &rcClient);
		int lstWidth = rcClient.right - rcClient.left - GetSystemMetrics(SM_CXVSCROLL);

		struct col_info
		{
			int width;
			int flex;
		};
		col_info* colsWidth = new col_info[m_childCount];
		int colsCount = 0;

		int nonFlexWidth = 0;
		int flexCount = 0;

		for(int i=0; i < m_childCount; i++)
		{
			CXUIListCol* col = NULL;
			if(m_childs[i]->QueryElement(L"listcol", (LPVOID*) &col))
			{
				colsWidth[colsCount].width = col->get_size();
				colsWidth[colsCount].flex  = col->get_flex();
				if(!col->get_flex())
				{
					nonFlexWidth += col->get_size();
				}
				flexCount += col->get_flex();
				colsCount++; 
			}
		}

		for(int i=0; i < colsCount; i++)
		{
			if(colsWidth[i].flex && flexCount)
			{
				colsWidth[i].width = (lstWidth - nonFlexWidth) * colsWidth[i].flex / flexCount;
			}
			ListView_SetColumnWidth(m_hWnd, i, colsWidth[i].width);
		}
	}
}

int CXUIList::insertItem(LPWSTR text, LPARAM data, int iconID)
{
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.pszText		= text;
	lvi.lParam		= data;
	lvi.iItem		= ListView_GetItemCount(m_hWnd);
	lvi.iSubItem	= 0;
	if(iconID >= 0)
	{
		lvi.mask |= LVIF_IMAGE;
		lvi.iImage = iconID;
	}
	return ListView_InsertItem(m_hWnd, &lvi);
}

void CXUIList::setItemText(int iItem, int iSubitem, LPWSTR text)
{
	ListView_SetItemText(m_hWnd, iItem, iSubitem, text);
}

void CXUIList::selectItem( int idx )
{
	ListView_SetItemState(m_hWnd, idx, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

void CXUIList::clearItems()
{
	ListView_DeleteAllItems(m_hWnd);
}

int CXUIList::getItemsCount()
{
	return ListView_GetItemCount(m_hWnd);
}

BOOL CXUIList::isSelected( int idx )
{
	if(ListView_GetItemState(m_hWnd, idx, LVIS_SELECTED) == LVIS_SELECTED)
	{
		return TRUE;
	}
	return FALSE;
}

LPARAM CXUIList::getSelected()
{
	int count = getItemsCount();
	for(int i=0; i < count; i++)
	{
		if(isSelected(i))
		{
			return getItemData(i);
		}
	}
	return (LPARAM) -1;
}

LPARAM CXUIList::getItemData( int idx )
{
	LVITEM lvi = {0};
	lvi.mask	= LVIF_PARAM;
	lvi.iItem	= idx;
	if(ListView_GetItem(m_hWnd, &lvi))
	{
		return lvi.lParam;
	}
	return (LPARAM)	-1;
}

BOOL CXUIList::onNotify( int idCtrl, LPNMHDR pnmh )
{
	switch(pnmh->code)
	{
	case NM_RCLICK:
		return raiseEvent(XUI_EVENT_LST_RCLICK, NULL, (LPARAM) pnmh);
	case NM_CLICK:
		return raiseEvent(XUI_EVENT_LST_CLICK, NULL, (LPARAM) pnmh);
	case LVN_ITEMCHANGED:
		return raiseEvent(XUI_EVENT_LST_ITEMCHANGED, NULL, (LPARAM) pnmh);
	case LVN_ITEMACTIVATE :
		return raiseEvent(XUI_EVENT_LST_ITEMACTIVATE, NULL, (LPARAM) pnmh);
	case LVN_COLUMNCLICK:
		return raiseEvent(XUI_EVENT_LST_COLUMNCLICK, NULL, (LPARAM) pnmh);
	case NM_DBLCLK:
		return raiseEvent(XUI_EVENT_LST_DBLCLK, NULL, (LPARAM) pnmh);
	}
	return FALSE;
}

INT CXUIList::getSelectedIndex()
{
	int count = getItemsCount();
	for(int i=0; i < count; i++)
	{
		if(isSelected(i))
		{
			return i;
		}
	}
	return -1;
}

void CXUIList::deleteItemByIndex( int idx )
{
	ListView_DeleteItem(m_hWnd, idx);
}

void CXUIList::setImageList( HIMAGELIST himl, int iImageList )
{
	ListView_SetImageList(m_hWnd, himl, iImageList);
}

void CXUIList::getItemText( int iItem, int iSubitem, LPWSTR text, int cbText )
{
	ListView_GetItemText(m_hWnd, iItem, iSubitem, text, cbText);
}