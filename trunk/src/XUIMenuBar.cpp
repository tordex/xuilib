#include "xuilib.h"

CXUIMenuBar::CXUIMenuBar(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_hMenu = 0;
}

CXUIMenuBar::~CXUIMenuBar(void)
{
	DestroyMenu(m_hMenu);
}

void CXUIMenuBar::Init()
{
	m_hMenu = CreateMenu();
	CXUIElement::Init();
	for(int i=0; i < m_childCount; i++)
	{
		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(MENUITEMINFO);

		if(m_childs[i]->raiseEvent(L"get-menu-item", 0, (LPARAM) &mii))
		{
			InsertMenuItem(m_hMenu, i, TRUE, &mii);
		}
	}
	SetMenu(get_parentWnd(), m_hMenu);
}

