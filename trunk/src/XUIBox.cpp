#include "xuilib.h"

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
	return TRUE;
}
