#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>

CXUIMenuItem::CXUIMenuItem(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_label = NULL;
}

CXUIMenuItem::~CXUIMenuItem(void)
{
	if(m_label) delete m_label;
}

BOOL CXUIMenuItem::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_label) delete m_label;
	m_label = xmlGetAttributeSTR(node, TEXT("label"));
	return TRUE;
}
