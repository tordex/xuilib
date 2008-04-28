#pragma once
#include "xuielement.h"

class CXUIMenuPopup : public CXUIElement
{
	HMENU	m_hMenu;
	LPWSTR  m_label;
public:
	CXUIMenuPopup(CXUIElement* parent, CXUIEngine* engine);
	~CXUIMenuPopup(void);
	IMPLEMENT_INTERFACE(L"menupopup")

	void Init();
	BOOL loadDATA(IXMLDOMNode* node);

	HMENU get_menu() { return m_hMenu; }
	LPCWSTR get_label() { return m_label; }
};
