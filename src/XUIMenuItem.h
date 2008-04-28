#pragma once
#include "xuielement.h"

class CXUIMenuItem : public CXUIElement
{
	LPWSTR m_label;
public:
	CXUIMenuItem(CXUIElement* parent, CXUIEngine* engine);
	~CXUIMenuItem(void);
	IMPLEMENT_INTERFACE(L"menuitem")

	BOOL loadDATA(IXMLDOMNode* node);
	LPCWSTR get_label() { return m_label; }
};
