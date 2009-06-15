#include "xuilib.h"
#include "XUISplitter.h"
#include <WindowsX.h>

CXUIVSplitter::CXUIVSplitter(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	WNDCLASS wc;
	if(!GetClassInfo(engine->get_hInstance(), XUI_VSPLITTER_CLASS, &wc))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style			= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS;
		wc.lpfnWndProc		= (WNDPROC) CXUIVSplitter::WndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= engine->get_hInstance();
		wc.hIcon			= NULL;
		wc.hCursor			= LoadCursor(NULL, IDC_SIZEWE);
		wc.hbrBackground	= NULL;
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= XUI_VSPLITTER_CLASS;

		RegisterClass(&wc);
	}
	m_inCapture = FALSE;
}

CXUIVSplitter::~CXUIVSplitter(void)
{
}

void CXUIVSplitter::Init()
{
	DWORD wStyle = WS_CHILD | WS_VISIBLE;

	m_hWnd = CreateWindowEx(0, XUI_VSPLITTER_CLASS, TEXT(""), wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), (LPVOID) this);

	RECT rcDlg = {0, 0, 7, 7};
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minWidth  = rcDlg.right;
	m_minHeight = rcDlg.bottom;
}

LRESULT CALLBACK CXUIVSplitter::WndProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	CXUIVSplitter* pThis = NULL;
	if(IsWindow(hWnd))
	{
		pThis = (CXUIVSplitter*)GetProp(hWnd, TEXT("vsplitter_this"));
	}

	if(pThis || uMessage == WM_CREATE)
	{
		switch (uMessage)
		{
		case WM_ERASEBKGND:
			return TRUE;
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
				pThis = (CXUIVSplitter*)(lpcs->lpCreateParams);
				SetProp(hWnd, TEXT("vsplitter_this"), (HANDLE) pThis);
				pThis->m_hWnd = hWnd;
			}
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);

				RECT rcClient;
				GetClientRect(hWnd, &rcClient);
				int width	= rcClient.right - rcClient.left;
				int height	= rcClient.bottom - rcClient.top;

				HBITMAP bmp		= CreateCompatibleBitmap(hdc, width, height);
				HDC		memDC	= CreateCompatibleDC(hdc);
				HBITMAP oldBmp	= (HBITMAP) SelectObject(memDC, bmp);

				RECT rcDraw;
				rcDraw.left		= 0;
				rcDraw.right	= width;
				rcDraw.top		= 0;
				rcDraw.bottom	= height;

				pThis->OnPaint(memDC, &rcDraw);

				BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
				SelectObject(memDC, oldBmp);
				DeleteDC(memDC);
				DeleteObject(bmp);

				EndPaint(hWnd, &ps);
			}
			break;
			//return TRUE;
		case WM_LBUTTONDOWN:
			pThis->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_LBUTTONUP:
			pThis->OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_MOUSEMOVE:
			pThis->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return TRUE;
		}
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void CXUIVSplitter::OnPaint( HDC hdc, LPRECT rcDraw )
{
	SendMessage(GetParent(m_hWnd), WM_ERASEBKGND, (WPARAM) hdc, NULL);
}

void CXUIVSplitter::OnLButtonDown( int x, int y )
{
	POINT pos;
	GetCursorPos(&pos);
	SetCapture(m_hWnd);
	m_downX = pos.x;
	m_inCapture = TRUE;
}

void CXUIVSplitter::OnLButtonUp( int x, int y )
{
	ReleaseCapture();
	m_inCapture = FALSE;
}

void CXUIVSplitter::OnMouseMove( int x, int y )
{
	if(m_inCapture)
	{
		POINT pos;
		GetCursorPos(&pos);

		CXUIElement* prevEl = getPrevEl();
		if(prevEl)
		{
			int width = prevEl->get_width();
			width += pos.x - m_downX;
			m_downX = pos.x;
			prevEl->set_width(width);
			m_parent->reRender();
			UpdateWindow(get_root()->get_wnd());
		}
	}
}

void CXUIVSplitter::value_INT( INT val )
{
	CXUIElement* prevEl = getPrevEl();
	if(prevEl)
	{
		prevEl->set_width(val);
		m_parent->reRender();
		UpdateWindow(get_root()->get_wnd());
	}
}

INT CXUIVSplitter::value_INT()
{
	CXUIElement* prevEl = getPrevEl();
	if(prevEl)
	{
		return prevEl->get_width();
	}
	return 10;
}
//////////////////////////////////////////////////////////////////////////


CXUIHSplitter::CXUIHSplitter( CXUIElement* parent, CXUIEngine* engine ) : CXUIElement(parent, engine)
{
	WNDCLASS wc;
	if(!GetClassInfo(engine->get_hInstance(), XUI_HSPLITTER_CLASS, &wc))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style			= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS;
		wc.lpfnWndProc		= (WNDPROC) CXUIHSplitter::WndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= engine->get_hInstance();
		wc.hIcon			= NULL;
		wc.hCursor			= LoadCursor(NULL, IDC_SIZENS);
		wc.hbrBackground	= NULL;
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= XUI_HSPLITTER_CLASS;

		RegisterClass(&wc);
	}
	m_inCapture = FALSE;
}

CXUIHSplitter::~CXUIHSplitter( void )
{

}

void CXUIHSplitter::Init()
{
	DWORD wStyle = WS_CHILD | WS_VISIBLE;

	m_hWnd = CreateWindowEx(0, XUI_HSPLITTER_CLASS, TEXT(""), wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), (LPVOID) this);

	RECT rcDlg = {0, 0, 7, 7};
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minWidth  = rcDlg.right;
	m_minHeight = rcDlg.bottom;
}

LRESULT CALLBACK CXUIHSplitter::WndProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	CXUIHSplitter* pThis = NULL;
	if(IsWindow(hWnd))
	{
		pThis = (CXUIHSplitter*)GetProp(hWnd, TEXT("hsplitter_this"));
	}

	if(pThis || uMessage == WM_CREATE)
	{
		switch (uMessage)
		{
		case WM_ERASEBKGND:
			return TRUE;
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
				pThis = (CXUIHSplitter*)(lpcs->lpCreateParams);
				SetProp(hWnd, TEXT("hsplitter_this"), (HANDLE) pThis);
				pThis->m_hWnd = hWnd;
			}
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);

				RECT rcClient;
				GetClientRect(hWnd, &rcClient);
				int width	= rcClient.right - rcClient.left;
				int height	= rcClient.bottom - rcClient.top;

				HBITMAP bmp		= CreateCompatibleBitmap(hdc, width, height);
				HDC		memDC	= CreateCompatibleDC(hdc);
				HBITMAP oldBmp	= (HBITMAP) SelectObject(memDC, bmp);

				RECT rcDraw;
				rcDraw.left		= 0;
				rcDraw.right	= width;
				rcDraw.top		= 0;
				rcDraw.bottom	= height;

				pThis->OnPaint(memDC, &rcDraw);

				BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
				SelectObject(memDC, oldBmp);
				DeleteDC(memDC);
				DeleteObject(bmp);

				EndPaint(hWnd, &ps);
			}
			break;
			//return TRUE;
		case WM_LBUTTONDOWN:
			pThis->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_LBUTTONUP:
			pThis->OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_MOUSEMOVE:
			pThis->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return TRUE;
		}
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void CXUIHSplitter::OnPaint( HDC hdc, LPRECT rcDraw )
{
	SendMessage(GetParent(m_hWnd), WM_ERASEBKGND, (WPARAM) hdc, NULL);
}

void CXUIHSplitter::OnLButtonDown( int x, int y )
{
	POINT pos;
	GetCursorPos(&pos);
	SetCapture(m_hWnd);
	m_downY = pos.y;
	m_inCapture = TRUE;
}

void CXUIHSplitter::OnLButtonUp( int x, int y )
{
	ReleaseCapture();
	m_inCapture = FALSE;
}

void CXUIHSplitter::OnMouseMove( int x, int y )
{
	if(m_inCapture)
	{
		POINT pos;
		GetCursorPos(&pos);

		CXUIElement* prevEl = getPrevEl();
		if(prevEl)
		{
			int height = prevEl->get_height();
			height += pos.y - m_downY;
			m_downY = pos.y;
			prevEl->set_height(height);
			m_parent->reRender();
			UpdateWindow(get_root()->get_wnd());
		}
	}
}

void CXUIHSplitter::value_INT( INT val )
{
	CXUIElement* prevEl = getPrevEl();
	if(prevEl)
	{
		prevEl->set_height(val);
		m_parent->reRender();
		UpdateWindow(get_root()->get_wnd());
	}
}

INT CXUIHSplitter::value_INT()
{
	CXUIElement* prevEl = getPrevEl();
	if(prevEl)
	{
		return prevEl->get_height();
	}
	return 10;
}
