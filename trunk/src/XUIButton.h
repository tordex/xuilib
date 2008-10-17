#pragma once
#include "xuielement.h"

#define XUI_BTN_DGLTYPE_NONE		0
#define XUI_BTN_DGLTYPE_ACCEPT		1
#define XUI_BTN_DGLTYPE_CANCEL		2
#define XUI_BTN_DGLTYPE_HELP		3
#define XUI_BTN_DGLTYPE_DISCLOSURE	4


class CXUIButton : public CXUIElement
{
	LPWSTR	m_label;
	int		m_dlgType;
	BOOL	m_bDefault;
public:
	CXUIButton(CXUIElement* parent, CXUIEngine* engine);
	~CXUIButton(void);

	IMPLEMENT_INTERFACE(L"button")

	virtual BOOL	loadDATA(IXMLDOMNode* node);
	virtual void	Init();
	virtual BOOL	onCommnd(UINT code, UINT id, HWND hWnd);
	virtual void	value_STR(LPCWSTR val);

	void set_default(BOOL isDefault);
};
