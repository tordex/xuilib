#include "xuilib.h"

CXUIMenu::CXUIMenu(CXUIElement* parent) : CXUIElement(parent)
{
	m_hMenu = NULL;
}

CXUIMenu::~CXUIMenu(void)
{
	if(m_hMenu) DestroyMenu(m_hMenu);
}

void CXUIMenu::Init()
{
	m_hMenu = CreatePopupMenu();
	if(m_childCount)
	{
		CXUIMenuPopup* popup = NULL;
		if(m_childs[0]->QueryElement(L"menupopup", (LPVOID*) &popup))
		{
			popup->fillMenu(m_hMenu);
		}
	}
}

void CXUIMenu::recalcLayout(int x, int y, int maxWidth, int maxHeight, int orient)
{
	m_layWidth = 0;
	m_layHeight = 0;
}
