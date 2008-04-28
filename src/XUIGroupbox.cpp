#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>

CXUIGroupbox::CXUIGroupbox(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_label = NULL;
}

CXUIGroupbox::~CXUIGroupbox(void)
{
	if(m_label) delete m_label;
}

BOOL CXUIGroupbox::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_label) delete m_label;
	m_label = getLocalizedString(node, TEXT("label"));
	return TRUE;
}

void CXUIGroupbox::Init()
{
	CXUIElement::Init();

	DWORD wStyle = WS_CHILD | BS_GROUPBOX;
	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

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
	m_minWidth = rcDraw.right - rcDraw.left + 12;
	m_minHeight = rcDraw.bottom - rcDraw.top + 12;

	RECT rcMargins;
	rcMargins.left = 7;
	rcMargins.right = 8;
	rcMargins.top = 11;
	rcMargins.bottom = 7;
	MapDialogRect(m_parent->get_parentWnd(), &rcMargins);

	m_marginLeft	= rcMargins.left;
	m_marginRight	= rcMargins.right;
	m_marginTop		= rcMargins.top;
	m_marginBottom	= rcMargins.bottom;

	SelectObject(hdc, oldFont);
	ReleaseDC(m_hWnd, hdc);
}

HWND CXUIGroupbox::get_parentWnd()
{
	if(m_parent) return m_parent->get_parentWnd();
	return NULL;
}
