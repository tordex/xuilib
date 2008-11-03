#include "xuilib.h"
#include "xmltools.h"
#include "RenderData.h"

CXUIElement::CXUIElement(CXUIElement* parent, CXUIEngine* engine)
{
	m_cols			= 0;
	m_rows			= 0;

	m_span			= 1;
	m_lockid		= NULL;
	m_engine		= engine;
	m_id			= 0;
	m_strID			= NULL;
	m_parent		= parent;
	m_childs		= NULL;
	m_childCount	= 0;
	m_hWnd			= NULL;
	m_layX			= 0;
	m_layY			= 0;
	m_layWidth		= 0;
	m_layHeight		= 0;
	m_hFlex			= 0;
	m_vFlex			= 0;

	m_marginLeft	= 0;
	m_marginRight	= 0;
	m_marginTop		= 0;
	m_marginBottom	= 0;

	m_cellSpaceX	= 0;
	m_cellSpaceY	= 0;
}

CXUIElement::~CXUIElement(void)
{
	if(m_lockid)	delete m_lockid;
	if(m_strID)		delete m_strID;
	clearChilds();
}

BOOL CXUIElement::loadDATA(IXMLDOMNode* node)
{
	m_width			= xmlGetAttributeValueNonSTR<int>(node,	TEXT("width"),		0);
	m_height		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("height"),		0);
	m_left			= xmlGetAttributeValueNonSTR<int>(node,	TEXT("left"),		0);
	m_top			= xmlGetAttributeValueNonSTR<int>(node,	TEXT("top"),		0);
	m_hFlex			= xmlGetAttributeValueNonSTR<int>(node,	TEXT("hFlex"),		0);
	m_vFlex			= xmlGetAttributeValueNonSTR<int>(node,	TEXT("vFlex"),		0);
	m_minWidth		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("minwidth"),	0);
	m_maxWidth		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("maxwidth"),	0);
	m_minHeight		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("minheight"),	0);
	m_maxHeight		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("maxheight"),	0);
	m_cols			= xmlGetAttributeValueNonSTR<int>(node,	TEXT("columns"),	0);
	m_rows			= xmlGetAttributeValueNonSTR<int>(node,	TEXT("rows"),		0);
	m_span			= xmlGetAttributeValueNonSTR<int>(node,	TEXT("span"),		1);
	m_hidden		= xmlGetAttributeValueBOOL(node,	TEXT("hidden"),		FALSE);
	m_disabled		= xmlGetAttributeValueBOOL(node,	TEXT("disabled"),	FALSE);
	m_orient		= xmlGetAttributeValueSTRArray(node, TEXT("orient"), XUI_ORIENT_HORIZONTAL, L"horizontal\0vertical\0");
	m_lockid		= xmlGetAttributeSTR(node, TEXT("locid"));
	m_strID			= xmlGetAttributeSTR(node, TEXT("id"));

	m_id = get_newDlgID();

	IXMLDOMNode* child = NULL;
	node->get_firstChild(&child);
	while(child)
	{
		BSTR name = NULL;
		child->get_nodeName(&name);
		if(name)
		{
			CXUIElement* el = m_engine->createXUIElement(name, this);
			if(el)
			{
				el->loadDATA(child);
				addChild(el);
			}
			SysFreeString(name);
		}
		IXMLDOMNode* newChild = NULL;
		child->get_nextSibling(&newChild);
		child->Release();
		child = newChild;
	}	

	return TRUE;
}

void CXUIElement::clearChilds()
{
	for(int i=0; i < m_childCount; i++)
	{
		delete m_childs[i];
	}
	if(m_childs) free(m_childs);
	m_childs = NULL;
	m_childCount = 0;
}

BOOL CXUIElement::loadFile(LPCTSTR fileName, LPCTSTR defNode, HINSTANCE hInst)
{
	BOOL ret = FALSE;
	CComPtr<IXMLDOMDocument> doc;
	doc.CoCreateInstance(CLSID_DOMFreeThreadedDocument, NULL, CLSCTX_INPROC_SERVER);
	doc->put_async(VARIANT_FALSE);
	doc->put_validateOnParse(VARIANT_FALSE);
	doc->put_resolveExternals(VARIANT_FALSE);
	VARIANT_BOOL bSuccess = VARIANT_FALSE;
	if(StrCmpN(fileName, TEXT("res:"), 4))
	{
		if(doc->load(CComVariant(fileName), &bSuccess) == S_OK && bSuccess == VARIANT_TRUE)
		{
			IXMLDOMNode* node = xmlOpenNode(doc, defNode);
			if(node)
			{
				ret = loadDATA(node);
				node->Release();
			}
		}
	} else
	{
		HRSRC hrc = FindResource(hInst, fileName + 4, TEXT("XUILIB"));
		if(hrc)
		{
			int sz = SizeofResource(hInst, hrc);
			HGLOBAL hgrc = LoadResource(hInst, hrc);
			LPWSTR resData = (LPWSTR) LockResource(hgrc);
			LPWSTR xmlText = new WCHAR[sz / 2 + 1];
			memcpy(xmlText, resData, sz);
			xmlText[sz / 2] = 0;
			LPWSTR xml = xmlText;
			if(xml[0] == 0xFEFF)
			{
				xml++;
			}
			if(doc->loadXML(xml, &bSuccess) == S_OK && bSuccess == VARIANT_TRUE)
			{
				IXMLDOMNode* node = xmlOpenNode(doc, defNode);
				if(node)
				{
					ret = loadDATA(node);
					node->Release();
				}
			}
			delete xmlText;
		}
	}
	return ret;
}

void CXUIElement::addChild(CXUIElement* child)
{
	if(!m_childCount)
	{
		m_childs = (CXUIElement**) malloc(sizeof(CXUIElement*));
	} else
	{
		m_childs = (CXUIElement**) realloc(m_childs, sizeof(CXUIElement*) * (m_childCount + 1));
	}
	m_childs[m_childCount] = child;
	m_childCount++;
}

void CXUIElement::Init()
{
	if(m_parent)
	{
		m_cellSpaceX = m_parent->m_cellSpaceX;
		m_cellSpaceY = m_parent->m_cellSpaceY;

		RECT rcDlg = {0, 0, 0, 0};
		rcDlg.right		= m_maxWidth;
		rcDlg.bottom	= m_maxHeight;
		MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
		m_maxHeight = rcDlg.bottom;
		m_maxWidth	= rcDlg.right;
	}

	for(int i=0; i < m_childCount; i++)
	{
		m_childs[i]->Init();
	}
}

HWND CXUIElement::get_parentWnd()
{
	if(m_hWnd) return m_hWnd;
	if(m_parent)
	{
		return m_parent->get_parentWnd();
	}
	return NULL;
}

BOOL CXUIElement::onNotify(int idCtrl, LPNMHDR pnmh)
{
	return FALSE;
}

BOOL CXUIElement::onCommnd(UINT code, UINT id, HWND hWnd)
{
	return FALSE;
}

void CXUIElement::set_hidden(int hide)
{
	m_hidden = hide;
	updateHiddenState();
}

void CXUIElement::getMinSize( SIZE& minSize )
{
	int width	= 0;
	int height	= 0;

	if(m_childCount)
	{
		CRenderData* rd = getRenderData();
		if(rd)
		{
			rd->normalize(m_cellSpaceX, m_cellSpaceY);
			int cols = rd->getCols();
			int rows = rd->getRows();
			for(int i=0; i < cols; i++)
			{
				width += rd->getColWidth(i);
				if(i) width += m_cellSpaceX;
			}
			for(int i=0; i < rows; i++)
			{
				height += rd->getRowHeight(i);
				if(i) height += m_cellSpaceY;
			}
			delete rd;
		}
	}
	minSize.cx = max(m_minWidth, width) + m_marginLeft + m_marginRight;
	minSize.cy = max(m_minHeight, height) + m_marginTop + m_marginBottom;
}

void CXUIElement::render( int x, int y, int width, int height )
{
	if(m_hWnd && m_parent)
	{
		int w = width;
		int h = height;
		if(m_maxWidth > 0 && width > m_maxWidth)
		{
			w = m_maxWidth;
		}
		if(m_maxHeight > 0 && height > m_maxHeight)
		{
			h = m_maxHeight;
		}
		SetWindowPos(m_hWnd, NULL, x, y, w, h, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOCOPYBITS | SWP_DEFERERASE);
	}
	if(m_childCount)
	{
		if(m_orient == XUI_ORIENT_HORIZONTAL)
		{
			CRenderData* rd = getRenderData();
			rd->normalize(m_cellSpaceX, m_cellSpaceY);
			rd->processFlex(width - m_marginLeft - m_marginRight, 
							height - m_marginTop - m_marginBottom,
							m_cellSpaceX, m_cellSpaceY);

			// recalc height
			for(int i=0; i < m_childCount; i++)
			{
				m_childs[i]->recalcHeight(rd->get(i)->cx);
			}
			delete rd;

			rd = getRenderData();
			rd->normalize(m_cellSpaceX, m_cellSpaceY);
			rd->processFlex(width - m_marginLeft - m_marginRight, 
				height - m_marginTop - m_marginBottom,
				m_cellSpaceX, m_cellSpaceY);

			int row = 0;
			int left = x + m_marginLeft;
			int top  = y + m_marginTop;
			for(int i=0; i < m_childCount; i++)
			{
				if(row != rd->get(i)->row)
				{
					left = x + m_marginLeft;
					top += rd->getRowHeight(row) + m_cellSpaceY; 
					row = rd->get(i)->row;
				}
				m_childs[i]->render(left, top, rd->get(i)->cx, rd->get(i)->cy);
				left += rd->get(i)->cx + m_cellSpaceX;
			}
			delete rd;
		} else
		{
			CRenderData* rd = getRenderData();
			rd->normalize(m_cellSpaceX, m_cellSpaceY);
			rd->processFlex(width - m_marginLeft - m_marginRight, 
				height - m_marginTop - m_marginBottom,
				m_cellSpaceX, m_cellSpaceY);

			// recalc height
			for(int i=0; i < m_childCount; i++)
			{
				m_childs[i]->recalcHeight(rd->get(i)->cx);
			}
			delete rd;

			rd = getRenderData();
			rd->normalize(m_cellSpaceX, m_cellSpaceY);
			rd->processFlex(width - m_marginLeft - m_marginRight, 
				height - m_marginTop - m_marginBottom,
				m_cellSpaceX, m_cellSpaceY);

			int col = 0;
			int left = x + m_marginLeft;
			int top  = y + m_marginTop;
			for(int i=0; i < m_childCount; i++)
			{
				if(col != rd->get(i)->col)
				{
					top		= y + m_marginTop;
					left	+= rd->getColWidth(col) + m_cellSpaceX; 
					col		= rd->get(i)->col;
				}

				m_childs[i]->render(left, top, rd->get(i)->cx, rd->get(i)->cy);
				top += rd->get(i)->cy + m_cellSpaceY;
			}
			delete rd;
		}
	}
}

/*
void CXUIElement::RowsColsSize( cell_info* cinf, int cols, cell_info* rinf, int rows )
{
	int col			= 0;
	int row			= 0;
	ZeroMemory(cinf, sizeof(cell_info) * cols);
	ZeroMemory(rinf, sizeof(cell_info) * rows);
	if(m_orient == XUI_ORIENT_HORIZONTAL)
	{
		for(int i=0; i < m_childCount; i++)
		{
			if(!cinf[col].flex) cinf[col].flex = m_childs[i]->m_hFlex;
			if(!rinf[row].flex) rinf[row].flex = m_childs[i]->m_vFlex;

			// new row if the span found
			if(m_childs[i]->m_span && col)
			{
				col = 0;
				row++;
			}
			rinf[row].span = m_childs[i]->m_span;

			SIZE sz;
			sz.cx = 0;
			sz.cy = 0;
			m_childs[i]->getMinSize(sz);

			if(cinf[col].size < sz.cx) cinf[col].size = sz.cx;
			if(rinf[row].size < sz.cy) rinf[row].size = sz.cy;

			col++;
			if(col >= cols || rinf[row].span)
			{
				col = 0;
				row++;
			}
		}
	} else
	{
		for(int i=0; i < m_childCount; i++)
		{
			if(!cinf[col].flex) cinf[col].flex = m_childs[i]->m_hFlex;
			if(!rinf[row].flex) rinf[row].flex = m_childs[i]->m_vFlex;

			SIZE sz;
			sz.cx = 0;
			sz.cy = 0;
			m_childs[i]->getMinSize(sz);

			if(cinf[col].size < sz.cx) cinf[col].size = sz.cx;
			if(rinf[row].size < sz.cy) rinf[row].size = sz.cy;

			row++;
			if(row >= rows)
			{
				row = 0;
				col++;
			}
		}
	}
}
*/

void CXUIElement::set_disabled( int disable )
{
	m_disabled = disable;
	updateDisabledState();
}

/*
void CXUIElement::ProcessFlex( cell_info* cinf, int count, int size, int cellSpace )
{
	int sz = size - (count - 1) * cellSpace;
	int flex = 0;
	for(int i=0; i < count; i++)
	{
		if(!cinf[i].flex)
		{
			sz -= cinf[i].size;
		} else
		{
			flex += cinf[i].flex;
		}
	}
	if(sz > 0 && flex)
	{
		for(int i=0; i < count; i++)
		{
			if(cinf[i].flex)
			{
				cinf[i].size = sz * cinf[i].flex / flex;
			}
		}
	}
}
*/

int CXUIElement::get_disabled()
{
	if(m_disabled || !m_parent) return m_disabled;
	return m_parent->get_disabled();
}

int CXUIElement::get_hidden()
{
	if(m_hidden || !m_parent) return m_hidden;
	return m_parent->get_hidden();
}

void CXUIElement::updateHiddenState()
{
	if(m_hWnd)
	{
		if(get_hidden())
		{
			ShowWindow(m_hWnd, SW_HIDE);
		} else
		{
			ShowWindow(m_hWnd, SW_SHOW);
		}
	}
	for(int i=0; i < m_childCount; i++)
	{
		m_childs[i]->updateHiddenState();
	}
}

void CXUIElement::updateDisabledState()
{
	if(m_hWnd)
	{
		if(get_disabled())
		{
			EnableWindow(m_hWnd, FALSE);
		} else
		{
			EnableWindow(m_hWnd, TRUE);
		}
	}
	for(int i=0; i < m_childCount; i++)
	{
		m_childs[i]->updateDisabledState();
	}
}

LPWSTR CXUIElement::getLocalizedString( IXMLDOMNode* item, LPCTSTR attrName )
{
	if(m_lockid)
	{
		LPCTSTR locStr = m_engine->getString(m_lockid, attrName);
		if(locStr)
		{
			LPWSTR retStr = new WCHAR[lstrlen(locStr) + 1];
			lstrcpy(retStr, locStr);
			return retStr;
		}
	}
	return xmlGetAttributeSTR(item, attrName);
}

INT CXUIElement::value_INT()
{
	return 0;
}

void CXUIElement::value_INT( INT val )
{

}

LPCWSTR CXUIElement::value_STR()
{
	return NULL;
}

void CXUIElement::value_STR( LPCWSTR val )
{

}

void CXUIElement::doDefaultAction( CXUIElement* el )
{

}

CXUIElement* CXUIElement::get_root()
{
	if(m_parent) return m_parent->get_root();
	return this;
}

CXUIElement* CXUIElement::find( HWND hWnd )
{
	if(hWnd == m_hWnd)
	{
		return this;
	}
	for(int i=0; i < m_childCount; i++)
	{
		CXUIElement* el = m_childs[i]->find(hWnd);
		if(el)
		{
			return el;
		}
	}
	return NULL;
}

CXUIElement* CXUIElement::find( LPTSTR id )
{
	if(!StrCmpI(m_strID, id))
	{
		return this;
	}
	for(int i=0; i < m_childCount; i++)
	{
		CXUIElement* el = m_childs[i]->find(id);
		if(el)
		{
			return el;
		}
	}
	return NULL;
}

CXUIElement* CXUIElement::find( UINT id )
{
	if(m_id == id)
	{
		return this;
	}
	for(int i=0; i < m_childCount; i++)
	{
		CXUIElement* el = m_childs[i]->find(id);
		if(el)
		{
			return el;
		}
	}
	return NULL;
}

LPCWSTR CXUIElement::value_STRdef( LPCWSTR defVal )
{
	LPCWSTR ret = value_STR();
	if(!ret)
	{
		ret = defVal;
	}
	return ret;
}

BOOL CXUIElement::raiseEvent( LPCWSTR evID, WPARAM wParam, LPARAM lParam )
{
	return OnEvent(this, evID, wParam, lParam);
}

BOOL CXUIElement::OnEvent( CXUIElement* el, LPCWSTR evID, WPARAM wParam, LPARAM lParam )
{
	if(m_parent)
	{
		return m_parent->OnEvent(el, evID, wParam, lParam);
	}
	return FALSE;
}

BOOL CXUIElement::isInitialized()
{
	if(m_parent)
	{
		return m_parent->isInitialized();
	}
	return TRUE;
}

UINT CXUIElement::get_newDlgID()
{
	if(m_parent)
	{
		return m_parent->get_newDlgID();
	}
	return 0;
}

void CXUIElement::processDefaultAction()
{
	for(int i=0; i < m_childCount; i++)
	{
		m_childs[i]->doDefaultAction(this);
	}
}

void CXUIElement::onCmd( LPWSTR cmd )
{

}

BOOL CXUIElement::set_TabStopFocus()
{
	if(m_hWnd)
	{
		LONG style = (LONG) GetWindowLongPtr(m_hWnd, GWL_STYLE);
		if(style & WS_TABSTOP)
		{
			SetFocus(m_hWnd);
			return TRUE;
		}
	}
	for(int i=0; i < m_childCount; i++)
	{
		if(m_childs[i]->set_TabStopFocus())
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CXUIElement::recalcHeight( int width )
{

}

CRenderData* CXUIElement::getRenderData()
{
	if(!m_childCount) return NULL;

	CRenderData* retData = new CRenderData(m_childCount);

	if(m_orient == XUI_ORIENT_HORIZONTAL)
	{
		int cols = m_cols;
		if(!cols) cols = m_childCount;

		int col = 0;
		int row = 0;

		for(int i=0; i < m_childCount; i++)
		{
			retData->get(i)->col		= col;
			retData->get(i)->row		= row;
			retData->get(i)->colSpan	= m_childs[i]->m_span;
			retData->get(i)->rowSpan	= 1;
			if(col + m_childs[i]->m_span > cols)
			{
				retData->get(i)->colSpan = cols - col;
			}

			retData->get(i)->hFlex	= m_childs[i]->m_hFlex;
			retData->get(i)->vFlex	= m_childs[i]->m_vFlex;

			SIZE sz;
			sz.cx = 0;
			sz.cy = 0;
			m_childs[i]->getMinSize(sz);
			retData->get(i)->cx = sz.cx;
			retData->get(i)->cy = sz.cy;

			col += m_childs[i]->m_span;
			if(col >= cols)
			{
				row++;
				col = 0;
			}
		}
	} else
	{
		int rows = m_rows;
		if(!rows) rows = m_childCount;

		int col = 0;
		int row = 0;

		for(int i=0; i < m_childCount; i++)
		{
			retData->get(i)->col		= col;
			retData->get(i)->row		= row;
			retData->get(i)->rowSpan	= m_childs[i]->m_span;
			retData->get(i)->colSpan	= 1;
			if(row + m_childs[i]->m_span > rows)
			{
				retData->get(i)->rowSpan = rows - row;
			}

			retData->get(i)->hFlex	= m_childs[i]->m_hFlex;
			retData->get(i)->vFlex	= m_childs[i]->m_vFlex;

			SIZE sz;
			sz.cx = 0;
			sz.cy = 0;
			m_childs[i]->getMinSize(sz);
			retData->get(i)->cx = sz.cx;
			retData->get(i)->cy = sz.cy;

			row += m_childs[i]->m_span;
			if(row >= rows)
			{
				col++;
				row = 0;
			}
		}
	}

	return retData;
}
