#include "xuilib.h"
#include "XUIHotkey.h"
#include <CommCtrl.h>
#include <vssym32.h>
#include <Uxtheme.h>

CXUIHotkey::CXUIHotkey(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	WNDCLASS wc;
	if(!GetClassInfo(m_engine->get_hInstance(), XUI_HOTKEY_CLASS, &wc))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style          = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc    = (WNDPROC)CXUIHotkey::WndProc;
		wc.cbClsExtra     = 0;
		wc.cbWndExtra     = 0;
		wc.hInstance      = m_engine->get_hInstance();
		wc.hIcon          = NULL;
		wc.hCursor        = LoadCursor(NULL, IDC_IBEAM);
		wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName   = NULL;
		wc.lpszClassName  = XUI_HOTKEY_CLASS;

		RegisterClass(&wc);
	}

	m_hotKey = 0;
	m_text[0] = 0;
	FillString();
}

CXUIHotkey::~CXUIHotkey(void)
{
}

LRESULT CALLBACK CXUIHotkey::WndProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	CXUIHotkey* pThis = NULL;
	if(IsWindow(hWnd))
	{
		pThis = (CXUIHotkey*)GetProp(hWnd, TEXT("tlbhotkey_this"));
	}

	if(pThis || uMessage == WM_CREATE)
	{
		switch (uMessage)
		{
		case WM_GETDLGCODE:
			return DLGC_WANTTAB | DLGC_WANTARROWS | DLGC_WANTALLKEYS;
		case HKM_SETHOTKEY:
			pThis->m_hotKey = ( WORD )wParam;
			pThis->FillString();
			pThis->UpdateCarret();
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;
		case HKM_GETHOTKEY:
			return pThis->m_hotKey;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			{
				BOOL changed = TRUE;
				UINT vk = (UINT) MapVirtualKey((UINT) ((lParam >> 16) & 0xFF), 1);
				switch(vk)
				{
				case 0:
					break;
				case VK_CONTROL:
					if(!(pThis->m_hotKey & 0x0200)) changed  = TRUE;
					if(pThis->m_hotKey & 0xFF)
					{
						pThis->m_hotKey = 0;
						changed = TRUE;
					}
					pThis->m_hotKey |= 0x0200;
					break;
				case VK_MENU:
					if(!(pThis->m_hotKey & 0x0400)) changed  = TRUE;
					if(pThis->m_hotKey & 0xFF)
					{
						pThis->m_hotKey = 0;
						changed = TRUE;
					}
					pThis->m_hotKey |= 0x0400;
					break;
				case VK_SHIFT:
					if(!(pThis->m_hotKey & 0x0100)) changed  = TRUE;
					if(pThis->m_hotKey & 0xFF)
					{
						pThis->m_hotKey = 0;
						changed = TRUE;
					}
					pThis->m_hotKey |= 0x0100;
					break;
				case VK_LWIN:
				case VK_RWIN:
				case 0xF1:
				case 0xEA:
					if(!(pThis->m_hotKey & 0x2000)) changed  = TRUE;
					if(pThis->m_hotKey & 0xFF)
					{
						pThis->m_hotKey = 0;
						changed = TRUE;
					}
					pThis->m_hotKey |= 0x2000;
					break;
				default:
					if((pThis->m_hotKey & 0xFF) != vk) changed  = TRUE;
					pThis->m_hotKey &= ~0x08FF;
					pThis->m_hotKey |= vk;
					if(lParam & 0x01000000)
					{
						pThis->m_hotKey |= 0x0800;
					}
					break;
				}
				pThis->FillString();
				if(changed) 
				{
					pThis->UpdateCarret();
					InvalidateRect(hWnd, NULL, FALSE);
					pThis->raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
				}
			}
			return 0;
		case WM_SYSKEYUP:
		case WM_KEYUP:
			{
				BOOL changed = TRUE;
				UINT vk = (UINT) MapVirtualKey((UINT) ((lParam >> 16) & 0xFF), 1);
				switch(vk)
				{
				case 0:
					break;
				case VK_CONTROL:
					if(!(pThis->m_hotKey & 0xFF))
					{
						if((pThis->m_hotKey & 0x0200)) changed  = TRUE;
						pThis->m_hotKey &= ~0x0200;
					}
					break;
				case VK_MENU:
					if(!(pThis->m_hotKey & 0xFF))
					{
						if((pThis->m_hotKey & 0x0400)) changed  = TRUE;
						pThis->m_hotKey &= ~0x0400;
					}
					break;
				case VK_SHIFT:
					if(!(pThis->m_hotKey & 0xFF))
					{
						if((pThis->m_hotKey & 0x0100)) changed  = TRUE;
						pThis->m_hotKey &= ~0x0100;
					}
					break;
				case VK_LWIN:
				case VK_RWIN:
				case 0xF1:
				case 0xEA:
					if(!(pThis->m_hotKey & 0xFF))
					{
						if((pThis->m_hotKey & 0x2000)) changed  = TRUE;
						pThis->m_hotKey &= ~0x2000;
					}
					break;
				default:
					//if(pThis->m_hotKey & 0xFF) changed  = TRUE;
					//pThis->m_hotKey &= ~0xFF;
					break;
				}
				pThis->FillString();
				if(changed) 
				{
					pThis->UpdateCarret();
					InvalidateRect(hWnd, NULL, FALSE);
					pThis->raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
				}
			}
			return 0;
		case WM_KILLFOCUS:
			DestroyCaret();
			return 0;
		case WM_SETFOCUS:
			{
				RECT rcClient;
				GetClientRect(hWnd, &rcClient);
				CreateCaret(hWnd, NULL, 0, rcClient.bottom - rcClient.top - 4);
				pThis->UpdateCarret();
				ShowCaret(hWnd);
			}
			return 0;
		case WM_LBUTTONDOWN:
			SetFocus(hWnd);
			return 0;
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
				pThis = (CXUIHotkey*) lpcs->lpCreateParams;
				SetProp(hWnd, TEXT("tlbhotkey_this"), (HANDLE) pThis);
				pThis->m_hWnd = hWnd;
			}
			break;
		case WM_DESTROY:
			RemoveProp(hWnd, TEXT("tlbhotkey_this"));
			break;
		case WM_PAINT:
			pThis->OnPaint();
			break;
		}
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void CXUIHotkey::OnPaint( void )
{
	PAINTSTRUCT ps;
	BeginPaint(m_hWnd, &ps);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	BOOL stdDraw = TRUE;
	__try
	{
		HTHEME hTheme = OpenThemeData(NULL, L"EDIT");
		if(hTheme)
		{
			DrawThemeBackground(hTheme, ps.hdc, EP_EDITTEXT, ETS_NORMAL, &rcClient, &rcClient);
			CloseThemeData(hTheme);
			stdDraw = FALSE;
		}
	} __except(1)
	{
	}
	if(stdDraw)
	{
		HPEN pen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_GRAYTEXT));
		HPEN oldPen = (HPEN) SelectObject(ps.hdc, pen);
		SetBkColor(ps.hdc, GetSysColor(COLOR_WINDOW));
		SetBkMode(ps.hdc, OPAQUE);
		Rectangle(ps.hdc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
		SelectObject(ps.hdc, oldPen);
		DeleteObject(pen);
	}

	HFONT hFont = (HFONT) SendMessage(GetParent(m_hWnd), WM_GETFONT, 0, 0);
	HFONT oldFont = (HFONT) SelectObject(ps.hdc, hFont);

	SetBkMode(ps.hdc, TRANSPARENT);
	rcClient.left += 4;
	rcClient.right -= 4;
	DrawText(ps.hdc, m_text, -1, &rcClient, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	SelectObject(ps.hdc, oldFont);

	EndPaint(m_hWnd, &ps);
}

void CXUIHotkey::FillString( void )
{
	if(!m_hotKey)
	{
		lstrcpy(m_text, TEXT("None"));
		return;
	}
	m_text[0] = 0;
	//Ctrl
	if(m_hotKey & 0x0200)
	{
		lstrcat(m_text, TEXT("Ctrl + "));
	}
	//Shift
	if(m_hotKey & 0x0100)
	{
		lstrcat(m_text, TEXT("Shift + "));
	}
	//Alt
	if(m_hotKey & 0x0400)
	{
		lstrcat(m_text, TEXT("Alt + "));
	}
	//Win
	if(m_hotKey & 0x2000)
	{
		lstrcat(m_text, TEXT("Win + "));
	}
	TCHAR keyName[255];
	keyName[0] = 0;
	UINT scanCode = MapVirtualKey(m_hotKey & 0xFF, 0);
	if(m_hotKey & 0x0800)
	{
		scanCode |= 0x00000100;
	}
	if(!GetKeyNameText(scanCode << 16, keyName, 255))
	{
		if(m_hotKey & 0xFF)
		{
			wsprintf(keyName, TEXT("#%d"), m_hotKey & 0xFF);
		}
	}
	lstrcat(m_text, keyName);
}

void CXUIHotkey::UpdateCarret( void )
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	HFONT hFont = (HFONT) SendMessage(GetParent(m_hWnd), WM_GETFONT, 0, 0);
	HDC dc = GetDC(m_hWnd);
	HFONT oldFont = (HFONT) SelectObject(dc, hFont);

	rcClient.left += 4;
	rcClient.right -= 4;
	DrawText(dc, m_text, -1, &rcClient, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);

	SelectObject(dc, oldFont);
	ReleaseDC(m_hWnd, dc);

	SetCaretPos(rcClient.right, 2);
}

void CXUIHotkey::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP;

	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	m_hWnd = CreateWindowEx(0, XUI_HOTKEY_CLASS, TEXT(""), wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), (LPVOID) this);

	RECT rcDlg = {0, 0, 80, 14};
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minWidth  = rcDlg.right;
	m_minHeight = rcDlg.bottom;

	CXUIElement::Init();
}

INT CXUIHotkey::value_INT()
{
	return (INT) m_hotKey;
}

void CXUIHotkey::value_INT( INT val )
{
	m_hotKey = (WORD) val;
	FillString();
	UpdateCarret();
	InvalidateRect(m_hWnd, NULL, FALSE);
}
