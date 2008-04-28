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
		BOOL isValid = FALSE;

		CXUIMenuItem* mi = NULL;
		CXUIMenuSeparator* sep = NULL;
		CXUIMenuPopup* pop = NULL;
		if(m_childs[i]->QueryElement(L"menuitem", (LPVOID*) &mi))
		{
			isValid = TRUE;
			mii.fMask = MIIM_ID | MIIM_TYPE;
			mii.fType = MFT_STRING;
			mii.dwTypeData = (LPWSTR) mi->get_label();
			mii.cch = lstrlen(mi->get_label()) + 1;
			mii.wID = i + 1;
		} else if(m_childs[i]->QueryElement(L"menuseparator", (LPVOID*) &sep))
		{
			isValid = TRUE;
			mii.fMask = MIIM_TYPE;
			mii.fType = MFT_SEPARATOR;
		} else if(m_childs[i]->QueryElement(L"menupopup", (LPVOID*) &pop))
		{
			isValid = TRUE;
			mii.fMask = MIIM_ID | MIIM_TYPE | MIIM_SUBMENU;
			mii.fType = MFT_STRING;
			mii.dwTypeData = (LPWSTR) pop->get_label();
			mii.hSubMenu = pop->get_menu();
			mii.cch = lstrlen(pop->get_label()) + 1;
			mii.wID = i + 1;
		}
		if(isValid)
		{
			InsertMenuItem(m_hMenu, i, TRUE, &mii);
		}
	}
	SetMenu(get_parentWnd(), m_hMenu);
}

