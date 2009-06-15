#include "xuilib.h"
#include "XUIList.h"
#include <CommCtrl.h>
#include "xmltools.h"
#include <WindowsX.h>

CXUIList::CXUIList(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_hSmallImages		= NULL;
}

CXUIList::~CXUIList(void)
{
	clearSmallIcons();
}

void CXUIList::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP | LVS_SHAREIMAGELISTS;

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
	if(m_bOwnerDraw)			wStyle |= LVS_OWNERDRAWFIXED;

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

	clearSmallIcons();


	DWORD exStyle = LVS_EX_SUBITEMIMAGES | LVS_EX_DOUBLEBUFFER;
	if(m_bCheckBoxes)		exStyle |= LVS_EX_CHECKBOXES;
	if(m_bFullRowSelect)	exStyle |= LVS_EX_FULLROWSELECT;
	if(m_bGridLines)		exStyle |= LVS_EX_GRIDLINES;
	if(m_bHeaderDragDrop)	exStyle |= LVS_EX_HEADERDRAGDROP;
	if(m_bLabelTip)			exStyle |= LVS_EX_LABELTIP;

	ListView_SetExtendedListViewStyle(m_hWnd, exStyle);

	m_hSmallImages = ImageList_Create(m_smallIconsWidth, m_smallIconsHeight, ILC_COLOR32, 0, 20);
	ListView_SetImageList(m_hWnd, m_hSmallImages, LVSIL_SMALL);

	m_minWidth = 0;
	m_minHeight = 0;
	m_bAutoSizeCols = FALSE;


	if(m_childCount)
	{
		XUI_COL_INFO* cols = new XUI_COL_INFO[m_childCount];
		int* order = new int[m_childCount];
		int colsCount = 0;
		ZeroMemory(cols, sizeof(XUI_COL_INFO) * m_childCount);
		raiseEvent(XUI_EVENT_LST_REQUESTCOLS, (WPARAM) m_childCount, (LPARAM) cols);
		for(int i=0; i < m_childCount; i++)
		{
			CXUIListCol* col = NULL;
			if(m_childs[i]->QueryElement(L"listcol", (LPVOID*) &col))
			{
				XUI_COL_INFO* colInfo = NULL;
				if(col->get_id())
				{
					for(int j=0; j < m_childCount; j++)
					{
						if(!StrCmpI(col->get_id(), cols[j].id))
						{
							colInfo = cols + j;
							break;
						}
					}
				}
				order[colsCount] = colsCount;
				if(colInfo)
				{
					col->setVisible(colInfo->visible);
					if(colInfo->visible)
					{
						col->set_size(colInfo->size);
						order[colsCount] = colInfo->order;
					}
				}
				if(col->isVisible())
				{
					LVCOLUMN lvc;
					ZeroMemory(&lvc, sizeof(lvc));
					col->get_colData(&lvc);

					int colID = ListView_InsertColumn(m_hWnd, i, &lvc);
					col->setColID(colID);

					colsCount++;

					if(col->get_flex())
					{
						m_bAutoSizeCols = TRUE;
					}
				}
			}
		}

		ListView_SetColumnOrderArray(m_hWnd, colsCount, order);
		delete order;

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

	if(m_hSmallImages)	ImageList_Destroy(m_hSmallImages);

	m_view					= xmlGetAttributeValueSTRArray(node,	TEXT("view"), XUI_LIST_VIEW_ICON, L"icon\0smallicon\0list\0report\0");
	m_bNoColHeader			= xmlGetAttributeValueBOOL(node,		TEXT("NoColHeader"),			FALSE);
	m_bAutoArrange			= xmlGetAttributeValueBOOL(node,		TEXT("AutoArrange"),			FALSE);
	m_bAlwaysShowSelection	= xmlGetAttributeValueBOOL(node,		TEXT("AlwaysShowSelection"),	TRUE);
	m_bEditLabels			= xmlGetAttributeValueBOOL(node,		TEXT("EditLabels"),				FALSE);
	m_bNoSortHeader			= xmlGetAttributeValueBOOL(node,		TEXT("NoSortHeader"),			TRUE);
	m_bNoLabelWrap			= xmlGetAttributeValueBOOL(node,		TEXT("NoLabelWrap"),			TRUE);
	m_bNoScroll				= xmlGetAttributeValueBOOL(node,		TEXT("NoScroll"),				FALSE);
	m_bSingleSelection		= xmlGetAttributeValueBOOL(node,		TEXT("SingleSelection"),		TRUE);
	m_bOwnerDraw			= xmlGetAttributeValueBOOL(node,		TEXT("ownerDraw"),				FALSE);

	m_bCheckBoxes			= xmlGetAttributeValueBOOL(node,		TEXT("CheckBoxes"),				FALSE);
	m_bFullRowSelect		= xmlGetAttributeValueBOOL(node,		TEXT("FullRowSelect"),			FALSE);
	m_bGridLines			= xmlGetAttributeValueBOOL(node,		TEXT("GridLines"),				FALSE);
	m_bHeaderDragDrop		= xmlGetAttributeValueBOOL(node,		TEXT("HeaderDragDrop"),			FALSE);
	m_bLabelTip				= xmlGetAttributeValueBOOL(node,		TEXT("LabelTip"),				FALSE);
	m_bEditColumns			= xmlGetAttributeValueBOOL(node,		TEXT("editColumns"),			FALSE);

	m_smallIconsWidth		= xmlGetAttributeValueNonSTR<INT>(node, L"smallIconsWidth",		16);
	m_smallIconsHeight		= xmlGetAttributeValueNonSTR<INT>(node, L"smallIconsHeight",	16);

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
		delete colsWidth;
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
	if(pnmh->hwndFrom == m_hWnd)
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
		case NM_CUSTOMDRAW:
			return raiseEvent(XUI_EVENT_LST_CUSTOMDRAW, NULL, (LPARAM) pnmh);
		case LVN_KEYDOWN:
			return raiseEvent(XUI_EVENT_LST_KEYDOWN, NULL, (LPARAM) pnmh);
		}
	} else
	{
		HWND hHeader = ListView_GetHeader(m_hWnd);
		if(hHeader == pnmh->hwndFrom)
		{
			if(pnmh->code == NM_RCLICK && m_bEditColumns)
			{
				HMENU menu = CreatePopupMenu();

				for(int i=0; i < m_childCount; i++)
				{
					CXUIListCol* col = NULL;
					if(m_childs[i]->QueryElement(L"listcol", (LPVOID*) &col))
					{
						if(col->isVisible())
						{
							AppendMenu(menu, MF_STRING | MF_CHECKED, i + 1, col->get_name());
						} else
						{
							AppendMenu(menu, MF_STRING, i + 1, col->get_name());
						}
					}
				}

				POINT pt;
				GetCursorPos(&pt);
				UINT ret = TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, NULL, m_hWnd, NULL);
				DestroyMenu(menu);
				if(ret > 0)
				{
					assignColumnsSize();
					CXUIListCol* col = NULL;
					m_childs[ret - 1]->QueryElement(L"listcol", (LPVOID*) &col);
					if(col->isVisible())
					{
						col->setVisible(FALSE);
						if(col->get_ColID())
						{
							ListView_DeleteColumn(m_hWnd, col->get_ColID());
							int id = 0;
							for(int i=0; i < m_childCount; i++)
							{
								CXUIListCol* col = NULL;
								if(m_childs[i]->QueryElement(L"listcol", (LPVOID*) &col))
								{
									if(col->isVisible())
									{
										col->setColID(id++);
									}
								}
							}
						} else
						{
							ListView_DeleteColumn(m_hWnd, col->get_ColID() + 1);
							int id = 0;
							for(int i=0; i < m_childCount; i++)
							{
								CXUIListCol* col = NULL;
								if(m_childs[i]->QueryElement(L"listcol", (LPVOID*) &col))
								{
									if(col->isVisible())
									{
										LVCOLUMN lvc;
										ZeroMemory(&lvc, sizeof(lvc));
										col->get_colData(&lvc);
										ListView_SetColumn(m_hWnd, id, &lvc);
										col->setColID(id++);
									}
								}
							}
							raiseEvent(XUI_EVENT_LST_COLSCHANGED, 0, NULL);
						}

					} else
					{
						col->setVisible(TRUE);

						LVCOLUMN lvc;
						ZeroMemory(&lvc, sizeof(lvc));
						col->get_colData(&lvc);
						ListView_InsertColumn(m_hWnd, ret - 1, &lvc);
						int id = 0;
						for(int i=0; i < m_childCount; i++)
						{
							CXUIListCol* col = NULL;
							if(m_childs[i]->QueryElement(L"listcol", (LPVOID*) &col))
							{
								if(col->isVisible())
								{
									LVCOLUMN lvc;
									ZeroMemory(&lvc, sizeof(lvc));
									col->get_colData(&lvc);
									ListView_SetColumn(m_hWnd, id, &lvc);
									col->setColID(id++);
								}
							}
						}
						raiseEvent(XUI_EVENT_LST_COLSCHANGED, 0, NULL);
					}
				}
				return TRUE;
			}
		}
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

void CXUIList::clearSmallIcons()
{
	if(m_hSmallImages)	ImageList_Destroy(m_hSmallImages);
	m_hSmallImages = NULL;
	for(UINT i=0; i < m_smallIcons.size(); i++)
	{
		if(m_smallIcons[i]) delete m_smallIcons[i];
	}
	m_smallIcons.clear();
}

int CXUIList::addIcon( LPCWSTR icon, ICON_TYPE iconType )
{
	if(!icon)
	{
		return -1;
	}
	HIMAGELIST himgAdd = NULL;
	int cx;
	int cy;
	switch(iconType)
	{
	case ICON_TYPE_SMALL:
		{
			for(UINT i=0; i < m_smallIcons.size(); i++)
			{
				if(m_smallIcons[i])
				{
					if(!StrCmpI(icon, m_smallIcons[i]))
					{
						return (int) i;
					}
				}
			}
			himgAdd = m_hSmallImages;
			cx		= m_smallIconsWidth;
			cy		= m_smallIconsHeight;
		}
		break;
	}
	if(himgAdd)
	{
		BITMAPINFOHEADER bmpInfo;
		ZeroMemory(&bmpInfo, sizeof(bmpInfo));
		bmpInfo.biSize				= sizeof(bmpInfo);
		bmpInfo.biWidth				= cx;
		bmpInfo.biHeight			= cy;
		bmpInfo.biPlanes			= 1;
		bmpInfo.biBitCount			= 32;
		bmpInfo.biCompression		= BI_RGB;
		bmpInfo.biSizeImage			= cx * cy * 4;

		HDC hdc = GetDC(NULL);
		LPVOID pBit32;
		HBITMAP bmp = CreateDIBSection(hdc, (LPBITMAPINFO) &bmpInfo, DIB_RGB_COLORS, &pBit32, NULL, 0);
		ZeroMemory(pBit32, cx * cy * 4);

		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP oldBmp = (HBITMAP) SelectObject(memDC, bmp);
		m_engine->DrawImage(memDC, 0, 0, cx, cy, (LPWSTR) icon);
		SelectObject(memDC, oldBmp);
		DeleteDC(memDC);
		ReleaseDC(NULL, hdc);

		int ret = ImageList_Add(m_hSmallImages, bmp, NULL);
		if(ret >= 0)
		{
			LPWSTR str = new WCHAR[lstrlen(icon) + 1];
			lstrcpy(str, icon);
			m_smallIcons.insert(m_smallIcons.end(), str);
		}
		DeleteObject(bmp);
		return ret;
	}
	return -1;
}

int CXUIList::addIcon( HBITMAP bmp, ICON_TYPE iconType )
{
	if(!bmp)
	{
		return -1;
	}
	HIMAGELIST himgAdd = NULL;
	switch(iconType)
	{
	case ICON_TYPE_SMALL:
		himgAdd = m_hSmallImages;
		break;
	}
	if(himgAdd)
	{
		int ret = ImageList_Add(m_hSmallImages, bmp, NULL);
		if(ret >= 0)
		{
			LPWSTR str = NULL;
			m_smallIcons.insert(m_smallIcons.end(), str);
		}
		return ret;
	}
	return -1;
}

void CXUIList::assignColumnsSize()
{
	for(int i=0; i < m_childCount; i++)
	{
		CXUIListCol* col = NULL;
		if(m_childs[i]->QueryElement(L"listcol", (LPVOID*) &col))
		{
			if(col->isVisible())
			{
				col->set_size(ListView_GetColumnWidth(m_hWnd, col->get_ColID()));
			}
		}
	}
}

CXUIListCol* CXUIList::getColByID( int id )
{
	for(int i=0; i < m_childCount; i++)
	{
		CXUIListCol* col = NULL;
		if(m_childs[i]->QueryElement(L"listcol", (LPVOID*) &col))
		{
			if(col->isVisible() && col->get_ColID() == id)
			{
				return col;
			}
		}
	}
	return NULL;
}

void CXUIList::getColsInfo( XUI_COL_INFO* ci )
{
	assignColumnsSize();

	int colsCount = 0;
	for(int i=0; i < m_childCount; i++)
	{
		CXUIListCol* col = NULL;
		if(m_childs[i]->QueryElement(L"listcol", (LPVOID*) &col))
		{
			if(col->isVisible())
			{
				colsCount++;
			}
		}
	}
	int* order = new int[colsCount];
	ListView_GetColumnOrderArray(m_hWnd, colsCount, order);
	for(int i=0; i < m_childCount; i++)
	{
		CXUIListCol* col = NULL;
		if(m_childs[i]->QueryElement(L"listcol", (LPVOID*) &col))
		{
			StringCchCopy(ci[i].id, 255, col->get_id());
			ci[i].visible	= col->isVisible();
			ci[i].size		= col->get_size();
			ci[i].order	= 0;
			if(col->isVisible())
			{
				ci[i].order = order[col->get_ColID()];
			}
		}
	}
	delete order;
}

void CXUIList::setColSort( int colID, BOOL down )
{
	HWND header = ListView_GetHeader(m_hWnd);
	for(int i=0; i < m_childCount; i++)
	{
		CXUIListCol* col = NULL;
		if(m_childs[i]->QueryElement(L"listcol", (LPVOID*) &col))
		{
			if(col->isVisible())
			{
				HDITEM hdi;
				ZeroMemory(&hdi, sizeof(HDITEM));
				hdi.mask = HDI_FORMAT;
				Header_GetItem(header, col->get_ColID(), &hdi);
				hdi.fmt &= ~HDF_SORTDOWN;
				hdi.fmt &= ~HDF_SORTUP;
				if(col->get_ColID() == colID)
				{
					if(down)
					{
						hdi.fmt |= HDF_SORTDOWN;
					} else
					{
						hdi.fmt |= HDF_SORTUP;
					}
				}
				Header_SetItem(header, col->get_ColID(), &hdi);
			}
		}
	}
}
