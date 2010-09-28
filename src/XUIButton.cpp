#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>

CXUIButton::CXUIButton(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_label = NULL;
	m_dlgType = 0;
}

CXUIButton::~CXUIButton(void)
{
	if(m_label) delete m_label;
}

BOOL CXUIButton::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_label) delete m_label;
	m_label = getLocalizedString(node, TEXT("label"));

	m_dlgType	=	xmlGetAttributeValueSTRArray(node,	TEXT("dlgType"), 0, L"button\0accept\0cancel\0help\0disclosure\0");
	m_bDefault	=	xmlGetAttributeValueBOOL(node, L"default", FALSE);
	return TRUE;
}

void CXUIButton::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP;
	switch(m_dlgType)
	{
	case XUI_BTN_DGLTYPE_ACCEPT:
		wStyle |= BS_DEFPUSHBUTTON;
		m_id = IDOK;
		break;
	case XUI_BTN_DGLTYPE_CANCEL:
		m_id = IDCANCEL;
		break;
	}
	if(m_bDefault)		wStyle |= BS_DEFPUSHBUTTON;
	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	m_hWnd = CreateWindowEx(0, TEXT("BUTTON"), m_label, wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
	SetWindowFont(m_hWnd, getFont(), TRUE);
	HDC hdc = GetDC(m_hWnd);
	HFONT oldFont = (HFONT) SelectObject(hdc, getFont());
	RECT rcDraw = {0, 0, 3, 3};
	if(m_label)
	{
		DrawText(hdc, m_label, -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
	} else
	{
		DrawText(hdc, L"W", -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
	}
	m_minWidth = rcDraw.right - rcDraw.left + 12;

	RECT rcDlg = {0, 0, 50, 14};
	if(m_width) rcDlg.right = m_width;
	if(m_height) rcDlg.bottom = m_height;
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	if(m_minWidth < rcDlg.right || m_width) m_minWidth = rcDlg.right;
	m_minHeight = rcDlg.bottom;

	SelectObject(hdc, oldFont);
	ReleaseDC(m_hWnd, hdc);
}

BOOL CXUIButton::onCommnd( UINT code, UINT id, HWND hWnd )
{
	if(code == BN_CLICKED)
	{
		processDefaultAction();
		return raiseEvent(XUI_EVENT_CLICKED, 0, NULL);
	}
	return FALSE;
}

void CXUIButton::set_default( BOOL isDefault )
{
	if(isDefault)
	{
		SendMessage(m_hWnd, BM_SETSTYLE, (WPARAM) BS_DEFPUSHBUTTON, (LPARAM) TRUE);
	} else
	{
		SendMessage(m_hWnd, BM_SETSTYLE, (WPARAM) 0, (LPARAM) TRUE);
	}
}

void CXUIButton::value_STR( LPCWSTR val )
{
	if(val)
	{
		SetWindowText(m_hWnd, val);
	} else
	{
		SetWindowText(m_hWnd, TEXT(""));
	}
}