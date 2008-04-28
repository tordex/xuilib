#pragma once
#include "xuielement.h"

class CXUIMenu : public CXUIElement
{
	HMENU	m_hMenu;
public:
	CXUIMenu(CXUIElement* parent);
	~CXUIMenu(void);
	IMPLEMENT_INTERFACE(L"menu")

	void Init();
	void recalcLayout(int x, int y, int maxWidth, int maxHeight, int orient);

	HMENU get_hMenu() { return m_hMenu; }
};
