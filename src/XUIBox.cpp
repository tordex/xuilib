#include "xuilib.h"
#include "xmltools.h"

CXUIBox::CXUIBox(CXUIElement* parent, CXUIEngine* engine, int defOrient) : CXUIElement(parent, engine)
{
	m_defOrient = defOrient;
}

CXUIBox::~CXUIBox(void)
{
}


int	CXUIBox::get_margin(int mgidx)
{
	return 3;
}

BOOL CXUIBox::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_defOrient >= 0)
	{
		m_orient = m_defOrient;
	}
	m_marginLeft	=	xmlGetAttributeValueNonSTR<INT>(node, L"marginLeft",	0);
	m_marginRight	=	xmlGetAttributeValueNonSTR<INT>(node, L"marginRight",	0);
	m_marginBottom	=	xmlGetAttributeValueNonSTR<INT>(node, L"marginBottom",	0);
	m_marginTop		=	xmlGetAttributeValueNonSTR<INT>(node, L"marginTop",		0);

	return TRUE;
}

void CXUIBox::Init()
{
	CXUIElement::Init();

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