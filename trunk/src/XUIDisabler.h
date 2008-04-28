#pragma once
#include "xuielement.h"

#define XUI_DISABLER_TYPE_INTVAL		0
#define XUI_DISABLER_TYPE_EMPTYSTR		1
#define XUI_DISABLER_TYPE_NONEMPTYSTR	2

class CXUIDisabler : public CXUIElement
{
	BOOL	m_bDisable;
	LPWSTR	m_elID;
	INT		m_type;
	INT		m_intVal;
public:
	CXUIDisabler(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIDisabler(void);

	IMPLEMENT_INTERFACE(L"disabler")

	virtual BOOL loadDATA(IXMLDOMNode* node);
	virtual void doDefaultAction(CXUIElement* el);
};
