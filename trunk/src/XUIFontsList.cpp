#include "xuilib.h"
#include "XUIFontsList.h"

CXUIFontsList::CXUIFontsList(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
}

CXUIFontsList::~CXUIFontsList(void)
{
}

void CXUIFontsList::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | CBS_SORT | CBS_OWNERDRAWFIXED;

	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;

	m_hWnd = CreateWindowEx(0, TEXT("COMBOBOX"), NULL, wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, NULL, NULL);
	SetWindowFont(m_hWnd, GetStockObject(DEFAULT_GUI_FONT), TRUE);
	m_minWidth = 0;
	m_minHeight = 0;

	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT) SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
	for(int i=0; i < m_childCount; i++)
	{
		CXUIListItem* cbi = NULL;
		if(m_childs[i]->QueryElement(L"listitem", (LPVOID*) &cbi))
		{
			int idx = SendMessage(m_hWnd, CB_ADDSTRING, NULL, (LPARAM) cbi->get_label());
			SendMessage(m_hWnd, CB_SETITEMDATA, idx, (LPARAM) cbi->get_value());

			RECT rcDraw = {0, 0, 3, 3};
			DrawText(hdc, cbi->get_label(), -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
			if(m_minWidth < rcDraw.right) m_minWidth = rcDraw.right;
		}
	}
	SelectObject(hdc, oldFont);
	ReleaseDC(NULL, hdc);
	if(!m_minWidth)
	{
		HDC hdc = GetDC(m_hWnd);
		HFONT oldFont = (HFONT) SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
		RECT rcDraw = {0, 0, 3, 3};
		LPSTR label = new CHAR[m_size + 1];
		memset(label, 'W', m_size);
		label[m_size] = 0;
		DrawTextA(hdc, label, -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
		delete label;
		m_minHeight = rcDraw.bottom - rcDraw.top;
		m_minWidth = rcDraw.right - rcDraw.left;
		SelectObject(hdc, oldFont);
		ReleaseDC(m_hWnd, hdc);
	}
	RECT rcDlg = {0, 0, 1, 14};
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minHeight = rcDlg.bottom;
	m_minWidth += 26;
}