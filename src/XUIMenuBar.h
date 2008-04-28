#pragma once
#include "xuielement.h"

class CXUIMenuBar :	public CXUIElement
{
	HMENU	m_hMenu;
public:
	CXUIMenuBar(CXUIElement* parent, CXUIEngine* engine);
	~CXUIMenuBar(void);
	IMPLEMENT_INTERFACE(L"menubar")

	void Init();
};
