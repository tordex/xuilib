#pragma once
#include "xuielement.h"

class CXUICheckbox : public CXUIElement
{
	LPWSTR	m_label;
	BOOL	m_checked;
	BOOL	m_bAuto;
	BOOL	m_bTriState;
public:
	CXUICheckbox(CXUIElement* parent, CXUIEngine* engine);
	~CXUICheckbox(void);
	IMPLEMENT_INTERFACE(L"checkbox")

	virtual BOOL	loadDATA(IXMLDOMNode* node);
	virtual void	Init();
	virtual INT		value_INT();
	virtual void	value_INT(INT val);
	virtual BOOL	onCommnd(UINT code, UINT id, HWND hWnd);
};
