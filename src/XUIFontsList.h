#pragma once
#include "xuielement.h"

class CXUIFontsList : public CXUIElement
{
public:
	CXUIFontsList(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIFontsList(void);

	IMPLEMENT_INTERFACE(L"fontslist")

	virtual void Init();
};
