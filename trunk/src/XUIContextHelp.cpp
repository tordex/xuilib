#include "xuilib.h"
#include "XUIContextHelp.h"
#include "xmltools.h"


CXUIContextHelp::CXUIContextHelp(CXUIEngine* engine) : m_html(this)
{
	m_engine	= engine;
	m_items		= NULL;
	m_count		= 0;
	m_hWnd		= NULL;
	m_curItem	= -1;

	WNDCLASS wc;
	if(!GetClassInfo(m_engine->get_hInstance(), XUI_CONTEXTHELP_CLASS, &wc))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style          = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc    = (WNDPROC)CXUIContextHelp::WndProc;
		wc.cbClsExtra     = 0;
		wc.cbWndExtra     = 0;
		wc.hInstance      = m_engine->get_hInstance();
		wc.hIcon          = NULL;
		wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground  = (HBRUSH)(COLOR_INFOBK + 1);
		wc.lpszMenuName   = NULL;
		wc.lpszClassName  = XUI_CONTEXTHELP_CLASS;

		RegisterClass(&wc);
	}
}

CXUIContextHelp::~CXUIContextHelp(void)
{
	Clear();
}

void CXUIContextHelp::LinkClicked( TCHAR *szLink )
{

}

void CXUIContextHelp::SetCursor( BOOL bLink,TCHAR *szLink )
{

}

void CXUIContextHelp::SetTitle( TCHAR *szTitle )
{

}

void CXUIContextHelp::loadDATA( IXMLDOMNode* node )
{
	IXMLDOMNode* child = NULL;
	node->get_firstChild(&child);
	while(child)
	{
		BSTR name = NULL;
		child->get_nodeName(&name);
		if(name && !lstrcmpi(name, L"tip"))
		{
			if(!m_items)
			{
				m_items = (CONTEXT_HELP_ITEM*) malloc(sizeof(CONTEXT_HELP_ITEM));
			} else
			{
				m_items = (CONTEXT_HELP_ITEM*) realloc(m_items, (m_count + 1) * sizeof(CONTEXT_HELP_ITEM));
			}
			
			m_items[m_count].control	= xmlGetAttributeSTR(child, TEXT("control"));
			m_items[m_count].img		= xmlGetAttributeSTR(child, TEXT("img"));
			m_items[m_count].tag		= xmlGetAttributeSTR(child, TEXT("tag"));
			m_items[m_count].imgWidth	= xmlGetAttributeValueNonSTR<INT>(child, TEXT("imgWidth"), 0);
			m_items[m_count].imgHeight	= xmlGetAttributeValueNonSTR<INT>(child, TEXT("imgHeight"), 0);
			m_items[m_count].text		= NULL;

			LPWSTR locStr = NULL;
			LPWSTR locID = xmlGetAttributeSTR(child, TEXT("locid"));
			if(locID)
			{
				locStr = m_engine->getString(locID, L"CDATA");
				if(locStr)
				{
					m_items[m_count].text = new WCHAR[lstrlen(locStr) + 1];
					lstrcpy(m_items[m_count].text, locStr);
				}
				delete locID;
			}
			if(!m_items[m_count].text)
			{
				m_items[m_count].text = xmlGetNodeText(child);
			}

			m_count++;
		}
		if(name)
		{
			SysFreeString(name);
		}
		IXMLDOMNode* newChild = NULL;
		child->get_nextSibling(&newChild);
		child->Release();
		child = newChild;
	}
}

void CXUIContextHelp::Clear()
{
	for(int i=0; i < m_count; i++)
	{
		if(m_items[i].control)	delete m_items[i].control;
		if(m_items[i].text)		delete m_items[i].text;
		if(m_items[i].img)		delete m_items[i].img;
		if(m_items[i].tag)		delete m_items[i].tag;
	}
	if(m_items)
	{
		free(m_items);
	}
	m_items = NULL;
	m_count = NULL;
}

void CXUIContextHelp::show( LPCWSTR control, HWND hWndCtl, LPCWSTR tag )
{
	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
	}
	m_curItem = -1;
	for(int i=0; i < m_count; i++)
	{
		if(tag)
		{
			if(m_items[i].tag)
			{
				if(!lstrcmpi(m_items[i].control, control) && !lstrcmpi(m_items[i].tag, tag))
				{
					m_curItem = i;
					break;
				}
			}
		} else
		{
			if(!lstrcmpi(m_items[i].control, control) && !m_items[i].tag)
			{
				m_curItem = i;
				break;
			}
		}
	}
	if(m_curItem >= 0)
	{
		RECT rcTip = {0};
		GetWindowRect(hWndCtl, &rcTip);
		LOGFONT lf;
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);

		m_html.SetHTML(m_items[m_curItem].text, lf.lfFaceName, lf.lfHeight, GetSysColor(COLOR_INFOTEXT), L"/");
		RECT rcHTML = {10, 10, 320, 20};
		rcHTML.left += m_items[m_curItem].imgWidth;
		if(m_items[m_curItem].imgWidth)
		{
			rcHTML.left += 10;
		}
		rcHTML.right = rcHTML.left + 310;
		m_html.CalcPositions(rcHTML);

		RECT rcDesk;
		GetDesktopRect(&rcDesk, hWndCtl);

		int width	= rcHTML.right - rcHTML.left + 20;
		if(m_items[m_curItem].imgWidth)
		{
			width += m_items[m_curItem].imgWidth + 10;
		}
		int height	= rcHTML.bottom - rcHTML.top + 20;
		if(height < m_items[m_curItem].imgHeight + 20)
		{
			height = m_items[m_curItem].imgHeight + 20;
		}
		int x		= rcTip.left;
		int y		= rcTip.top - height - 5;


		if(y < rcDesk.top)
		{
			y = rcTip.bottom + 5;
		}
		if(x + width > rcDesk.right)
		{
			x = rcDesk.right - width - 5;
		}
		if(x < rcDesk.left)
		{
			x = rcDesk.left + 5;
		}

		m_hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, XUI_CONTEXTHELP_CLASS, L"", 
			WS_POPUP | WS_VISIBLE, 
			x, y,
			width, height,
			NULL, NULL, m_engine->get_hInstance(), (LPVOID) this);
	}
}

LRESULT CALLBACK CXUIContextHelp::WndProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	CXUIContextHelp* pThis = NULL;
	if(IsWindow(hWnd))
	{
		pThis = (CXUIContextHelp*)GetProp(hWnd, TEXT("xuictxhelp_this"));
	}

	if(pThis || uMessage == WM_CREATE)
	{
		switch (uMessage)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_KILLFOCUS:
		case WM_KEYDOWN:
			DestroyWindow(hWnd);
			return 0;
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
				pThis = (CXUIContextHelp*) lpcs->lpCreateParams;
				SetProp(hWnd, TEXT("xuictxhelp_this"), (HANDLE) pThis);
				pThis->m_hWnd = hWnd;
			}
			break;
		case WM_DESTROY:
			pThis->m_hWnd = NULL;
			RemoveProp(hWnd, TEXT("xuictxhelp_this"));
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				pThis->OnPaint(hdc);
				EndPaint(hWnd, &ps);
			}
			break;
		}
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void CXUIContextHelp::OnPaint( HDC hdc )
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	HPEN pen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_INFOTEXT));
	HPEN oldPen = (HPEN) SelectObject(hdc, pen);
	Rectangle(hdc, rcClient.left, rcClient.top,
		rcClient.right,	rcClient.bottom);
	SelectObject(hdc, oldPen);
	DeleteObject(pen);

	rcClient.left	+= 10;
	rcClient.top	+= 10;
	if(m_curItem >= 0)
	{
		if(m_items[m_curItem].imgWidth)
		{
			if(m_items[m_curItem].img)
			{
				m_engine->DrawImage(hdc, rcClient.left, rcClient.top, 
					m_items[m_curItem].imgWidth, m_items[m_curItem].imgHeight,
					m_items[m_curItem].img);
			}
			rcClient.left += m_items[m_curItem].imgWidth + 10;
		}
	}
	m_html.Draw(hdc, rcClient, 0, 0);
}

void CXUIContextHelp::GetDesktopRect(RECT* rcDsk, HWND hWnd)
{
	int nMonitors = GetSystemMetrics(80);

	HWND dskWnd = GetDesktopWindow();
	GetClientRect(dskWnd, rcDsk);

	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	if(hMonitor)
	{
		MONITORINFO mInf;
		mInf.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &mInf);
		*rcDsk = mInf.rcWork;
	}
}
