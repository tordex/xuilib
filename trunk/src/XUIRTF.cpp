#include "xuilib.h"
#include "XUIRTF.h"
#include "xmltools.h"
#include <richedit.h>
#include <windowsx.h>

CXUIRTF::CXUIRTF(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_bLockEvents	= FALSE;
	m_value			= NULL;
}

CXUIRTF::~CXUIRTF(void)
{
	if(m_value)
	{
		delete m_value;
	}
}

BOOL CXUIRTF::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;

	m_style = WS_TABSTOP | WS_CHILD;

	m_style	|= xmlGetAttributeValueBOOL(node, L"autoHScroll",		TRUE)	? ES_AUTOHSCROLL			: 0;
	m_style	|= xmlGetAttributeValueBOOL(node, L"autoVScroll",		TRUE)	? ES_AUTOVSCROLL			: 0;

	switch(xmlGetAttributeValueSTRArray(node, TEXT("textalign"), XUI_RTF_ALIGN_LEFT, L"left\0right\0center\0"))
	{
	case XUI_RTF_ALIGN_LEFT:
		m_style |= ES_LEFT;
		break;
	case XUI_RTF_ALIGN_CENTER:
		m_style |= ES_CENTER;
		break;
	case XUI_RTF_ALIGN_RIGHT:
		m_style |= ES_RIGHT;
		break;
	}

	m_style	|= xmlGetAttributeValueBOOL(node, L"multiline",		TRUE)	? ES_MULTILINE					: 0;
	m_style	|= xmlGetAttributeValueBOOL(node, L"hScroll",		TRUE)	? WS_HSCROLL					: 0;
	m_style	|= xmlGetAttributeValueBOOL(node, L"vScroll",		TRUE)	? WS_VSCROLL					: 0;
	m_style	|= xmlGetAttributeValueBOOL(node, L"wantReturn",	TRUE)	? ES_WANTRETURN					: 0;
	m_style	|= xmlGetAttributeValueBOOL(node, L"readonly",		FALSE)	? ES_READONLY					: 0;

	return TRUE;
}

void CXUIRTF::Init()
{
	DWORD wStyle = m_style;

	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, L"", wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);

	SetWindowFont(m_hWnd, getFont(), TRUE);

	CXUIElement::Init();
}

LPCWSTR CXUIRTF::value_STR()
{
	if(m_value) delete m_value;
	m_value = NULL;
	int txtLen = (int) SendMessage(m_hWnd, WM_GETTEXTLENGTH, 0, 0);
	if(txtLen)
	{
		m_value = new TCHAR[txtLen + 1];
		SendMessage(m_hWnd, WM_GETTEXT, (WPARAM) (txtLen + 1), (LPARAM) m_value);
	}
	return m_value;
}

void CXUIRTF::value_STR( LPCWSTR val )
{
	m_bLockEvents = TRUE;
	if(val)
	{
		SetWindowText(m_hWnd, val);
	} else
	{
		SetWindowText(m_hWnd, TEXT(""));
	}
	m_bLockEvents = FALSE;
}

BOOL CXUIRTF::onNotify( int idCtrl, LPNMHDR pnmh )
{
	if(pnmh->code == EN_LINK)
	{
		ENLINK* lnk = (ENLINK*) pnmh;
		if(lnk->msg == WM_LBUTTONDOWN)
		{
			TEXTRANGE range;
			LPTSTR txt = new TCHAR[lnk->chrg.cpMax - lnk->chrg.cpMin + 3];
			range.chrg = lnk->chrg;
			range.lpstrText = txt;
			SendMessage(m_hWnd, EM_GETTEXTRANGE, 0, (LPARAM) &range);
			ShellExecute(m_hWnd, NULL, txt, NULL, NULL, SW_SHOWNORMAL);
			delete txt;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CXUIRTF::onCommnd( UINT code, UINT id, HWND hWnd )
{
	if(code == EN_CHANGE && !m_bLockEvents)
	{
		processDefaultAction();
		return raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
	}
	return FALSE;
}
