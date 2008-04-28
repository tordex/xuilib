#include "xuilib.h"
#include "XUIFreeDraw.h"

CXUIFreeDraw::CXUIFreeDraw(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	WNDCLASS wc;
	if(!GetClassInfo(engine->get_hInstance(), XUI_FREEDRAW_CLASS, &wc))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style			= CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= (WNDPROC) CXUIFreeDraw::WndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= engine->get_hInstance();
		wc.hIcon			= NULL;
		wc.hCursor			= LoadCursor(engine->get_hInstance(), IDC_ARROW);
		wc.hbrBackground	= (HBRUSH)(COLOR_BTNFACE + 1);
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= XUI_FREEDRAW_CLASS;

		RegisterClass(&wc);
	}
}

CXUIFreeDraw::~CXUIFreeDraw(void)
{
}

LRESULT CALLBACK CXUIFreeDraw::WndProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	CXUIFreeDraw* pThis = NULL;
	if(IsWindow(hWnd))
	{
		pThis = (CXUIFreeDraw*)GetProp(hWnd, TEXT("CXUIFreeDrawThis"));
	}

	MSG msg;
	memset(&msg, 0, sizeof(msg));
	msg.hwnd = hWnd;
	msg.lParam = lParam;
	msg.message = uMessage;
	msg.wParam = wParam;
	LRESULT res = NULL;

	if(pThis || uMessage == WM_CREATE)
	{
		switch (uMessage)
		{
		case WM_DESTROY:
			RemoveProp(hWnd, TEXT("CXUIFreeDrawThis"));
			return 0;
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
				pThis = (CXUIFreeDraw*) lpcs->lpCreateParams;
				SetProp(hWnd, TEXT("CXUIFreeDrawThis"), (HANDLE) pThis);
				pThis->m_hWnd = hWnd;
			}
			break;
		case WM_PAINT:
			pThis->OnPaint();
			return TRUE;
		}
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void CXUIFreeDraw::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hWnd, &ps);
	
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	raiseEvent(XUI_EVENT_FREEDRAW, (WPARAM) hdc, (LPARAM) &rcClient);

	EndPaint(m_hWnd, &ps);
}

void CXUIFreeDraw::Init()
{
	DWORD wStyle = WS_CHILD;

	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	m_hWnd = CreateWindowEx(0, XUI_FREEDRAW_CLASS, TEXT(""), wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), (LPVOID) this);

	RECT rcDlg = {0, 0, 14, 14};
	if(m_width)		rcDlg.right		= m_width;
	if(m_height)	rcDlg.bottom	= m_height;
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minWidth  = rcDlg.right;
	m_minHeight = rcDlg.bottom;
}

void CXUIFreeDraw::redraw( BOOL eraseBg )
{
	InvalidateRect(m_hWnd, NULL, eraseBg);
}