#include "xuilib.h"
#include "XUIColor.h"
#include "xmltools.h"
#include <WindowsX.h>

//////////////////////////////////////////////////////////////////////////
//				CXUIColorPicker Window										//
//////////////////////////////////////////////////////////////////////////

#define COLOR_BOX_SIZE	15

#define COLORS_H		18
#define COLORS_V		15

class CXUIColorPicker
{
	COLORREF m_colors[COLORS_V][COLORS_H];
	HWND m_hWnd;
	COLORREF m_color;
	HWND m_hWndParent;

	int selectedX;
	int selectedY;

	void OnPaint();

	void GetDesktopRect(RECT* rcDsk, HWND hWnd);
public:
	BOOL m_enableNone;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

	CXUIColorPicker();
	virtual ~CXUIColorPicker(void);
	void Create(HWND hWndParent, COLORREF color, HINSTANCE hInst);
	void OnMouseMove(DWORD keys, int x, int y);
private:
	void DrawColorBar(HDC hdc);
	void DrawSelectedColor(HDC hdc, BOOL remove);
	void SetSelectedColor(int x, int y);
	void OnLButtonDown(int x, int y);
};

void CXUIColorPicker::OnPaint()
{
	PAINTSTRUCT ps;
	BeginPaint(m_hWnd, &ps);

	HPEN pen = CreatePen(PS_SOLID, 1, 0);

	HPEN oldPen = (HPEN) SelectObject(ps.hdc, pen);

	for(DWORD i=0; i < COLORS_V; i++)
	{
		for(DWORD j=0; j < COLORS_H; j++)
		{
			HBRUSH br = CreateSolidBrush(m_colors[i][j]);
			RECT rc;
			rc.top = i * COLOR_BOX_SIZE + 20; rc.left = j * COLOR_BOX_SIZE + 1;
			rc.bottom = rc.top + COLOR_BOX_SIZE + 1;
			rc.right = rc.left + COLOR_BOX_SIZE + 1;

			HBRUSH oldBrush = (HBRUSH) SelectObject(ps.hdc, br);
			Rectangle(ps.hdc, rc.left, rc.top, rc.right, rc.bottom);
			SelectObject(ps.hdc, oldBrush);
			DeleteObject(br);
		}
	}

	SelectObject(ps.hdc, pen);
	HBRUSH oldBrush = (HBRUSH) SelectObject(ps.hdc, GetStockObject(HOLLOW_BRUSH));

	RECT rcButton;
	GetClientRect(m_hWnd, &rcButton);

	rcButton.top += 2; rcButton.bottom = rcButton.top + 16;
	rcButton.right -= 2; rcButton.left = rcButton.right - 16;

	Rectangle(ps.hdc, rcButton.left, rcButton.top, rcButton.right, rcButton.bottom);

	int x = rcButton.left + 6;
	int y = rcButton.top + 2;
	for(int i=0; i < 6; i++)
	{
		MoveToEx(ps.hdc, x + i, y + i, NULL);
		LineTo(ps.hdc, x + i, y + 11 - i);
	}

	if(m_enableNone)
	{
		RECT rcNoneButton = rcButton;

		rcNoneButton.right -= 20;
		rcNoneButton.left -= 20;
		Rectangle(ps.hdc, rcNoneButton.left, rcNoneButton.top, rcNoneButton.right, rcNoneButton.bottom);
		HPEN redPen = CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
		SelectObject(ps.hdc, redPen);
		MoveToEx(ps.hdc, rcNoneButton.left + 1, rcNoneButton.bottom - 2, NULL);
		LineTo(ps.hdc, rcNoneButton.right - 1, rcNoneButton.top);
		DeleteObject(redPen);
	}


	SelectObject(ps.hdc, oldPen);
	SelectObject(ps.hdc, oldBrush);

	DeleteObject(pen);
	DrawColorBar(ps.hdc);

	EndPaint(m_hWnd, &ps);
}

void CXUIColorPicker::Create(HWND hWndParent, COLORREF color, HINSTANCE hInst)
{
	m_color = color;
	m_hWndParent = hWndParent;
	RECT rcWindow;
	GetWindowRect(hWndParent, &rcWindow);

	int y = rcWindow.bottom;
	int x = rcWindow.left;

	int width = COLORS_H * COLOR_BOX_SIZE + 5;
	int height = COLORS_V * COLOR_BOX_SIZE + 24;

	RECT rcDesktop;
	GetDesktopRect(&rcDesktop, m_hWndParent);
	if(y + height > rcDesktop.bottom)
	{
		y = rcWindow.top - height;
	}

	CreateWindowEx(0,
		XUI_COLORPICKER_CLASS, NULL, 
		WS_POPUP | WS_VISIBLE | WS_BORDER, 
		x, y, width, height, hWndParent, NULL, hInst, (LPVOID) this);
	SetFocus(m_hWnd);
}

void CXUIColorPicker::OnMouseMove(DWORD keys, int x, int y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	{
		rcClient.top += 20;
		rcClient.bottom -= 2;
		rcClient.left += 2;
		rcClient.right += 2;
		if(PtInRect(&rcClient, pt))
		{
			int xx = (x - rcClient.left) / COLOR_BOX_SIZE;
			int yy = (y - rcClient.top) / COLOR_BOX_SIZE;
			if(xx >=0 && xx < COLORS_H && yy >=0 && yy < COLORS_V)
			{
				m_color = m_colors[yy][xx];
				DrawColorBar(NULL);
				SetSelectedColor(xx, yy);
			}
		}
	}
}

void CXUIColorPicker::DrawColorBar(HDC hdc)
{
	BOOL releaseDC = FALSE;
	if(!hdc)
	{
		hdc = GetDC(m_hWnd);
		releaseDC = TRUE;
	}

	HBRUSH br = CreateSolidBrush(m_color);

	RECT rc;
	rc.top = 2;
	rc.bottom = 18;
	rc.left = 2;
	rc.right = 50;

	HPEN pen = CreatePen(PS_SOLID, 1, 0);
	HPEN   oldPen = (HPEN) SelectObject(hdc, pen);
	HBRUSH oldBrush = (HBRUSH) SelectObject(hdc, br);

	Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

	SelectObject(hdc, oldPen);
	SelectObject(hdc, oldBrush);

	DeleteObject(br);
	DeleteObject(pen);

	if(releaseDC)
	{
		ReleaseDC(m_hWnd, hdc);
	}
}

void CXUIColorPicker::DrawSelectedColor(HDC hdc, BOOL remove)
{
	if(selectedX < 0 || selectedY < 0) return;
	HPEN pen;
	if(remove)
	{
		pen = CreatePen(PS_SOLID, 1, 0); 
	} else
	{
		pen = CreatePen(PS_SOLID, 1, RGB(0xFF, 0xFF, 0xFF));
	}
	HBRUSH br = CreateSolidBrush(m_colors[selectedY][selectedX]);

	HPEN   oldPen = (HPEN) SelectObject(hdc, pen);
	HBRUSH oldBrush = (HBRUSH) SelectObject(hdc, br);

	RECT rc;
	rc.top = selectedY * COLOR_BOX_SIZE + 20; rc.left = selectedX * COLOR_BOX_SIZE + 1;
	rc.bottom = rc.top + COLOR_BOX_SIZE + 1;
	rc.right = rc.left + COLOR_BOX_SIZE + 1;

	Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

	SelectObject(hdc, oldPen);
	SelectObject(hdc, oldBrush);

	DeleteObject(pen);
	DeleteObject(br);
}

void CXUIColorPicker::SetSelectedColor(int x, int y)
{
	if(x != selectedX || y != selectedY)
	{
		HDC dc = GetDC(m_hWnd);
		DrawSelectedColor(dc, TRUE);
		selectedY = y;
		selectedX = x;
		DrawSelectedColor(dc, FALSE);
		ReleaseDC(m_hWnd, dc);
	}
}

void CXUIColorPicker::OnLButtonDown(int x, int y)
{
	RECT rcButton;
	GetClientRect(m_hWnd, &rcButton);
	rcButton.top += 2; rcButton.bottom = rcButton.top + 16;
	rcButton.right -= 2; rcButton.left = rcButton.right - 16;

	RECT rcNoneButton = rcButton;
	rcNoneButton.right -= 18;
	rcNoneButton.left  -= 18;

	POINT pt;
	pt.x = x, pt.y = y;
	BOOL destroy = FALSE;
	BOOL colorSelected = FALSE;
	if(PtInRect(&rcButton, pt))
	{
		PostMessage(m_hWndParent, WM_USER + 2, m_color, 0);
		destroy = TRUE;
	} else 
	{
		if(m_enableNone && PtInRect(&rcNoneButton, pt))
		{
			m_color = 0xFFFFFFFF;
			destroy = TRUE;
			colorSelected = TRUE;
		} else
		{
			RECT rcColors;
			GetClientRect(m_hWnd, &rcColors);
			if(!PtInRect(&rcColors, pt))
			{
				destroy = TRUE;
			}
			rcColors.top += 20;
			rcColors.bottom -= 2;
			rcColors.left += 2;
			rcColors.right += 2;
			if(PtInRect(&rcColors, pt))
			{
				destroy = TRUE;
				colorSelected = TRUE;
			}
		}
	}
	if(colorSelected == TRUE)
	{
		SendMessage(m_hWndParent, WM_USER + 1, m_color, 0);
	}
	if(destroy)
	{
		DestroyWindow(m_hWnd);
	}
}

typedef HANDLE (WINAPI *MonitorFromWindowFNC)(HWND hwnd, DWORD dwFlags);
typedef BOOL (WINAPI *GetMonitorInfoFNC)(HANDLE hMonitor, LPMONITORINFO lpmi);

void CXUIColorPicker::GetDesktopRect(RECT* rcDsk, HWND hWnd)
{
	int nMonitors = GetSystemMetrics(80);
	if(!nMonitors) nMonitors = 0;
	if(nMonitors == 1)
	{
oneMonitor:
		HWND dskWnd = GetDesktopWindow();
		GetClientRect(dskWnd, rcDsk);
	} else
	{
		HMODULE hUser32 = LoadLibrary(TEXT("user32.dll"));
		MonitorFromWindowFNC getMonitor = (MonitorFromWindowFNC) GetProcAddress(hUser32, "MonitorFromWindow");
		if(!getMonitor)
		{
			FreeLibrary(hUser32);
			goto oneMonitor;
		}
		HANDLE hMonitor = (*getMonitor)(hWnd, 0x00000002);
		if(!hMonitor)
		{
			FreeLibrary(hUser32);
			goto oneMonitor;
		}
		GetMonitorInfoFNC getMonitorInfo = (GetMonitorInfoFNC) GetProcAddress(hUser32, "GetMonitorInfoA");
		MONITORINFO mInf;
		mInf.cbSize = sizeof(MONITORINFO);
		(*getMonitorInfo)(hMonitor, &mInf);
		*rcDsk = mInf.rcMonitor;
		FreeLibrary(hUser32);
	}
}

CXUIColorPicker::CXUIColorPicker()
{
	m_enableNone = TRUE;
	m_color = 0;
	m_hWnd = NULL;
	selectedX = -1;
	selectedY = -1;

	int rVal = 0;
	int gVal = 0;
	int bVal = 0;
	for(int i=1; i <= 18; i++)
	{
		for(int j=1; j <= 12; j++)
		{
			switch(i)
			{
			case 1:
			case 12:
			case 13:
				rVal = 0xFF;
				break;
			case 2:
			case 11:
			case 14:
				rVal = 0xCC;
				break;
			case 3:
			case 10:
			case 15:
				rVal = 0x99;
				break;
			case 4:
			case 9:
			case 16:
				rVal = 0x66;
				break;
			case 5:
			case 8:
			case 17:
				rVal = 0x33;
				break;
			case 6:
			case 7:
			case 18:
				rVal = 0x00;
				break;
			}
			switch(j)
			{
			case 6:
			case 7:
				gVal = 0xFF;
				break;
			case 5:
			case 8:
				gVal = 0xCC;
				break;
			case 4:
			case 9:
				gVal = 0x99;
				break;
			case 3:
			case 10:
				gVal = 0x66;
				break;
			case 2:
			case 11:
				gVal = 0x33;
				break;
			case 1:
			case 12:
				gVal = 0x00;
				break;
			}
			if(j <= 6)
			{
				if(i <= 6) bVal = 0xFF;
				else if(i <= 12) bVal = 0xCC;
				else if(i <= 18) bVal = 0x99;
			} else
			{
				if(i <= 6) bVal = 0x66;
				else if(i <= 12) bVal = 0x33;
				else if(i <= 18) bVal = 0x00;
			}
			m_colors[j-1][i-1] = RGB(rVal, gVal, bVal);
		}
	}
	int clr = 0;
	int step = 5;
	for(int j=12; j < COLORS_V; j++)
	{
		for(int i=0; i < 18; i++)
		{
			m_colors[j][i] = RGB(clr, clr, clr);
			clr += step;
			if(clr > 255) clr = 255;
		}
	}
}

CXUIColorPicker::~CXUIColorPicker( void )
{

}

LRESULT CALLBACK CXUIColorPicker::WndProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	CXUIColorPicker* pThis = NULL;
	if(IsWindow(hWnd))
	{
		pThis = (CXUIColorPicker*)GetProp(hWnd, TEXT("clrpick_this"));
	}

	if(pThis || uMessage == WM_CREATE)
	{
		switch (uMessage)
		{
		case WM_KEYDOWN:
			if(wParam == VK_ESCAPE)
			{
				DestroyWindow(hWnd);
				return TRUE;
			}
			break;
		case WM_ACTIVATE:
			SetCapture(hWnd);
			break;
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
				pThis = (CXUIColorPicker*)(lpcs->lpCreateParams);
				SetProp(hWnd, TEXT("clrpick_this"), (HANDLE) pThis);
				pThis->m_hWnd = hWnd;
			}
			break;
		case WM_PAINT:
			pThis->OnPaint();
			break;
			//return TRUE;
		case WM_LBUTTONDOWN:
			pThis->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_NCHITTEST:
			return HTCLIENT;
		case WM_MOUSEMOVE:
			pThis->OnMouseMove((DWORD) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return TRUE;
		}
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CXUIColor::CXUIColor(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	WNDCLASS wc;
	if(!GetClassInfo(engine->get_hInstance(), XUI_COLORPICKER_CLASS, &wc))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style			= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS;
		wc.lpfnWndProc		= (WNDPROC) CXUIColorPicker::WndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= engine->get_hInstance();
		wc.hIcon			= NULL;
		wc.hCursor			= LoadCursor(engine->get_hInstance(), IDC_ARROW);
		wc.hbrBackground	= (HBRUSH)(COLOR_MENU+1);
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= XUI_COLORPICKER_CLASS;

		RegisterClass(&wc);
	}
	if(!GetClassInfo(engine->get_hInstance(), XUI_COLORBUTTON_CLASS, &wc))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style			= CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= (WNDPROC) CXUIColor::WndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= engine->get_hInstance();
		wc.hIcon			= NULL;
		wc.hCursor			= LoadCursor(engine->get_hInstance(), IDC_ARROW);
		wc.hbrBackground	= NULL;//(HBRUSH)(COLOR_MENU+1);
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= XUI_COLORBUTTON_CLASS;

		RegisterClass(&wc);
	}
	m_picker		= new CXUIColorPicker;
	m_color			= 0xFFFFFFFF;
	m_enableNone	= FALSE;
}

CXUIColor::~CXUIColor(void)
{
	delete m_picker;
}

LRESULT CALLBACK CXUIColor::WndProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	CXUIColor* pThis = NULL;
	if(IsWindow(hWnd))
	{
		pThis = (CXUIColor*)GetProp(hWnd, TEXT("CXUIColorThis"));
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
			RemoveProp(hWnd, TEXT("CXUIColorThis"));
			return 0;
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
				pThis = (CXUIColor*) lpcs->lpCreateParams;
				SetProp(hWnd, TEXT("CXUIColorThis"), (HANDLE) pThis);
				pThis->m_hWnd = hWnd;
			}
			break;
		case WM_PAINT:
			pThis->OnPaint();
			return TRUE;
		case WM_LBUTTONDOWN:
			pThis->OnClick();
			return TRUE;
		case WM_USER + 1:
			pThis->m_color = (COLORREF) wParam;
			pThis->raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case WM_USER + 2:
			pThis->SelectColor();
			break;
		}
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void CXUIColor::OnPaint(void)
{
	PAINTSTRUCT ps;
	BeginPaint(m_hWnd, &ps);

	COLORREF dkShadow = GetSysColor(COLOR_3DDKSHADOW);
	COLORREF Shadow = GetSysColor(COLOR_3DFACE);
	COLORREF ltShadow = GetSysColor(COLOR_3DHILIGHT);
	COLORREF arrow = GetSysColor(COLOR_BTNTEXT);

	HPEN penDkShadow = CreatePen(PS_SOLID, 1, dkShadow);
	HPEN penShadow = CreatePen(PS_SOLID, 1, Shadow);
	HPEN penLtShadow = CreatePen(PS_SOLID, 1, ltShadow);
	HPEN penArrow = CreatePen(PS_SOLID, 1, arrow);

	HBRUSH brColor = NULL;
	if(m_color != 0xFFFFFFFF) 
	{
		brColor = CreateSolidBrush(m_color);
	} else
	{
		brColor = CreateSolidBrush(RGB(0xCC, 0xCC, 0xCC));
	}
	HBRUSH brShadow = CreateSolidBrush(Shadow);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	rcClient.right--;
	rcClient.bottom--;

	if(rcClient.right != rcClient.top) 
	{
		int w = min(rcClient.right, rcClient.bottom);
		rcClient.right = rcClient.bottom = w;
	}

	FillRect(ps.hdc, &rcClient, brColor);

	if(m_color == 0xFFFFFFFF)
	{
		HPEN redPen = CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
		HPEN oldPen = (HPEN) SelectObject(ps.hdc, redPen);
		MoveToEx(ps.hdc, rcClient.left, rcClient.bottom, NULL);
		LineTo(ps.hdc, rcClient.right, rcClient.top);
		SelectObject(ps.hdc, oldPen);
		DeleteObject(redPen);
	}


	HPEN oldPen = (HPEN) SelectObject(ps.hdc, penLtShadow);
	MoveToEx(ps.hdc, rcClient.left, rcClient.bottom, NULL);
	LineTo(ps.hdc, rcClient.left, rcClient.top);
	LineTo(ps.hdc, rcClient.right, rcClient.top);

	SelectObject(ps.hdc, penDkShadow);
	LineTo(ps.hdc, rcClient.right, rcClient.bottom);
	LineTo(ps.hdc, rcClient.left, rcClient.bottom);

	rcClient.left++; rcClient.right--;
	rcClient.top++;  rcClient.bottom--;

	SelectObject(ps.hdc, penShadow);
	MoveToEx(ps.hdc, rcClient.left, rcClient.bottom, NULL);
	LineTo(ps.hdc, rcClient.left, rcClient.top);
	LineTo(ps.hdc, rcClient.right, rcClient.top);
	LineTo(ps.hdc, rcClient.right, rcClient.bottom);
	LineTo(ps.hdc, rcClient.left, rcClient.bottom);

	rcClient.left++; rcClient.right--;
	rcClient.top++;  rcClient.bottom--;

	SelectObject(ps.hdc, penDkShadow);
	MoveToEx(ps.hdc, rcClient.left, rcClient.bottom, NULL);
	LineTo(ps.hdc, rcClient.left, rcClient.top);
	LineTo(ps.hdc, rcClient.right, rcClient.top);

	SelectObject(ps.hdc, penLtShadow);
	LineTo(ps.hdc, rcClient.right, rcClient.bottom);
	LineTo(ps.hdc, rcClient.left, rcClient.bottom);

	rcClient.top = rcClient.bottom - 4;
	rcClient.left = rcClient.right - 6;
	SelectObject(ps.hdc, penLtShadow);
	MoveToEx(ps.hdc, rcClient.left, rcClient.bottom, NULL);
	LineTo(ps.hdc, rcClient.left, rcClient.top);
	LineTo(ps.hdc, rcClient.right, rcClient.top);

	rcClient.left++; rcClient.right++;
	rcClient.top++;  rcClient.bottom++;

	FillRect(ps.hdc, &rcClient, brShadow);

	int x = rcClient.left + 1;
	int y = rcClient.top + 1;
	SelectObject(ps.hdc, penArrow);

	MoveToEx(ps.hdc, x, y, NULL);
	LineTo(ps.hdc, x + 5, y);

	MoveToEx(ps.hdc, x + 1, y + 1, NULL);
	LineTo(ps.hdc, x + 4, y + 1);

	MoveToEx(ps.hdc, x + 2, y + 2, NULL);
	LineTo(ps.hdc, x + 3, y + 2);

	SelectObject(ps.hdc, oldPen);

	DeleteObject(penDkShadow);
	DeleteObject(penShadow);
	DeleteObject(penLtShadow);
	DeleteObject(brShadow);
	DeleteObject(brColor);
	DeleteObject(penArrow);

	EndPaint(m_hWnd, &ps);
}

void CXUIColor::OnClick(void)
{
	m_picker->m_enableNone = m_enableNone;
	m_picker->Create(m_hWnd, m_color, m_engine->get_hInstance());
}

void CXUIColor::SelectColor(void)
{
	COLORREF custColors[16];
	ZeroMemory(custColors, sizeof(custColors));
	CHOOSECOLOR cc;
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.Flags = CC_RGBINIT | CC_ANYCOLOR | CC_FULLOPEN;
	cc.hwndOwner = m_hWnd;
	cc.rgbResult = m_color;
	cc.lpCustColors = custColors;
	if(ChooseColor(&cc))
	{
		m_color = cc.rgbResult;
		InvalidateRect(m_hWnd, NULL, TRUE);
		raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
	}
}

void CXUIColor::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP;

	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	m_hWnd = CreateWindowEx(0, XUI_COLORBUTTON_CLASS, TEXT(""), wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), (LPVOID) this);

	RECT rcDlg = {0, 0, 14, 14};
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minWidth  = rcDlg.right;
	m_minHeight = rcDlg.bottom;
}

BOOL CXUIColor::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	m_enableNone = xmlGetAttributeValueBOOL(node,	TEXT("enableNone"),		FALSE);
	return TRUE;
}

INT CXUIColor::value_INT()
{
	return (INT) m_color;
}

void CXUIColor::value_INT( INT val )
{
	m_color = (COLORREF) val;
	InvalidateRect(m_hWnd, NULL, TRUE);
}