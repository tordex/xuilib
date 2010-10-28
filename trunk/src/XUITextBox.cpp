#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>

CXUITextBox::CXUITextBox(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_value			= NULL;
	m_bLockEvents	= FALSE;
}

CXUITextBox::~CXUITextBox(void)
{
	if(m_value) delete m_value;
}

BOOL CXUITextBox::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_value) delete m_value;
	m_value = xmlGetAttributeSTR(node, TEXT("value"));

	m_readonly		=	xmlGetAttributeValueBOOL(node,	TEXT("readonly"),				VARIANT_FALSE);
	m_multiline		=	xmlGetAttributeValueBOOL(node,	TEXT("multiline"),				VARIANT_FALSE);
	m_maxlength		=	xmlGetAttributeValueNonSTR<int> (node,	TEXT("maxlength"),		0);
	m_rows			=	xmlGetAttributeValueNonSTR<int> (node,	TEXT("textrows"),		1);
	m_cols			=	xmlGetAttributeValueNonSTR<int> (node,	TEXT("textcols"),		10);
	m_size			=	xmlGetAttributeValueNonSTR<int> (node,	TEXT("size"),			10);
	m_numbers		=	xmlGetAttributeValueBOOL(node,	TEXT("numberonly"),				VARIANT_FALSE);
	m_password		=	xmlGetAttributeValueBOOL(node,	TEXT("password"),				VARIANT_FALSE);
	m_textAlign		=	xmlGetAttributeValueSTRArray(node, TEXT("textalign"), XUI_TEXTBOX_ALIGN_LEFT, L"left\0right\0center\0");

	m_bWantReturn	=	xmlGetAttributeValueBOOL(node,	TEXT("wantReturn"),		VARIANT_FALSE);
	m_bAutoHScroll	=	xmlGetAttributeValueBOOL(node,	TEXT("autoHScroll"),	m_multiline ? VARIANT_FALSE : VARIANT_TRUE);
	m_bAutoVScroll	=	xmlGetAttributeValueBOOL(node,	TEXT("autoVScroll"),	VARIANT_FALSE);
	m_bHScroll		=	xmlGetAttributeValueBOOL(node,	TEXT("hScroll"),		VARIANT_FALSE);
	m_bVScroll		=	xmlGetAttributeValueBOOL(node,	TEXT("vScroll"),		VARIANT_FALSE);

	return TRUE;
}

void CXUITextBox::Init()
{
	DWORD wStyle = WS_CHILD | WS_TABSTOP;

	if(get_disabled())	wStyle |= WS_DISABLED;
	if(!get_hidden())	wStyle |= WS_VISIBLE;
	if(m_readonly)		wStyle |= ES_READONLY;
	if(m_multiline)		wStyle |= ES_MULTILINE;
	if(m_bAutoHScroll)	wStyle |= ES_AUTOHSCROLL;
	if(m_bAutoVScroll)	wStyle |= ES_AUTOVSCROLL;
	if(m_bWantReturn)	wStyle |= ES_WANTRETURN;
	if(m_bHScroll)		wStyle |= WS_HSCROLL;
	if(m_bVScroll)		wStyle |= WS_VSCROLL;
	if(m_numbers)		wStyle |= ES_NUMBER;
	if(m_password)		wStyle |= ES_PASSWORD;
	switch(m_textAlign)
	{
	case XUI_TEXTBOX_ALIGN_LEFT:
		wStyle |= ES_LEFT;
		break;
	case XUI_TEXTBOX_ALIGN_RIGHT:
		wStyle |= ES_RIGHT;
		break;
	case XUI_TEXTBOX_ALIGN_CENTER:
		wStyle |= ES_CENTER;
		break;
	}

	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), m_value, wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);

	if(m_maxlength)
	{
		SendMessage(m_hWnd, EM_LIMITTEXT, m_maxlength, NULL);
	}

	SetWindowFont(m_hWnd, getFont(), TRUE);

	int rows = 1;
	int cols = m_size;
	if(m_multiline)
	{
		rows = m_rows;
		cols = m_cols;
	}

	if(!m_minWidth)
	{
		HDC hdc = GetDC(m_hWnd);
		HFONT oldFont = (HFONT) SelectObject(hdc, getFont());
		LPWSTR testStr = new WCHAR[rows * (cols + 2)];
		int k=0;
		for(int i=0; i < rows; i++)
		{
			for(int j=0; j < cols; j++)
			{
				testStr[k] = L'W';
				k++;
			}
			testStr[k] = L'\n';
			k++;
		}
		testStr[k] = 0;
		RECT rcDraw = {0, 0, 3, 3};
		DrawText(hdc, testStr, -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
		m_minWidth	= rcDraw.right - rcDraw.left + 6;
		m_minHeight = rcDraw.bottom - rcDraw.top + 6;

		SelectObject(hdc, oldFont);
		ReleaseDC(m_hWnd, hdc);
	} else
	{
		RECT rcDlg = {0, 0, 1, 14};
		rcDlg.right = m_minWidth;
		MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
		m_minWidth = rcDlg.right;
		m_minHeight = rcDlg.bottom * rows;
	}

	if(!m_multiline)
	{
		RECT rcDlg = {0, 0, 1, 14};
		MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
		m_minHeight = rcDlg.bottom * rows;
	}

	CXUIElement::Init();
}

HWND CXUITextBox::get_parentWnd()
{
	if(m_parent)
	{
		return m_parent->get_parentWnd();
	}
	return NULL;
}

INT CXUITextBox::value_INT()
{
	LPCWSTR str = value_STR();
	int ret = 0;
	if(str)
	{
		ret = _wtoi(str);
	}
	return ret;
}

void CXUITextBox::value_INT( INT val )
{
	WCHAR txt[255];
	_itow_s(val, txt, 10);
	value_STR(txt);
	processDefaultAction();
}

LPCWSTR CXUITextBox::value_STR()
{
	if(m_value) delete m_value;
	m_value = NULL;
	int txtLen = (int) SendMessage(m_hWnd, WM_GETTEXTLENGTH, 0, 0);
	if(txtLen)
	{
		m_value = new TCHAR[txtLen + 1];
		SendMessage(m_hWnd, WM_GETTEXT, (WPARAM) (txtLen + 1), (LPARAM) m_value);
	}
	return m_value;
}

void CXUITextBox::value_STR( LPCWSTR val )
{
	m_bLockEvents = TRUE;
	if(val)
	{
		SetWindowText(m_hWnd, val);
	} else
	{
		SetWindowText(m_hWnd, TEXT(""));
	}
	m_bLockEvents = FALSE;
	processDefaultAction();
}

BOOL CXUITextBox::onCommnd( UINT code, UINT id, HWND hWnd )
{
	if(code == EN_CHANGE && !m_bLockEvents)
	{
		processDefaultAction();
		return raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
	}
	return FALSE;
}