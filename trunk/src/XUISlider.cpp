#include "xuilib.h"
#include "XUISlider.h"
#include "xmltools.h"
#include <CommCtrl.h>
#include <WindowsX.h>

CXUISlider::CXUISlider(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_bAutoTicks	= FALSE;
	m_bNoTicks		= FALSE;
	m_minValue		= 0;
	m_maxValue		= 100;
	m_pos			= 0;
	m_ticFreq		= 0;
	m_type			= XUI_SLIDER_TYPE_HORIZONTAL;
}

CXUISlider::~CXUISlider(void)
{
}

void CXUISlider::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP;

	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;
	if(m_bAutoTicks)	wStyle |= TBS_AUTOTICKS;
	if(m_bNoTicks)		wStyle |= TBS_NOTICKS;
	if(m_type == XUI_SLIDER_TYPE_HORIZONTAL)
	{
		wStyle |= TBS_HORZ;
	} else
	{
		wStyle |= TBS_VERT;
	}

	m_hWnd = CreateWindowEx(0, TRACKBAR_CLASS, TEXT(""), wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
	SetWindowFont(m_hWnd, GetStockObject(DEFAULT_GUI_FONT), TRUE);

	SendMessage(m_hWnd, TBM_SETRANGEMIN, FALSE, (LPARAM) m_minValue);
	SendMessage(m_hWnd, TBM_SETRANGEMAX, TRUE, (LPARAM) m_maxValue);

	RECT rcDlg = {0, 0, 100, 14};
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minWidth = rcDlg.right;
	m_minHeight = rcDlg.bottom;
}

BOOL CXUISlider::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	m_type			= xmlGetAttributeValueSTRArray(node,	TEXT("type"),		XUI_SLIDER_TYPE_HORIZONTAL, L"horizontal\0vertical\0");
	m_bAutoTicks	= xmlGetAttributeValueBOOL(node,		TEXT("autoTics"),	FALSE);
	m_bNoTicks		= xmlGetAttributeValueBOOL(node,		TEXT("noTics"),		FALSE);
	m_bInvert		= xmlGetAttributeValueBOOL(node,		TEXT("invert"),		FALSE);
	m_minValue		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("minValue"),	0);
	m_maxValue		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("maxValue"),	100);
	m_pos			= xmlGetAttributeValueNonSTR<int>(node,	TEXT("pos"),		100);
	m_ticFreq		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("ticFreq"),	0);
	return TRUE;
}

INT CXUISlider::value_INT()
{
	if(!m_bInvert)
	{
		return (INT) SendMessage(m_hWnd, TBM_GETPOS, 0, 0);
	}
	return m_maxValue - (INT) SendMessage(m_hWnd, TBM_GETPOS, 0, 0);
}

void CXUISlider::value_INT( INT val )
{
	if(!m_bInvert)
	{
		SendMessage(m_hWnd, TBM_SETPOS, TRUE, (LPARAM) val);
	} else
	{
		SendMessage(m_hWnd, TBM_SETPOS, TRUE, (LPARAM) (m_maxValue - val));
	}
}

