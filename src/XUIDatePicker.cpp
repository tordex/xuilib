#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>

CXUIDatePicker::CXUIDatePicker(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
}

CXUIDatePicker::~CXUIDatePicker(void)
{
}

HWND CXUIDatePicker::get_parentWnd()
{
	if(m_parent)
	{
		return m_parent->get_parentWnd();
	}
	return NULL;
}

BOOL CXUIDatePicker::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;

	m_format		=	xmlGetAttributeValueSTRArray(node, TEXT("format"), XUI_DATE_FORMAT_SHORT, L"shortDate\0longDate\0time\0");
	m_showNone		=	xmlGetAttributeValueBOOL(node,	TEXT("showNone"),				VARIANT_FALSE);
	m_useSpin		=	xmlGetAttributeValueBOOL(node,	TEXT("useSpin"),				VARIANT_FALSE);

	return TRUE;
}

void CXUIDatePicker::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP;

	if(m_showNone)	wStyle |= DTS_SHOWNONE;
	if(m_useSpin)	wStyle |= DTS_UPDOWN;
	switch(m_format)
	{
	case XUI_DATE_FORMAT_LONG:
		wStyle |= DTS_LONGDATEFORMAT;
		break;
	case XUI_DATE_FORMAT_TIME:
		wStyle |= DTS_TIMEFORMAT;
		break;
	default:
		wStyle |= DTS_SHORTDATEFORMAT;
		break;
	}

	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	m_hWnd = CreateWindowEx(0, DATETIMEPICK_CLASS, L"", wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);

	RECT rcDlg = {0, 0, 100, 14};
	if(m_width) rcDlg.right = m_width;
	if(m_height) rcDlg.bottom = m_height;
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minWidth	= rcDlg.right;
	m_minHeight = rcDlg.bottom;

	CXUIElement::Init();
}

BOOL CXUIDatePicker::isNone()
{
	SYSTEMTIME tm;
	if(DateTime_GetSystemtime(m_hWnd, &tm) != GDT_VALID)
	{
		return TRUE;
	}
	return FALSE;
}

void CXUIDatePicker::getDate( LPSYSTEMTIME pTm )
{
	DateTime_GetSystemtime(m_hWnd, pTm);
}

void CXUIDatePicker::setDate( LPSYSTEMTIME pTm, BOOL isNone )
{
	if(!isNone)
	{
		DateTime_SetSystemtime(m_hWnd, GDT_VALID, pTm);
	} else
	{
		DateTime_SetSystemtime(m_hWnd, GDT_NONE, pTm);
	}
}

void CXUIDatePicker::setFormat( LPWSTR format )
{
	DateTime_SetFormat(m_hWnd, format);
}