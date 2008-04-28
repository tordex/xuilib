#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>

CXUICheckbox::CXUICheckbox(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_label = NULL;
}

CXUICheckbox::~CXUICheckbox(void)
{
	if(m_label) delete m_label;
}

BOOL CXUICheckbox::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_label) delete m_label;
	m_label = getLocalizedString(node, TEXT("label"));

	m_checked   =	xmlGetAttributeValueBOOL(node,	TEXT("checked"),		FALSE);
	m_bAuto		=	xmlGetAttributeValueBOOL(node,	TEXT("auto"),			TRUE);
	m_bTriState   =	xmlGetAttributeValueBOOL(node,	TEXT("triState"),		FALSE);
	return TRUE;
}

void CXUICheckbox::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP;
	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;
	if(m_bAuto)
	{
		if(m_bTriState)
		{
			wStyle |= BS_AUTO3STATE;
		} else
		{
			wStyle |= BS_AUTOCHECKBOX;
		}
	} else
	{
		if(m_bTriState)
		{
			wStyle |= BS_3STATE;
		} else
		{
			wStyle |= BS_CHECKBOX;
		}
	}

	m_hWnd = CreateWindowEx(0, TEXT("BUTTON"), m_label, wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
	SetWindowFont(m_hWnd, GetStockObject(DEFAULT_GUI_FONT), TRUE);
	HDC hdc = GetDC(m_hWnd);
	HFONT oldFont = (HFONT) SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
	RECT rcDraw = {0, 0, 3, 3};
	if(m_label)
	{
		DrawText(hdc, m_label, -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
	} else
	{
		DrawText(hdc, L"W", -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
	}
	RECT rcDlg = {0, 0, 1, 10};
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minHeight = rcDlg.bottom;
	m_minWidth = rcDraw.right - rcDraw.left + m_minHeight + rcDlg.right;
	SelectObject(hdc, oldFont);
	ReleaseDC(m_hWnd, hdc);
}

INT CXUICheckbox::value_INT()
{
	if(m_hWnd)
	{
		return (INT) SendMessage(m_hWnd, BM_GETCHECK, 0, NULL);
	}
	return 0;
}

void CXUICheckbox::value_INT( INT val )
{
	SendMessage(m_hWnd, BM_SETCHECK, (WPARAM) val, NULL);
	processDefaultAction();
}

BOOL CXUICheckbox::onCommnd( UINT code, UINT id, HWND hWnd )
{
	if(code == BN_CLICKED)
	{
		processDefaultAction();
		return raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
	}
	return FALSE;
}