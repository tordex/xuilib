#pragma once
#include "xuielement.h"

class CXUIListItem : public CXUIElement
{
	LPWSTR	m_label;
	int		m_value;
public:
	CXUIListItem(CXUIElement* parent, CXUIEngine* engine);
	~CXUIListItem(void);
	IMPLEMENT_INTERFACE(L"listitem")

	BOOL loadDATA(IXMLDOMNode* node);
	LPCWSTR get_label() { return m_label; }
	int		get_value() { return m_value; }
};
