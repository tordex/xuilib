#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>

CXUIRadio::CXUIRadio(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_label		= NULL;
	m_bGroup	= FALSE;
	m_value		= 0;
}

CXUIRadio::~CXUIRadio(void)
{
	if(m_label) delete m_label;
}

BOOL CXUIRadio::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_label) delete m_label;
	m_label			= getLocalizedString(node, TEXT("label"));
	m_selected		= xmlGetAttributeValueBOOL(node,	TEXT("selected"),		VARIANT_FALSE);
	m_bMultiline	= xmlGetAttributeValueBOOL(node,	TEXT("multiline"),		VARIANT_FALSE);
	m_value			= xmlGetAttributeValueNonSTR<int>(node,	TEXT("value"),	0);
	m_vAlign		= (VALIGN) xmlGetAttributeValueSTRArray(node, TEXT("vAlign"), vaDefault, L"default\0top\0bottom\0center\0");
	m_hAlign		= (HALIGN) xmlGetAttributeValueSTRArray(node, TEXT("hAlign"), haDefault, L"default\0left\0right\0center\0");
	return TRUE;
}

void CXUIRadio::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP | BS_AUTORADIOBUTTON;
	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;
	if(m_bGroup)		wStyle |= WS_GROUP;
	if(m_bMultiline)	wStyle |= BS_MULTILINE;

	switch(m_vAlign)
	{
	case vaTop:
		wStyle |= BS_TOP;
		break;
	case vaBottom:
		wStyle |= BS_BOTTOM;
		break;
	case vaCenter:
		wStyle |= BS_VCENTER;
		break;
	}

	switch(m_hAlign)
	{
	case haLeft:
		wStyle |= BS_LEFT;
		break;
	case haRight:
		wStyle |= BS_RIGHT;
		break;
	case haCenter:
		wStyle |= BS_CENTER;
		break;
	}

	m_hWnd = CreateWindowEx(0, TEXT("BUTTON"), m_label, wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
	SetWindowFont(m_hWnd, GetStockObject(DEFAULT_GUI_FONT), TRUE);
	if(!m_bMultiline)
	{
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
	} else
	{
		RECT rcDlg = {0, 0, 10, 10};
		MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
		m_minHeight = rcDlg.bottom;
		m_minWidth	= m_minHeight + rcDlg.right;
	}
}

INT CXUIRadio::value_INT()
{
	if(m_hWnd)
	{
		return (INT) SendMessage(m_hWnd, BM_GETCHECK, 0, NULL);
	}
	return 0;
}

void CXUIRadio::value_INT( INT val )
{
	SendMessage(m_hWnd, BM_SETCHECK, (WPARAM) val, NULL);
}

BOOL CXUIRadio::onCommnd( UINT code, UINT id, HWND hWnd )
{
	if(code == BN_CLICKED)
	{
		return raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
	}
	return FALSE;
}

void CXUIRadio::recalcHeight( int width )
{
	if(m_bMultiline)
	{
		m_minWidth = width;

		RECT rcDlg = {0, 0, 14, 14};
		MapDialogRect(m_parent->get_parentWnd(), &rcDlg);

		HDC hdc = GetDC(m_hWnd);
		HFONT oldFont = (HFONT) SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

		RECT rcDraw = {0, 0, m_minWidth - rcDlg.right, 3};
		if(m_label)
		{
			DrawText(hdc, m_label, -1, &rcDraw, DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL);
		} else
		{
			DrawText(hdc, L"W", -1, &rcDraw, DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL);
		}
		m_minHeight = rcDraw.bottom - rcDraw.top;

		SelectObject(hdc, oldFont);
		ReleaseDC(m_hWnd, hdc);
	}
}