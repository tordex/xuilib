#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>
#include <commctrl.h>

CXUISpin::CXUISpin(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
}

CXUISpin::~CXUISpin(void)
{
}

BOOL CXUISpin::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;

	m_buddyAlign	=	xmlGetAttributeValueSTRArray(node,	TEXT("buddyalign"), 1, L"left\0right\0");
	m_upper			=	xmlGetAttributeValueNonSTR<int>(node,	TEXT("upper"),		0x7FFFFFFF);
	m_lower			=	xmlGetAttributeValueNonSTR<int>(node,	TEXT("lower"),		0);
	m_pos			=	xmlGetAttributeValueNonSTR<int>(node,	TEXT("pos"),		m_lower);
	return TRUE;
}

void CXUISpin::Init()
{
	DWORD wStyle = WS_CHILD | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_NOTHOUSANDS;
	switch(m_buddyAlign)
	{
	case XUI_SPIN_ALIGN_LEFT:
		wStyle |= UDS_ALIGNLEFT ;
		break;
	case XUI_SPIN_ALIGN_RIGHT:
		wStyle |= UDS_ALIGNRIGHT;
		break;
	}
	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	RECT rcDlg = {11, 14, 0, 0};
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minWidth = rcDlg.left;
	m_minHeight = rcDlg.top;

//	m_hWnd = CreateUpDownControl(wStyle, m_left, m_top, m_minWidth, m_minHeight, m_parent->get_parentWnd(), NULL, GetModuleHandle(NULL), buddyWND, m_upper, m_lower, m_pos);
	m_hWnd = CreateWindowEx(0, UPDOWN_CLASS, L"", wStyle, m_left, m_top, m_minWidth, m_minHeight, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
	SendMessage(m_hWnd, UDM_SETRANGE32, (WPARAM) m_lower, (LPARAM) m_upper);
	SendMessage(m_hWnd, UDM_SETPOS32, 0, (LPARAM) m_pos);

}


void CXUISpin::render( int x, int y, int width, int height )
{
	if(m_hWnd && m_parent)
	{
		SendMessage(m_hWnd, UDM_SETBUDDY, (WPARAM) NULL, NULL);

		HWND buddyWND = NULL;
		CXUITextBox* txt = NULL;
		if(m_parent->QueryElement(L"textbox", (LPVOID*) &txt))
		{
			buddyWND = txt->get_wnd();
		}
		SetWindowPos(m_hWnd, NULL, 0, 0, width, height, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);
		SendMessage(m_hWnd, UDM_SETBUDDY, (WPARAM) buddyWND, NULL);
	}
}
