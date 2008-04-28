#include "xuilib.h"
#include "XUIDisabler.h"
#include "xmltools.h"

CXUIDisabler::CXUIDisabler(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_elID = NULL;
}

CXUIDisabler::~CXUIDisabler(void)
{
	if(m_elID) delete m_elID;
}

BOOL CXUIDisabler::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_elID) delete m_elID;
	m_elID		= xmlGetAttributeSTR(node,					TEXT("applyto"));
	m_bDisable  = xmlGetAttributeValueBOOL(node,			TEXT("disable"),	VARIANT_TRUE);
	m_type		= xmlGetAttributeValueSTRArray(node,		TEXT("type"), XUI_DISABLER_TYPE_INTVAL, L"int\0emptystring\0nonemptystring\0");
	m_intVal	= xmlGetAttributeValueNonSTR<INT>(node,		TEXT("intval"),	0);
	return TRUE;
}

void CXUIDisabler::doDefaultAction( CXUIElement* el )
{
	BOOL isOK = FALSE;
	switch(m_type)
	{
	case XUI_DISABLER_TYPE_INTVAL:
		if(el->value_INT() == m_intVal)
		{
			isOK = TRUE;
		}
		break;
	case XUI_DISABLER_TYPE_EMPTYSTR:
		if(!el->value_STR())
		{
			isOK = TRUE;
		}
		break;
	case XUI_DISABLER_TYPE_NONEMPTYSTR:
		if(el->value_STR())
		{
			isOK = TRUE;
		}
		break;
	default:
		return;
	}
	CXUIElement* applyto = get_root()->find(m_elID);
	if(isOK)
	{
		if(m_bDisable)
		{
			if(applyto)	applyto->set_disabled(TRUE);
		} else
		{
			if(applyto)	applyto->set_disabled(FALSE);
		}
	} else
	{
		if(m_bDisable)
		{
			if(applyto)	applyto->set_disabled(FALSE);
		} else
		{
			if(applyto)	applyto->set_disabled(TRUE);
		}
	}
}