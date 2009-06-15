#include "xuilib.h"
#include "XUITabSwitcher.h"
#include "XUITab.h"
#include <windowsx.h>
#include "xmltools.h"
#include <commctrl.h>

CXUITabSwitcher::CXUITabSwitcher(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_imageAlign	= 0;
	m_imageSize		= 16;
	m_hWndCaption	= NULL;

	m_mouseTrackInstalled = FALSE;

	WNDCLASS wc;
	if(!GetClassInfo(m_engine->get_hInstance(), XUI_TAB_SWITCHER, &wc))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc    = (WNDPROC)CXUITabSwitcher::WndProc;
		wc.cbClsExtra     = 0;
		wc.cbWndExtra     = 0;
		wc.hInstance      = m_engine->get_hInstance();
		wc.hIcon          = NULL;
		wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground  = (HBRUSH) (COLOR_WINDOW + 1);
		wc.lpszMenuName   = NULL;
		wc.lpszClassName  = XUI_TAB_SWITCHER;

		RegisterClass(&wc);
	}

	if(!GetClassInfo(m_engine->get_hInstance(), XUI_TAB_CAPTION, &wc))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc    = (WNDPROC)CXUITabSwitcher::WndProcCaption;
		wc.cbClsExtra     = 0;
		wc.cbWndExtra     = 0;
		wc.hInstance      = m_engine->get_hInstance();
		wc.hIcon          = NULL;
		wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground  = NULL;
		wc.lpszMenuName   = NULL;
		wc.lpszClassName  = XUI_TAB_CAPTION;

		RegisterClass(&wc);
	}

	m_hCaptionFont = NULL;
}

BOOL CXUITabSwitcher::GetNonClientMetrics(NONCLIENTMETRICS* ncm)
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


CXUITabSwitcher::~CXUITabSwitcher(void)
{
	if(m_hCaptionFont)
	{
		DeleteObject(m_hCaptionFont);
	}
}

LRESULT CXUITabSwitcher::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	CXUITabSwitcher* pThis = NULL;
	if(IsWindow(hWnd))
	{
		pThis = (CXUITabSwitcher*)GetProp(hWnd, TEXT("CXUITabSwitcher_this"));
	}

	if(pThis || uMessage == WM_CREATE)
	{
		switch(uMessage)
		{
		case WM_SETFOCUS:
			{
				HDC hdc = GetDC(hWnd);
				pThis->drawFocusRect(hdc);
				ReleaseDC(hWnd, hdc);
			}
			break;
		case WM_KILLFOCUS:
			{
				HDC hdc = GetDC(hWnd);
				pThis->drawFocusRect(hdc);
				ReleaseDC(hWnd, hdc);
			}
			break;
		case WM_GETDLGCODE:
			return DLGC_WANTARROWS;
		case WM_KEYDOWN:
			pThis->OnKeyDown(wParam);
			return 0;
		case WM_ERASEBKGND:
			return TRUE;
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
				pThis = (CXUITabSwitcher*) lpcs->lpCreateParams;
				SetProp(hWnd, TEXT("CXUITabSwitcher_this"), (HANDLE) pThis);
				pThis->m_hWnd = hWnd;
			}
			break;
		case WM_DESTROY:
			RemoveProp(hWnd, TEXT("CXUITabSwitcher_this"));
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				HFONT oldFont = (HFONT) SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
				pThis->OnPaint(hdc, &ps.rcPaint);
				SelectObject(hdc, oldFont);
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_LBUTTONDOWN:
			pThis->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_MOUSEMOVE:
			pThis->trackMouseLeave(TRUE);
			pThis->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_TIMER:
			if(wParam == TIMER_LEAVE)
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
					pThis->trackMouseLeave(FALSE);
					pThis->OnMouseLeave();
				}
			}
			break;
		case WM_NCCALCSIZE:
			{
				LPRECT rc = (LPRECT) lParam;
				rc->left	+= 1;
				rc->right	-= 1;
				rc->top		+= 1;
				rc->bottom	-= 1;
			}
			return 0;
		case WM_NCPAINT:
			{
				RECT rc;
				GetWindowRect(hWnd, &rc);
				rc.bottom = rc.bottom - rc.top;
				rc.right = rc.right - rc.left;
				rc.top = rc.left = 0;

				HDC hdc = GetWindowDC(hWnd);

				if(!hdc)
				{
					return DefWindowProc(hWnd, uMessage, wParam, lParam);
				}

				RECT rcDraw = rc;
				rcDraw.bottom = rcDraw.top + 1;
				FillRect(hdc, &rcDraw, (HBRUSH) (COLOR_GRAYTEXT + 1));

				rcDraw = rc;
				rcDraw.top = rcDraw.bottom - 1;
				FillRect(hdc, &rcDraw, (HBRUSH) (COLOR_GRAYTEXT + 1));

				rcDraw = rc;
				rcDraw.right = rcDraw.left + 1;
				FillRect(hdc, &rcDraw, (HBRUSH) (COLOR_GRAYTEXT + 1));

				rcDraw = rc;
				rcDraw.left = rcDraw.right - 1;
				FillRect(hdc, &rcDraw, (HBRUSH) (COLOR_GRAYTEXT + 1));

				ReleaseDC(hWnd, hdc);
			}
			return 0;
		}
	}
	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

LRESULT CXUITabSwitcher::WndProcCaption(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	CXUITabSwitcher* pThis = NULL;
	if(IsWindow(hWnd))
	{
		pThis = (CXUITabSwitcher*)GetProp(hWnd, TEXT("CXUITabSwitcher_this"));
	}

	if(pThis || uMessage == WM_CREATE)
	{
		switch(uMessage)
		{
		case WM_ERASEBKGND:
			return TRUE;
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
				pThis = (CXUITabSwitcher*) lpcs->lpCreateParams;
				SetProp(hWnd, TEXT("CXUITabSwitcher_this"), (HANDLE) pThis);
				pThis->m_hWndCaption = hWnd;
			}
			break;
		case WM_DESTROY:
			RemoveProp(hWnd, TEXT("CXUITabSwitcher_this"));
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				HFONT oldFont = (HFONT) SelectObject(hdc, pThis->m_hCaptionFont);
				pThis->OnPaintCaption(hdc, &ps.rcPaint);
				SelectObject(hdc, oldFont);
				EndPaint(hWnd, &ps);
			}
			break;
		}
	}
	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void CXUITabSwitcher::Init()
{
	DWORD wStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER;
	DWORD wStyleCaption = WS_CHILD | WS_VISIBLE;
	if(m_disabled) wStyle |= WS_DISABLED;

	RECT rcMargins;
	rcMargins.left = 0;
	rcMargins.right = 10;
	rcMargins.top = 7;
	rcMargins.bottom = 20;
	MapDialogRect(m_parent->get_parentWnd(), &rcMargins);
	m_ctlsMarginX	= rcMargins.right;
	m_ctlsMarginY	= rcMargins.top;
	m_captionHeight	= rcMargins.bottom;
	if(!m_bShowCaption)
	{
		m_captionHeight = 0;
		m_ctlsMarginY	= 0;
	}
	if(!m_bShowSwitcher)
	{
		m_ctlsMarginX	= 0;
	}

	NONCLIENTMETRICS ncm;
	ZeroMemory(&ncm, sizeof(ncm));
	ncm.cbSize = sizeof(ncm);
	GetNonClientMetrics(&ncm);
	ncm.lfCaptionFont.lfHeight = m_captionHeight * 2 / 3;
	ncm.lfCaptionFont.lfWidth = 0;
	ncm.lfCaptionFont.lfWeight = FW_BOLD;
	m_hCaptionFont = CreateFontIndirect(&ncm.lfCaptionFont);

	m_minWidth		= 0;
	m_minHeight		= 0;
	m_defTabHeight	= 0;

	if(m_bShowSwitcher)
	{
		m_hWnd = CreateWindowEx(0, XUI_TAB_SWITCHER, TEXT(""), wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), (LPVOID) this);
		SetWindowFont(m_hWnd, GetStockObject(DEFAULT_GUI_FONT), TRUE);
		HDC hdc = GetDC(m_hWnd);
		HFONT oldFont = (HFONT) SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

		for(int i=0; i < m_childCount; i++)
		{
			CXUITab* tab = NULL;
			m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
			if(tab)
			{
				SIZE szTab;
				tab->GetTabSize(hdc, m_imageAlign, m_imageSize, szTab);
				m_minWidth		= max(m_minWidth, szTab.cx);
				m_minHeight		+= szTab.cy + 2;
				m_defTabHeight	= max(m_defTabHeight, szTab.cy);
			}
		}
		m_minWidth  += 2; // add the border size
		m_minHeight += 4; // add buttons margins

		SelectObject(hdc, oldFont);
		ReleaseDC(m_hWnd, hdc);
	}

	if(m_bShowCaption)
	{
		m_hWndCaption = CreateWindowEx(0, XUI_TAB_CAPTION, TEXT(""), wStyleCaption, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), (LPVOID) this);
	}

	CXUIElement::Init();
}

void CXUITabSwitcher::render( int x, int y, int width, int height )
{
	if(m_hWnd)
	{
		SetWindowPos(m_hWnd, NULL, x, y, m_minWidth, height, SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
	if(m_hWndCaption)
	{
		SetWindowPos(m_hWndCaption, NULL, 
			x + m_minWidth + m_ctlsMarginX, y, 
			width - m_minWidth - m_ctlsMarginX, 
			m_captionHeight, 
			SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
	recalcLayout();
	for(int i=0; i < m_childCount; i++)
	{
		m_childs[i]->render(x + m_minWidth + m_ctlsMarginX, 
			y + m_captionHeight + m_ctlsMarginY, 
			width - m_minWidth - m_ctlsMarginX, 
			height - m_captionHeight - m_ctlsMarginY);
	}
}

void CXUITabSwitcher::getMinSize( SIZE& minSize )
{
	minSize.cx = m_minWidth  + m_ctlsMarginX;
	minSize.cy = max(m_minHeight, m_captionHeight + m_ctlsMarginY);
	int width	= 0;
	int height	= 0;
	for(int i=0; i < m_childCount; i++)
	{
		SIZE sz = {0};
		m_childs[i]->getMinSize(sz);
		height = max(height, sz.cy);
		width  = max(width,  sz.cx);
	}
	minSize.cx += width;
	minSize.cy = max(m_minHeight, height + m_captionHeight + m_ctlsMarginY);
}

BOOL CXUITabSwitcher::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	m_imageAlign	= xmlGetAttributeValueSTRArray(node, TEXT("imageAlign"), XUI_TAB_IMAGE_ALIGN_LEFT, L"left\0top\0");
	m_imageSize		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("imageSize"),	16);
	m_bShowSwitcher	= xmlGetAttributeValueBOOL(node, L"showSwitcher", TRUE);
	m_bShowCaption	= xmlGetAttributeValueBOOL(node, L"showCaption", TRUE);
	return TRUE;
}

HWND CXUITabSwitcher::get_parentWnd()
{
	if(m_parent)
	{
		return m_parent->get_parentWnd();
	}
	return NULL;
}

void CXUITabSwitcher::recalcLayout()
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	int top = 2;
	for(int i=0; i < m_childCount; i++)
	{
		CXUITab* tab = NULL;
		m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
		if(tab)
		{
			RECT rcItem;
			rcItem.left		= rcClient.left;
			rcItem.right	= rcClient.right;
			rcItem.top		= rcClient.top + top;
			rcItem.bottom	= rcItem.top + m_defTabHeight;
			tab->rect(&rcItem);
			top += m_defTabHeight + 2;
		}
	}
}

void CXUITabSwitcher::OnPaint( HDC hdc, LPRECT rcDraw )
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	HBITMAP bmp = CreateCompatibleBitmap(hdc, 
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top);
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP oldBmp = (HBITMAP) SelectObject(memDC, bmp);

	FillRect(memDC, &rcClient, (HBRUSH) (COLOR_WINDOW + 1));
	HFONT oldFont = (HFONT) SelectObject(memDC, GetStockObject(DEFAULT_GUI_FONT));

	for(int i=0; i < m_childCount; i++)
	{
		CXUITab* tab = NULL;
		m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
		if(tab)
		{
			tab->Draw(memDC, m_imageAlign, m_imageSize, (LPRECT) tab->rect());
		}
	}

	if(GetFocus() == m_hWnd)
	{
		drawFocusRect(memDC);
	}

	BitBlt(hdc, rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		memDC, 0, 0, SRCCOPY);
	SelectObject(memDC, oldFont);
	SelectObject(memDC, oldBmp);
	DeleteDC(memDC);
	DeleteObject(bmp);
}

void CXUITabSwitcher::OnMouseMove( int x, int y )
{
	CXUITab* overTab = findTab(x, y);
	CXUITab* hoverTab = findHoverTab();
	if(hoverTab && hoverTab != overTab)
	{
		hoverTab->hover(FALSE);
		InvalidateRect(m_hWnd, (LPRECT) hoverTab->rect(), TRUE);
	}
	if(overTab)
	{
		if(!overTab->selected() && !overTab->hover())
		{
			overTab->hover(TRUE);
			InvalidateRect(m_hWnd, (LPRECT) overTab->rect(), TRUE);
		}
	}
}

void CXUITabSwitcher::OnLButtonDown( int x, int y )
{
	CXUITab* overTab = findTab(x, y);
	if(overTab)
	{
		CXUITab* selTab = findSelectedTab();
		if(selTab != overTab)
		{
			if(selTab)
			{
				selTab->selected(FALSE);
				InvalidateRect(m_hWnd, (LPRECT) selTab->rect(), TRUE);
			}
			overTab->selected(TRUE);
			InvalidateRect(m_hWnd, (LPRECT) overTab->rect(), TRUE);
			InvalidateRect(m_hWndCaption, NULL, FALSE);
		}
	}
}

CXUITab* CXUITabSwitcher::findTab( int x, int y )
{
	for(int i=0; i < m_childCount; i++)
	{
		CXUITab* tab = NULL;
		m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
		if(tab)
		{
			LPCRECT rcItem = tab->rect();
			if(x >= rcItem->left && x <= rcItem->right &&
			   y >= rcItem->top  && y <= rcItem->bottom)
			{
				return tab;
			}
		}
	}
	return NULL;
}

CXUITab* CXUITabSwitcher::findSelectedTab()
{
	for(int i=0; i < m_childCount; i++)
	{
		CXUITab* tab = NULL;
		m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
		if(tab)
		{
			if(tab->selected())
			{
				return tab;
			}
		}
	}
	return NULL;
}

CXUITab* CXUITabSwitcher::findHoverTab()
{
	for(int i=0; i < m_childCount; i++)
	{
		CXUITab* tab = NULL;
		m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
		if(tab)
		{
			if(tab->hover())
			{
				return tab;
			}
		}
	}
	return NULL;
}

void CXUITabSwitcher::trackMouseLeave( BOOL bInstall )
{
	if(bInstall)
	{
		if(!m_mouseTrackInstalled)
		{
			m_mouseTrackInstalled = TRUE;
			SetTimer(m_hWnd, TIMER_LEAVE, 50, NULL);
		}
	} else
	{
		m_mouseTrackInstalled = FALSE;
		KillTimer(m_hWnd, TIMER_LEAVE);
	}
}

void CXUITabSwitcher::OnMouseLeave()
{
	CXUITab* hoverTab = findHoverTab();
	if(hoverTab)
	{
		hoverTab->hover(FALSE);
		InvalidateRect(m_hWnd, (LPRECT) hoverTab->rect(), TRUE);
	}
}

void CXUITabSwitcher::OnPaintCaption( HDC hdc, LPRECT rcDraw )
{
	CXUITab* selTab = findSelectedTab();
	if(selTab)
	{
		LPCWSTR caption = selTab->get_caption();
		RECT rcClient;
		GetClientRect(m_hWndCaption, &rcClient);
		DrawGradient(hdc, rcClient, tswLightColor(GetSysColor(COLOR_HIGHLIGHT), 20), tswLightColor(GetSysColor(COLOR_HIGHLIGHT), -20));
		rcClient.left += 4;
		if(caption)
		{
			SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
			SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, caption, -1, &rcClient, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		}
	}
}

void CXUITabSwitcher::DrawGradient(HDC dc, const RECT& rectClient, COLORREF m_clrStart, COLORREF m_clrEnd) 
{ 
	int nMaxWidth = rectClient.bottom - rectClient.top;
	RECT rectFill; // Rectangle for filling band 
	float fStep; // How wide is each band? 
	HBRUSH brush = NULL; // Brush to fill in the bar 
	//First find out the largest color distance between the start and 
	//end colors.This distance will determine how many steps we use to 
	//carve up the client region and the size of each gradient rect. 

	int r, g, b; // First distance, then starting value 
	float rStep, gStep, bStep; // Step size for each color 
	BOOL bSameColor = FALSE; 
	int nSteps; 
	// Get the color differences 
	r = (GetRValue(m_clrEnd) - GetRValue(m_clrStart)); 
	g = (GetGValue(m_clrEnd) - GetGValue(m_clrStart)); 
	b = (GetBValue(m_clrEnd) - GetBValue(m_clrStart)); 

	if((r == 0) && (g == 0) && (b == 0)) 
	{ 
		bSameColor = TRUE; 
		//{{ADDED BY RAJESH 
		//Added the three lines below to fix the drawing 
		//problem which used to occur when both the start 
		//and end colors are same. 
		r = GetRValue(m_clrStart); 
		g = GetGValue(m_clrStart); 
		b = GetBValue(m_clrStart); 
		//}}ADDED BY RAJESH 
	} 

	// Make the number of steps equal to the greatest distance 
	//{{MODIFIED BY RAJESH 
	//To fix the drawing problem which used to occur when 
	//both the start and end colors are RGB(0,0,0). 
	if(bSameColor && m_clrStart == 0) 
	{
		nSteps = 255;//Select max. possible value for nSteps 
	}
	else 
	{
		nSteps = max(abs(r), max(abs(g), abs(b))); 
	}
	//Determine how large each band should be in order to cover the 
	//client with nSteps bands (one for every color intensity level) 
	//}}MODIFIED BY RAJESH 
	fStep = (float)(rectClient.bottom - rectClient.top) / (float)nSteps; 

	//Calculate the step size for each color 
	rStep = r/(float)nSteps; 
	gStep = g/(float)nSteps; 
	bStep = b/(float)nSteps; 

	//Reset the colors to the starting position 
	r = GetRValue(m_clrStart); 
	g = GetGValue(m_clrStart); 
	b = GetBValue(m_clrStart); 

	// Start filling bands 
	for (int iOnBand = 0; iOnBand < nSteps; iOnBand++) 
	{ 
		SetRect(&rectFill, 
			rectClient.left, 
			rectClient.top + (int)(iOnBand * fStep), 
			rectClient.right, 
			rectClient.top + (int)((iOnBand+1) * fStep));

		if(iOnBand+1 == nSteps)
		{
			rectFill.bottom = rectClient.bottom;
		}

		if(rectFill.bottom > rectClient.bottom) 
		{ 
			rectFill.bottom = rectClient.bottom;
		}

		//CDC::FillSolidRect is faster, but it does not handle 
		//8-bit color depth 
		if(bSameColor) 
		{
			brush = CreateSolidBrush(m_clrStart);
		}
		else 
		{
			brush = CreateSolidBrush(RGB(r+rStep * iOnBand, 
				g + gStep * iOnBand, b + bStep * iOnBand)); 
		}
		FillRect(dc, &rectFill, brush); 
		DeleteObject(brush);

		//If we are past the maximum for the current position we 
		//need to get out of the loop.Before we leave,we repaint the 
		//remainder of the client area with the background color. 
	} 
} 

void CXUITabSwitcher::value_INT( INT val )
{
	for(int i=0; i < m_childCount; i++)
	{
		CXUITab* tab = NULL;
		m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
		if(tab)
		{
			if(tab->value_INT() == val)
			{
				if(!tab->selected())
				{
					CXUITab* selTab = findSelectedTab();
					if(selTab)
					{
						selTab->selected(FALSE);
					}
					tab->selected(TRUE);
				}
				break;
			}
		}
	}
	if(m_hWndCaption)
	{
		InvalidateRect(m_hWndCaption, NULL, FALSE);
	}
	if(m_bShowSwitcher)
	{
		InvalidateRect(get_wnd(), NULL, FALSE);
	}
}

INT CXUITabSwitcher::value_INT()
{
	for(int i=0; i < m_childCount; i++)
	{
		CXUITab* tab = NULL;
		m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
		if(tab)
		{
			if(tab->selected())
			{
				return tab->value_INT();
			}
		}
	}
	return -1;
}

void CXUITabSwitcher::onCmd( LPWSTR cmd )
{
	if(!lstrcmpi(cmd, L"next"))
	{
		CXUITab* curTab = NULL;
		CXUITab* nextTab = NULL;
		for(int i=0; i < m_childCount; i++)
		{
			CXUITab* tab = NULL;
			m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
			if(tab)
			{
				if(!curTab)
				{
					if(tab->selected())
					{
						curTab = tab;
					}
				} else
				{
					nextTab = tab;
				}
			}
			if(curTab && nextTab) break;
		}
		if(curTab && nextTab)
		{
			curTab->selected(FALSE);
			nextTab->selected(TRUE);
			if(m_hWndCaption)
			{
				InvalidateRect(m_hWndCaption, NULL, FALSE);
			}
		}
	} else if(!lstrcmpi(cmd, L"back"))
	{
		CXUITab* curTab = NULL;
		CXUITab* prevTab = NULL;
		for(int i=0; i < m_childCount; i++)
		{
			CXUITab* tab = NULL;
			m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
			if(tab)
			{
				if(tab->selected())
				{
					curTab = tab;
				} else
				{
					prevTab = tab;
				}
			}
			if(curTab) break;
		}
		if(curTab && prevTab)
		{
			curTab->selected(FALSE);
			prevTab->selected(TRUE);
			if(m_hWndCaption)
			{
				InvalidateRect(m_hWndCaption, NULL, FALSE);
			}
		}
	} else if(!lstrcmpi(cmd, L"help"))
	{
		CXUITab* curTab = NULL;
		for(int i=0; i < m_childCount; i++)
		{
			CXUITab* tab = NULL;
			m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
			if(tab)
			{
				if(tab->selected())
				{
					curTab = tab;
				}
			}
			if(curTab) break;
		}
		if(curTab)
		{
			curTab->onCmd(L"help");
		}
	}
}

void CXUITabSwitcher::reinitTabs()
{
	for(int i=0; i < m_childCount; i++)
	{
		CXUITab* tab = NULL;
		m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
		if(tab)
		{
			tab->reinit();
		}
	}
}

LPCWSTR CXUITabSwitcher::value_STR()
{
	for(int i=0; i < m_childCount; i++)
	{
		CXUITab* tab = NULL;
		m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
		if(tab)
		{
			if(tab->selected())
			{
				return tab->get_id();
			}
		}
	}
	return NULL;
}

void CXUITabSwitcher::value_STR( LPCWSTR val )
{
	for(int i=0; i < m_childCount; i++)
	{
		CXUITab* tab = NULL;
		m_childs[i]->QueryElement(L"tab", (LPVOID*) &tab);
		if(tab)
		{
			if(!lstrcmp(tab->get_id(), val))
			{
				value_INT(tab->value_INT());
				break;
			}
		}
	}
}

void CXUITabSwitcher::OnKeyDown( UINT vk )
{
	switch(vk)
	{
	case VK_UP:
		{
			CXUITab* oldTab = findSelectedTab();

			onCmd(L"back");

			CXUITab* newTab = findSelectedTab();
			InvalidateRect(m_hWnd, (LPRECT) oldTab->rect(), TRUE);
			InvalidateRect(m_hWnd, (LPRECT) newTab->rect(), TRUE);
			if(m_hWndCaption)
			{
				InvalidateRect(m_hWndCaption, NULL, FALSE);
			}
		}
		break;
	case VK_DOWN:
		{
			CXUITab* oldTab = findSelectedTab();

			onCmd(L"next");

			CXUITab* newTab = findSelectedTab();
			InvalidateRect(m_hWnd, (LPRECT) oldTab->rect(), TRUE);
			InvalidateRect(m_hWnd, (LPRECT) newTab->rect(), TRUE);
			if(m_hWndCaption)
			{
				InvalidateRect(m_hWndCaption, NULL, FALSE);
			}
		}
		break;
	}
}

void CXUITabSwitcher::drawFocusRect(HDC hdc)
{
	if(m_hWnd)
	{
		CXUITab* selTab = findSelectedTab();
		if(selTab)
		{
			RECT rcDraw = *selTab->rect();
			rcDraw.left  += 2;
			rcDraw.right -= 2;
			DrawFocusRect(hdc, &rcDraw);
		}
	}
}

BOOL CXUITabSwitcher::onNotify( int idCtrl, LPNMHDR pnmh )
{
	if(pnmh->code == TCN_SELCHANGE)
	{
		int idx = TabCtrl_GetCurSel(m_hWnd);
		TCITEM tci;
		ZeroMemory(&tci, sizeof(TCITEM));
		tci.mask = TCIF_PARAM;
		TabCtrl_GetItem(m_hWnd, idx, &tci);
		CXUITab* tab = (CXUITab*) tci.lParam;

		CXUITab* selTab = findSelectedTab();
		if(selTab != tab)
		{
			if(selTab)
			{
				selTab->selected(FALSE);
			}
			tab->selected(TRUE);
		}

	}
	return FALSE;
}
