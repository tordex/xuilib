#include "xuilib.h"
#include "XUIPicture.h"
#include "xmltools.h"

CXUIPicture::CXUIPicture(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_curFrame		= NULL;
	m_picture		= NULL;
	m_picWidth		= NULL;
	m_picHeight		= NULL;

	WNDCLASS wc;
	if(!GetClassInfo(m_engine->get_hInstance(), XUI_PICTURE_CLASS, &wc))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style          = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc    = (WNDPROC)CXUIPicture::WndProc;
		wc.cbClsExtra     = 0;
		wc.cbWndExtra     = 0;
		wc.hInstance      = m_engine->get_hInstance();
		wc.hIcon          = NULL;
		wc.hCursor        = NULL;
		wc.hbrBackground  = NULL;
		wc.lpszMenuName   = NULL;
		wc.lpszClassName  = XUI_PICTURE_CLASS;

		RegisterClass(&wc);
	}
}

CXUIPicture::~CXUIPicture(void)
{
	if(m_picture) delete m_picture;
}

LRESULT CALLBACK CXUIPicture::WndProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	CXUIPicture* pThis = NULL;
	if(IsWindow(hWnd))
	{
		pThis = (CXUIPicture*)GetProp(hWnd, TEXT("xuipicture_this"));
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
				pThis = (CXUIPicture*) lpcs->lpCreateParams;
				SetProp(hWnd, TEXT("xuipicture_this"), (HANDLE) pThis);
				pThis->m_hWnd = hWnd;
			}
			break;
		case WM_DESTROY:
			RemoveProp(hWnd, TEXT("xuipicture_this"));
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				pThis->OnPaint(hdc);
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_TIMER:
			{
				pThis->m_curFrame++;
				if(pThis->m_curFrame >= pThis->m_frames)
				{
					pThis->m_curFrame = 0;
				}
				InvalidateRect(hWnd, NULL, FALSE);
				UpdateWindow(hWnd);
			}
			break;
		}
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void CXUIPicture::OnPaint( HDC hdc )
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	int width	= rcClient.right - rcClient.left;
	int height	= rcClient.bottom - rcClient.top;

	HBITMAP bmp		= CreateCompatibleBitmap(hdc, width, height);
	HDC		memDC	= CreateCompatibleDC(hdc);
	HBITMAP oldBmp	= (HBITMAP) SelectObject(memDC, bmp);

	SendMessage(GetParent(m_hWnd), WM_ERASEBKGND, (WPARAM) memDC, NULL);

	if(m_frames <= 1)
	{
		m_engine->DrawImage(memDC, 0, 0, m_picWidth, m_picHeight, m_picture);
	} else
	{
		m_engine->DrawFrame(memDC, 0, 0, m_picWidth, m_picHeight, m_picture, m_frames, m_curFrame, m_framesAlign);
	}

	BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
	SelectObject(memDC, oldBmp);
	DeleteDC(memDC);
	DeleteObject(bmp);
}

void CXUIPicture::Init()
{
	DWORD wStyle = WS_CHILD;

	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	m_hWnd = CreateWindowEx(0, XUI_PICTURE_CLASS, TEXT(""), wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), (LPVOID) this);

	if(m_frames > 1)
	{
		SetTimer(m_hWnd, 1, m_animInterval, NULL);
	}

	m_minWidth  = m_picWidth;
	m_minHeight = m_picHeight;
}

BOOL CXUIPicture::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_picture) delete m_picture;
	m_picture		= xmlGetAttributeSTR(node,				TEXT("pic"));
	m_picWidth		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("picWidth"),		0);
	m_picHeight		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("picHeight"),		0);
	m_frames		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("frames"),			1);
	m_animInterval	= xmlGetAttributeValueNonSTR<int>(node,	TEXT("animInterval"),	0);
	m_framesAlign	= xmlGetAttributeValueSTRArray(node, TEXT("framesOrient"), XUI_FRAMES_VALIGN, L"vertical\0horizontal\0");
	return TRUE;
}