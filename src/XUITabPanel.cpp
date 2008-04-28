#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>
#include <commctrl.h>
#include <Uxtheme.h>

CXUITabPanel::CXUITabPanel(CXUIElement* parent, CXUIEngine* engine) : CXUIDialog(NULL, engine, parent)
{
	m_label = NULL;
}

CXUITabPanel::~CXUITabPanel(void)
{
	if(m_label) delete m_label;
}

BOOL CXUITabPanel::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_label) delete m_label;
	m_label =		xmlGetAttributeSTR(node, TEXT("label"));
	//m_flex	=		xmlGetAttributeValueNonSTR<int>(node,	TEXT("flex"),		1);
	return TRUE;
}

void CXUITabPanel::Init()
{
	m_style = WS_VISIBLE | DS_SETFONT | WS_CHILD;
	m_exStyle = WS_EX_CONTROLPARENT;
	m_hDlg = createDialog(m_left, m_top, m_width, m_height);
	m_hWnd = CreateDialogIndirectParam(NULL, m_hDlg, m_parent->get_parentWnd(), (DLGPROC) CXUIDialog::WndProc, (LPARAM)this);
	EnableThemeDialogTexture(m_hWnd, ETDT_ENABLETAB);

	RECT rcMargins = {7, 7, 7, 7};
	MapDialogRect(m_hWnd, &rcMargins);

	m_marginLeft	= rcMargins.left;
	m_marginRight	= rcMargins.right;
	m_marginTop		= rcMargins.top;
	m_marginBottom	= rcMargins.bottom;
}

void CXUITabPanel::OnInitDialog()
{
	CXUIElement::Init();
}

void CXUITabPanel::set_hidden(int hide)
{
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
