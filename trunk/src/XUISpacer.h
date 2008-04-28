#pragma once
#include "xuielement.h"

class CXUISpacer : public CXUIElement
{
public:
	CXUISpacer(CXUIElement* parent, CXUIEngine* engine);
	~CXUISpacer(void);
	IMPLEMENT_INTERFACE(L"spacer")

	void Init();
};
