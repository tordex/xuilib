#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>

CXUILabel::CXUILabel(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_value = NULL;
}

CXUILabel::~CXUILabel(void)
{
	if(m_value) delete m_value;
}

BOOL CXUILabel::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_value) delete m_value;
	m_value			= getLocalizedString(node,				TEXT("value"));
	m_bVCenter		= xmlGetAttributeValueBOOL(node,		TEXT("vcenter"),		TRUE);
	m_bMultiline	= xmlGetAttributeValueBOOL(node,		TEXT("multiline"),		FALSE);
	m_textAlign		= xmlGetAttributeValueSTRArray(node,	TEXT("textalign"), XUI_LABEL_ALIGN_LEFT, L"left\0right\0center\0");

	if(m_bMultiline)
	{
		m_bVCenter = FALSE;
	}

	return TRUE;
}

void CXUILabel::Init()
{
	DWORD wStyle = WS_CHILD;
	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	if(m_bVCenter)		wStyle |= SS_CENTERIMAGE;
	switch(m_textAlign)
	{
	case XUI_LABEL_ALIGN_RIGHT:
		wStyle |= SS_RIGHT;
		break;
	case XUI_LABEL_ALIGN_CENTER:
		wStyle |= SS_CENTER;
		break;
	case XUI_LABEL_ALIGN_LEFT:
		wStyle |= SS_LEFT;
		break;
	}

	m_hWnd = CreateWindowEx(0, TEXT("STATIC"), m_value, wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
	SetWindowFont(m_hWnd, getFont(), TRUE);
	HDC hdc = GetDC(m_hWnd);
	HFONT oldFont = (HFONT) SelectObject(hdc, getFont());
	if(!m_bMultiline)
	{
		if(!m_width)
		{
			RECT rcDraw = {0, 0, 3, 3};
			if(m_value)
			{
				DrawText(hdc, m_value, -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
			} else
			{
				DrawText(hdc, L"W", -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
			}
			m_minWidth = rcDraw.right - rcDraw.left;

			RECT rcDlg = {0, 0, 1, 8};
			if(m_height) rcDlg.bottom = m_height;
			MapDialogRect(m_parent->get_parentWnd(), &rcDlg);

			m_minHeight = max(rcDraw.bottom - rcDraw.top, rcDlg.bottom);
		} else
		{
			RECT rcDlg = {0, 0, m_width, 8};
			if(m_height) rcDlg.bottom = m_height;
			MapDialogRect(m_parent->get_parentWnd(), &rcDlg);

			RECT rcDraw = {0, 0, rcDlg.right, rcDlg.bottom};
			if(m_value)
			{
				DrawText(hdc, m_value, -1, &rcDraw, DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL);
			} else
			{
				DrawText(hdc, L"W", -1, &rcDraw, DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL);
			}
			m_minWidth  = rcDlg.right;
			m_minHeight = max(rcDraw.bottom - rcDraw.top, rcDlg.bottom);
		}
	} else
	{
		RECT rcDlg = {0, 0, 7, 7};
		if(m_width)		rcDlg.right		= m_width;
		if(m_height)	rcDlg.bottom	= m_height;
		MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
		m_minWidth  = rcDlg.right;
		m_minHeight = rcDlg.bottom;
	}
	SelectObject(hdc, oldFont);
	ReleaseDC(m_hWnd, hdc);
}

LPCWSTR CXUILabel::value_STR()
{
	return m_value;
}

void CXUILabel::value_STR( LPCWSTR val )
{
	if(m_value) delete m_value;
	m_value = NULL;
	if(val)
	{
		m_value = new WCHAR[lstrlen(val) + 1];
		lstrcpy(m_value, val);
	}
	if(m_value)
	{
		SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM) m_value);
	} else
	{
		SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM) TEXT(""));
	}
}

void CXUILabel::recalcHeight( int width )
{
	if(m_bMultiline)
	{
		RECT rcDlg = {0, 0, 7, 7};
/*
		if(m_width)		rcDlg.right		= m_width;
		if(m_height)	rcDlg.bottom	= m_height;
*/
		MapDialogRect(m_parent->get_parentWnd(), &rcDlg);

		m_minWidth = max(m_minWidth, max(rcDlg.right, width));

		HDC hdc = GetDC(m_hWnd);
		HFONT oldFont = (HFONT) SelectObject(hdc, getFont());

		RECT rcDraw = {0, 0, m_minWidth, 3};
		if(m_value)
		{
			DrawText(hdc, m_value, -1, &rcDraw, DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL);
		} else
		{
			DrawText(hdc, L"W", -1, &rcDraw, DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL);
		}
		m_minHeight = max(m_minHeight, max(rcDlg.bottom, rcDraw.bottom - rcDraw.top));

		SelectObject(hdc, oldFont);
		ReleaseDC(m_hWnd, hdc);
	}
}
