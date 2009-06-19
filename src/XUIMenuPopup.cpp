#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>

CXUIMenuPopup::CXUIMenuPopup(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_hMenu = NULL;
	m_label = NULL;
}

CXUIMenuPopup::~CXUIMenuPopup(void)
{
	if(m_hMenu) DestroyMenu(m_hMenu);
	if(m_label) delete m_label;
}

void CXUIMenuPopup::Init()
{
	m_hMenu = CreatePopupMenu();
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
}

BOOL CXUIMenuPopup::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_label) delete m_label;
	m_label = getLocalizedString(node, TEXT("label"));
	return TRUE;
}

void CXUIMenuPopup::doDefaultAction( CXUIElement* el )
{
	POINT pt;
	GetCursorPos(&pt);
	UINT ret = TrackPopupMenu(m_hMenu, TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, NULL, get_root()->get_wnd(), NULL);
	CXUIElement* mi = find(ret);
	if(mi)
	{
		mi->raiseEvent(XUI_EVENT_CLICKED, 0, NULL);
	}
}

BOOL CXUIMenuPopup::OnEvent( CXUIElement* el, LPCWSTR evID, WPARAM wParam, LPARAM lParam )
{
	if(!StrCmpI(evID, L"get-menu-item"))
	{
		LPMENUITEMINFO pmii = (LPMENUITEMINFO) lParam;
		pmii->fMask			= MIIM_ID | MIIM_TYPE | MIIM_SUBMENU;
		pmii->fType			= MFT_STRING;
		pmii->dwTypeData	= (LPWSTR) m_label;
		pmii->cch			= lstrlen(m_label) + 1;
		pmii->hSubMenu		= m_hMenu;
		pmii->wID			= get_dlgid();
		return TRUE;
	} else if(!StrCmpI(evID, L"enable-menu-item"))
	{
		EnableMenuItem(m_hMenu, (UINT) wParam, MF_BYCOMMAND | MF_ENABLED );
		return TRUE;
	} else if(!StrCmpI(evID, L"disable-menu-item"))
	{
		EnableMenuItem(m_hMenu, (UINT) wParam, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		return TRUE;
	} else if(!StrCmpI(evID, L"check-menu-item"))
	{
		CheckMenuItem(m_hMenu, (UINT) wParam, MF_BYCOMMAND | MF_CHECKED);
		return TRUE;
	} else if(!StrCmpI(evID, L"uncheck-menu-item"))
	{
		CheckMenuItem(m_hMenu, (UINT) wParam, MF_BYCOMMAND | MF_UNCHECKED);
		return TRUE;
	}
	return CXUIElement::OnEvent(el, evID, wParam, lParam);
}