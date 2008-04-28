#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>

CXUIListItem::CXUIListItem(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_label = NULL;
}

CXUIListItem::~CXUIListItem(void)
{
	if(m_label) delete m_label;
}

BOOL CXUIListItem::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_label) delete m_label;
	m_label = getLocalizedString(node, TEXT("label"));
	m_value	= xmlGetAttributeValueNonSTR<int>(node,	TEXT("value"),	0);
	return TRUE;
}
