#include "xuilib.h"
#include "XUIStatusBar.h"
#include "xmltools.h"

CXUIStatusBar::CXUIStatusBar(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{

}

CXUIStatusBar::~CXUIStatusBar(void)
{
}

BOOL CXUIStatusBar::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;

	m_bSizeGrip	= xmlGetAttributeValueBOOL(node, L"sizeGrip",	FALSE);
	m_hFlex = 1;

	return TRUE;
}

void CXUIStatusBar::Init()
{
	DWORD dwStyle = WS_CHILD | CCS_BOTTOM | WS_VISIBLE;

	if(m_bSizeGrip)	dwStyle |= SBARS_SIZEGRIP;

	m_hWnd = CreateStatusWindow(dwStyle, TEXT(""), m_parent->get_parentWnd(), m_id); 

	if(m_childCount)
	{
		int count = 0;
		for(int i=0; i < m_childCount; i++)
		{
			CXUISBPart* btn = NULL;
			if(m_childs[i]->QueryElement(L"sbpart", (LPVOID*) &btn))
			{
				btn->setPartIDX(count);
				count++;
			}
		}
	}

	RECT rcSb;
	GetWindowRect(m_hWnd, &rcSb);
	m_minHeight = rcSb.bottom - rcSb.top;
}

void CXUIStatusBar::render( int x, int y, int width, int height )
{
	m_left		= x;
	m_top		= y;
	m_width		= width;
	m_height	= height;
	if(m_hWnd)
	{
		SetWindowPos(m_hWnd, NULL, x, y, width, height, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOCOPYBITS | SWP_DEFERERASE);
	}

	if(m_childCount)
	{
		LPINT parts = new INT[m_childCount];
		LPINT flex = new INT[m_childCount];
		LPINT size = new INT[m_childCount];
		int count = 0;
		int totalFlex = 0;
		for(int i=0; i < m_childCount; i++)
		{
			CXUISBPart* btn = NULL;
			if(m_childs[i]->QueryElement(L"sbpart", (LPVOID*) &btn))
			{
				flex[count] = btn->getFlex();
				size[count] = btn->getSize();
				totalFlex += flex[count];
				count++;
			}
		}
		if(count)
		{
			int left = 0;
			for(int i=0; i < count; i++)
			{
				if(flex[i])
				{
					size[i] = (int) (((double) flex[i] / (double) totalFlex) * (double) width);
				}
				parts[i] = left + size[i];
				left += size[i];
			}
		}

		SendMessage(m_hWnd, SB_SETPARTS, count, (LPARAM) parts);

		delete parts;
		delete flex;
		delete size;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CXUISBPart::CXUISBPart( CXUIElement* parent, CXUIEngine* engine ) : CXUIElement(parent, engine)
{
	m_partIDX = 0;
}

CXUISBPart::~CXUISBPart( void )
{

}

BOOL CXUISBPart::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	m_flex = xmlGetAttributeValueNonSTR<INT>(node, L"flex", 0);
	m_size = xmlGetAttributeValueNonSTR<INT>(node, L"size", 0);
	return TRUE;
}

void CXUISBPart::Init()
{

}

void CXUISBPart::value_STR( LPCWSTR val )
{
	if(m_parent)
	{
		SendMessage(m_parent->get_wnd(), SB_SETTEXT, m_partIDX, (LPARAM) val);
	}
}

