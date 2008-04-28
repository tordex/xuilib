#pragma once
#include "xuielement.h"

class CXUIRadioGroup : public CXUIElement
{
public:
	CXUIRadioGroup(CXUIElement* parent, CXUIEngine* engine);
	~CXUIRadioGroup(void);
	IMPLEMENT_INTERFACE(L"radiogroup")

	void Init();

	virtual INT		value_INT();
	virtual void	value_INT(INT val);
	virtual BOOL	OnEvent(CXUIElement* el, LPCWSTR evID, WPARAM wParam, LPARAM lParam);
};
