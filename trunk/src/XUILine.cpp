#include "xuilib.h"
#include "XUILine.h"
#include "xmltools.h"
#include <WindowsX.h>

CXUILine::CXUILine(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
}

CXUILine::~CXUILine(void)
{
}

void CXUILine::Init()
{
	DWORD wStyle = WS_CHILD;
	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	switch(m_type)
	{
	case XUI_LINE_TYPE_ETCHED_HORZ:
		wStyle |= SS_ETCHEDHORZ;
		break;
	case XUI_LINE_TYPE_ETCHED_VERT:
		wStyle |= SS_ETCHEDVERT;
		break;
	}

	m_hWnd = CreateWindowEx(0, TEXT("STATIC"), TEXT(""), wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);

	switch(m_type)
	{
	case XUI_LINE_TYPE_ETCHED_HORZ:
		{
			RECT rcDlg = {0, 0, m_width ? m_width : 14, 2};
			MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
			m_minWidth  = rcDlg.right;
			m_minHeight = rcDlg.bottom;
		}
		break;
	case XUI_LINE_TYPE_ETCHED_VERT:
		{
			RECT rcDlg = {0, 0, 2, m_height ? m_height : 14};
			MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
			m_minWidth  = rcDlg.right;
			m_minHeight = rcDlg.bottom;
		}
		break;
	}
}

BOOL CXUILine::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	m_type	= xmlGetAttributeValueSTRArray(node, TEXT("type"), XUI_LINE_TYPE_ETCHED_HORZ, L"EtchedHorz\0EtchedVert\0");
	return TRUE;
}