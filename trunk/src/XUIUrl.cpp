#include "xuilib.h"
#include "XUIUrl.h"
#include "xmltools.h"
#include <WindowsX.h>

#define FREE_CLEAR_STR(str) if(str) delete str; str = NULL;
#define MAKE_STR(str, cpstr) FREE_CLEAR_STR(str); if(cpstr) { str = new TCHAR[lstrlen(cpstr) + 1]; lstrcpy(str, cpstr); }

CXUIUrl::CXUIUrl(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	WNDCLASS wc;
	if(!GetClassInfo(m_engine->get_hInstance(), XUI_URL_CLASS, &wc))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc    = (WNDPROC)CXUIUrl::WndProc;
		wc.cbClsExtra     = 0;
		wc.cbWndExtra     = 0;
		wc.hInstance      = m_engine->get_hInstance();
		wc.hIcon          = NULL;
		wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground  = (HBRUSH)(COLOR_BTNFACE + 1);
		wc.lpszMenuName   = NULL;
		wc.lpszClassName  = XUI_URL_CLASS;

		RegisterClass(&wc);
	}
	m_text			= NULL;
	m_url			= NULL;
	m_TextFont		= NULL;
	m_OverFont		= NULL;
	m_trackLeave	= FALSE;
}

CXUIUrl::~CXUIUrl(void)
{
	FREE_CLEAR_STR(m_url);
	FREE_CLEAR_STR(m_text);

	if(m_TextFont) DeleteObject(m_TextFont);
	if(m_OverFont) DeleteObject(m_OverFont);
}

LRESULT CALLBACK CXUIUrl::WndProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	CXUIUrl* pThis = NULL;
	if(IsWindow(hWnd))
	{
		pThis = (CXUIUrl*)GetProp(hWnd, TEXT("XUIUrl_this"));
	}

	if(pThis || uMessage == WM_CREATE)
	{
		switch (uMessage)
		{
		case WM_SETCURSOR:
			if(pThis->m_trackLeave)
			{
				SetCursor(LoadCursor(NULL, IDC_HAND));
				return TRUE;
			}
			break;
		case WM_GETDLGCODE:
			return DLGC_STATIC;
		case WM_LBUTTONDOWN:
			pThis->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_MOUSEMOVE:
			pThis->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_MOUSELEAVE:
			pThis->OnMouseLeave();
			return 0;
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
				pThis = (CXUIUrl*) lpcs->lpCreateParams;
				SetProp(hWnd, TEXT("XUIUrl_this"), (HANDLE) pThis);
				pThis->m_hWnd = hWnd;
				pThis->OnCreate();
			}
			return 0;
		case WM_DESTROY:
			RemoveProp(hWnd, TEXT("XUIUrl_this"));
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(pThis->m_hWnd, &ps);
				pThis->OnPaint(hdc);
				EndPaint(pThis->m_hWnd, &ps);
			}
			return 0;
		}
	}
	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void CXUIUrl::OnCreate( void )
{
	NONCLIENTMETRICS ncm;
	ZeroMemory(&ncm, sizeof(ncm));
	ncm.cbSize = sizeof(ncm);
	GetNonClientMetrics(&ncm);
	m_TextFont = CreateFontIndirect(&ncm.lfMessageFont);
	ncm.lfMessageFont.lfUnderline = TRUE;
	m_OverFont = CreateFontIndirect(&ncm.lfMessageFont);
}

BOOL CXUIUrl::GetNonClientMetrics( NONCLIENTMETRICS* ncm )
{
	ncm->cbSize = sizeof(NONCLIENTMETRICS);
	if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), ncm, 0))
	{
		ncm->cbSize -= sizeof(int);
		if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), ncm, 0))
		{
			return FALSE;
		}
	}
	return TRUE;
}

void CXUIUrl::OnLButtonDown( int x, int y )
{
	if(!m_text || m_text && !m_text[0]) 
	{
		return;
	}
	if (x >= m_rectLink.left && x <= m_rectLink.right)
	{
		if(!StrCmpI(m_url, TEXT(":notify:")))
		{
			raiseEvent(XUI_EVENT_CLICKED, 0, NULL);
		} else
		{
			ShellExecute(m_hWnd, TEXT("open"), m_url, NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

void CXUIUrl::OnPaint( HDC hdc )
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	HFONT oldfnt = NULL;

	oldfnt = (HFONT) SelectObject(hdc, m_TextFont);

	SendMessage(GetParent(m_hWnd), WM_ERASEBKGND, (WPARAM) hdc, NULL);

	if(m_text)
	{
		SetBkMode(hdc, TRANSPARENT);

		RECT rcText = {0, 0, 1, 1};
		DrawText(hdc, m_text, -1, &rcText, DT_CALCRECT | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);


		COLORREF txtColor = GetTextColor(hdc);
		SetTextColor(hdc, txtColor);

		int addStyle = DT_LEFT;

		if(m_textalign == XUI_URL_TEXT_ALIGN_RIGHT)
		{
			addStyle = DT_RIGHT;
			m_rectLink.left		= rcClient.right - rcText.right;
			m_rectLink.right	= rcClient.right;
			m_rectLink.top		= rcClient.top;
			m_rectLink.bottom	= rcClient.bottom;
		} else
		{
			m_rectLink.left		= rcClient.left;
			m_rectLink.right	= rcClient.left + rcText.right;
			m_rectLink.top		= rcClient.top;
			m_rectLink.bottom	= rcClient.bottom;
		}

		if(m_trackLeave)
			SelectObject(hdc, m_OverFont);

		SetTextColor(hdc, RGB(0, 0, 0xFF));
		DrawText(hdc, m_text, -1, &m_rectLink, addStyle | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	}
	SelectObject(hdc, oldfnt);
}

void CXUIUrl::OnMouseMove( int x, int y )
{
	if(!m_text || m_text && !m_text[0]) 
	{
		return;
	}
	if ((x > m_rectLink.left) && (x < m_rectLink.right))
	{
		if (!m_trackLeave)
		{
			m_trackLeave = TRUE;

			wlTrackMouseLeave(m_hWnd, FALSE);
			InvalidateRect(m_hWnd, NULL, FALSE);
		}
	}
	else
	{
		if (m_trackLeave)
		{
			m_trackLeave = FALSE;

			wlTrackMouseLeave(m_hWnd, FALSE);
			InvalidateRect(m_hWnd, NULL, FALSE);
		}
	}
}

VOID CALLBACK CXUIUrl::wlTrackMouseTimerProc(HWND hWnd,UINT uMsg,UINT idEvent,DWORD dwTime) 
{
	RECT rect;
	POINT pt;

	GetWindowRect(hWnd, &rect);
	GetCursorPos(&pt);

	BOOL isMenu = FALSE;
	{
		TCHAR clsInfo[255];
		clsInfo[0] = 0;
		HWND ptWND = WindowFromPoint(pt);
		GetClassName(ptWND, clsInfo, 255);
		if(!lstrcmp(clsInfo, TEXT("#32768")))
		{
			isMenu = TRUE;
		}
		if(!lstrcmp(clsInfo, TEXT("CPPTooltip")))
		{
			isMenu = TRUE;
		}
	}

	if (!PtInRect(&rect, pt) || (WindowFromPoint(pt) != hWnd && !isMenu)) 
	{
		KillTimer(hWnd, idEvent);
		PostMessage(hWnd, WM_MOUSELEAVE, 0, 0);
	}
}


void CXUIUrl::wlTrackMouseLeave(HWND hWnd, int remove)
{
	if(remove)
	{
		KillTimer(hWnd, 100);
	} else
	{
		SetTimer(hWnd, 100, 50,(TIMERPROC)wlTrackMouseTimerProc);
	}
}

void CXUIUrl::OnMouseLeave( void )
{
	wlTrackMouseLeave(m_hWnd, TRUE);
	m_trackLeave = FALSE;
	InvalidateRect(m_hWnd, NULL, TRUE);
}

void CXUIUrl::Init()
{
	DWORD wStyle = WS_CHILD;
	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	m_hWnd = CreateWindowEx(0, XUI_URL_CLASS, m_text, wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), (LPVOID) this);
	HDC hdc = GetDC(m_hWnd);
	HFONT oldFont = (HFONT) SelectObject(hdc, m_TextFont);
	
	RECT rcDraw = {0, 0, 3, 3};
	if(m_text)
	{
		DrawText(hdc, m_text, -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
	} else
	{
		DrawText(hdc, L"W", -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
	}
	m_minWidth = rcDraw.right - rcDraw.left;

	RECT rcDlg = {0, 0, 1, 8};
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);

	m_minHeight = max(rcDraw.bottom - rcDraw.top, rcDlg.bottom);

	SelectObject(hdc, oldFont);
	ReleaseDC(m_hWnd, hdc);
}

BOOL CXUIUrl::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_text) delete m_text;
	if(m_url)  delete m_url;
	m_text		= getLocalizedString(node,				TEXT("text"));
	m_url		= getLocalizedString(node,				TEXT("url"));
	m_textalign	= xmlGetAttributeValueSTRArray(node,	TEXT("textalign"), XUI_URL_TEXT_ALIGN_LEFT, L"left\0right\0");
	return TRUE;
}

void CXUIUrl::setData( LPCWSTR text, LPCWSTR url )
{
	MAKE_STR(m_text, text);
	MAKE_STR(m_url, url);
	InvalidateRect(m_hWnd, NULL, TRUE);
}
