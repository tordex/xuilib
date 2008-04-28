#pragma once
#include "xuielement.h"

class CXUIMenuSeparator : public CXUIElement
{
public:
	CXUIMenuSeparator(CXUIElement* parent, CXUIEngine* engine);
	~CXUIMenuSeparator(void);

	IMPLEMENT_INTERFACE(L"menuseparator")
};
