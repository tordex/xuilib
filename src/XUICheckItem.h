#pragma once
#include "xuielement.h"

class CXUICheckItem : public CXUIElement
{
	LPWSTR	m_label;
	INT		m_value;
public:
	CXUICheckItem(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUICheckItem(void);

	IMPLEMENT_INTERFACE(L"checkitem")

	BOOL loadDATA(IXMLDOMNode* node);

	LPWSTR	get_label()	{ return m_label;	}
	int		get_value()	{ return m_value;	}
};
