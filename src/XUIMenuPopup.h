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

	virtual void	Init();
	virtual BOOL	loadDATA(IXMLDOMNode* node);
	virtual void	doDefaultAction(CXUIElement* el);
	virtual BOOL	OnEvent(CXUIElement* el, LPCWSTR evID, WPARAM wParam, LPARAM lParam);

	HMENU get_menu()	{ return m_hMenu;	}

};
