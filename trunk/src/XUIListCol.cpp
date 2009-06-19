#include "xuilib.h"
#include "XUIListCol.h"
#include "xmltools.h"

CXUIListCol::CXUIListCol(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_caption	= NULL;
	m_name		= NULL;
	m_icon		= NULL;
	m_bVisible	= TRUE;
	m_isSorted	= FALSE;
	m_sortDown	= TRUE;
}

CXUIListCol::~CXUIListCol(void)
{
	if(m_caption)	delete m_caption;
	if(m_name)		delete m_name;
	if(m_icon)		delete m_icon;
}

BOOL CXUIListCol::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_caption)	delete m_caption;
	if(m_name)		delete m_name;
	if(m_icon)		delete m_icon;

	m_caption	= getLocalizedString(node, TEXT("caption"));
	m_name		= getLocalizedString(node, TEXT("name"));
	if(!m_name && m_caption)
	{
		m_name = new WCHAR[lstrlen(m_caption) + 1];
		lstrcpy(m_name, m_caption);
	}
	m_size				= xmlGetAttributeValueNonSTR<int>(node,	TEXT("size"),	0);
	m_flex				= xmlGetAttributeValueNonSTR<int>(node,	TEXT("flex"),	0);
	m_icon				= xmlGetAttributeSTR(node, L"icon");
	m_align				= xmlGetAttributeValueSTRArray(node, TEXT("textalign"), XUI_LISTCOL_ALIGN_LEFT, L"left\0right\0center\0");
	m_colHasImages		= xmlGetAttributeValueBOOL(node, L"colHasImages",		FALSE);
	m_image				= xmlGetAttributeValueBOOL(node, L"image",			FALSE);
	m_bitmapOnRight		= xmlGetAttributeValueBOOL(node, L"bitmapOnRight",	FALSE);
	m_bVisible			= xmlGetAttributeValueBOOL(node, L"visible",		TRUE);

	if(!m_size)
	{
		HDC hdc = GetDC(NULL);
		HFONT oldFont = (HFONT) SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
		LPWSTR testStr = L"W";
		RECT rcDraw = {0, 0, 3, 3};
		DrawText(hdc, testStr, -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
		m_size = (rcDraw.right - rcDraw.left) * m_size;
		SelectObject(hdc, oldFont);
		ReleaseDC(NULL, hdc);
	}

	return TRUE;
}

void CXUIListCol::get_colData( LPLVCOLUMN plvi )
{
	CXUIList* parent = NULL;
	m_parent->QueryElement(L"list", (LPVOID*) &parent);
	if(m_caption)
	{
		plvi->mask		|= LVCF_TEXT;
		plvi->pszText	=  m_caption;
	}
	if(m_size)
	{
		plvi->mask		|= LVCF_WIDTH;
		plvi->cx		=  m_size;
	}
	plvi->mask	|= LVCF_FMT;
	switch(m_align)
	{
	case XUI_LISTCOL_ALIGN_RIGHT:
		plvi->fmt |= LVCFMT_RIGHT;
		break;
	case XUI_LISTCOL_ALIGN_CENTER:
		plvi->fmt |= LVCFMT_CENTER;
		break;
	default:
		plvi->fmt |= LVCFMT_LEFT;
		break;
	}
	if(m_colHasImages)
	{
		plvi->fmt |= LVCFMT_COL_HAS_IMAGES;
	}
	if(m_image)
	{
		plvi->fmt |= LVCFMT_IMAGE;
	}
	if(m_bitmapOnRight)
	{
		plvi->fmt |= LVCFMT_BITMAP_ON_RIGHT;
	}
	if(parent)
	{
		if(m_icon)
		{
			plvi->iImage = parent->addIcon(m_icon, CXUIList::ICON_TYPE_SMALL);
			if(plvi->iImage >= 0)
			{
				plvi->mask	|= LVCF_IMAGE;
			}
		}
	}
}

void CXUIListCol::setSort( BOOL isSorted, BOOL down )
{
	m_isSorted	= isSorted;
	m_sortDown	= down;
}