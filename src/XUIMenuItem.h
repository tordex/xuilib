#pragma once
#include "xuielement.h"

class CXUIMenuItem : public CXUIElement
{
	LPWSTR	m_label;
	BOOL	m_isRadio;
	BOOL	m_isChecked;
	BOOL	m_isDefault;
public:
	CXUIMenuItem(CXUIElement* parent, CXUIEngine* engine);
	~CXUIMenuItem(void);
	IMPLEMENT_INTERFACE(L"menuitem")

	virtual BOOL loadDATA(IXMLDOMNode* node);
	virtual void value_INT(INT val);
	virtual BOOL OnEvent(CXUIElement* el, LPCWSTR evID, WPARAM wParam, LPARAM lParam);
	virtual void updateDisabledState();
};
