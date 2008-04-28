#include "xuilib.h"
#include "XUICheckItem.h"
#include "xmltools.h"

CXUICheckItem::CXUICheckItem(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_label	= NULL;
	m_value	= 0;
}

CXUICheckItem::~CXUICheckItem(void)
{
	if(m_label)	delete m_label;
}

BOOL CXUICheckItem::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_label)	delete m_label;

	m_label	= getLocalizedString(node, TEXT("label"));
	m_value	= xmlGetAttributeValueNonSTR<int>(node,	TEXT("value"),	0);

	return TRUE;
}