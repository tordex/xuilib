#include "xuilib.h"

CXUIMenuSeparator::CXUIMenuSeparator(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
}

CXUIMenuSeparator::~CXUIMenuSeparator(void)
{
}

BOOL CXUIMenuSeparator::OnEvent( CXUIElement* el, LPCWSTR evID, WPARAM wParam, LPARAM lParam )
{
	if(!StrCmpI(evID, L"get-menu-item"))
	{
		LPMENUITEMINFO pmii = (LPMENUITEMINFO) lParam;
		pmii->fMask			= MIIM_TYPE;
		pmii->fType			= MFT_SEPARATOR;
		return TRUE;
	}
	return CXUIElement::OnEvent(el, evID, wParam, lParam);
}
