#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>
#include <commctrl.h>
#include <Uxtheme.h>

CXUITabPanel::CXUITabPanel(CXUIElement* parent, CXUIEngine* engine) : CXUIDialog(NULL, engine, parent)
{
	m_label			= NULL;
	m_renderOnly	= TRUE;
}

CXUITabPanel::~CXUITabPanel(void)
{
	if(m_label) delete m_label;
}

BOOL CXUITabPanel::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_label) delete m_label;
	m_label = xmlGetAttributeSTR(node, TEXT("label"));

	m_marginLeft	=	xmlGetAttributeValueNonSTR<INT>(node, L"marginLeft",	7);
	m_marginRight	=	xmlGetAttributeValueNonSTR<INT>(node, L"marginRight",	7);
	m_marginBottom	=	xmlGetAttributeValueNonSTR<INT>(node, L"marginBottom",	7);
	m_marginTop		=	xmlGetAttributeValueNonSTR<INT>(node, L"marginTop",		7);

	return TRUE;
}

void CXUITabPanel::Init()
{
	m_style = DS_SETFONT | WS_CHILD | DS_CONTROL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	m_exStyle = WS_EX_CONTROLPARENT;

	if(!get_hidden())			m_style |= WS_VISIBLE;

	m_hDlg = createDialog(m_left, m_top, m_width, m_height);
	m_hWnd = CreateDialogIndirectParam(NULL, m_hDlg, m_parent->get_parentWnd(), (DLGPROC) CXUIDialog::WndProc, (LPARAM)this);
	EnableThemeDialogTexture(m_hWnd, ETDT_ENABLETAB);

	SetWindowPos(get_wnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	RECT rcDlg;
	rcDlg.left		= m_marginLeft;
	rcDlg.right		= m_marginRight;
	rcDlg.bottom	= m_marginBottom;
	rcDlg.top		= m_marginTop;
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_marginLeft	= rcDlg.left;
	m_marginRight	= rcDlg.right;
	m_marginBottom	= rcDlg.bottom;
	m_marginTop		= rcDlg.top;
}

void CXUITabPanel::OnInitDialog()
{
	CXUIElement::Init();
}

void CXUITabPanel::set_hidden(int hide)
{
	m_hidden = hide;
	if(m_hWnd)
	{
		if(hide)
		{
			ShowWindow(m_hWnd, SW_HIDE);
		} else
		{
			ShowWindow(m_hWnd, SW_SHOW);
		}
	}
}

void CXUITabPanel::render( int x, int y, int width, int height )
{
	if(m_hWnd)
	{
		SetWindowPos(m_hWnd, NULL, x, y, width, height, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOCOPYBITS | SWP_DEFERERASE);
	}
	CXUIElement::render(0, 
		0, 
		width, 
		height);
	m_left		= x;
	m_top		= y;
	m_width		= width;
	m_height	= height;
}
