#include "xuilib.h"
#include "xmltools.h"
#include <windowsx.h>

CXUIMenuItem::CXUIMenuItem(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_label = NULL;
}

CXUIMenuItem::~CXUIMenuItem(void)
{
	if(m_label) delete m_label;
}

BOOL CXUIMenuItem::loadDATA(IXMLDOMNode* node)
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_label) delete m_label;

	m_label		= getLocalizedString(		node,	L"label");
	m_isRadio	= xmlGetAttributeValueBOOL(	node,	L"radio",	FALSE);
	m_isDefault	= xmlGetAttributeValueBOOL(	node,	L"default", FALSE);

	return TRUE;
}

void CXUIMenuItem::value_INT( INT val )
{
	m_isChecked = val;
	if(val)
	{
		m_parent->raiseEvent(L"check-menu-item", get_dlgid(), NULL);
	} else
	{
		m_parent->raiseEvent(L"uncheck-menu-item", get_dlgid(), NULL);
	}
}

BOOL CXUIMenuItem::OnEvent( CXUIElement* el, LPCWSTR evID, WPARAM wParam, LPARAM lParam )
{
	if(!StrCmpI(evID, L"get-menu-item"))
	{
		LPMENUITEMINFO pmii = (LPMENUITEMINFO) lParam;
		pmii->fMask			= MIIM_ID | MIIM_TYPE;
		pmii->fType			= MFT_STRING;
		if(m_isRadio)
		{
			pmii->fType |= MFT_RADIOCHECK;
		}
		if(m_isDefault)
		{
			pmii->fMask		|= MIIM_STATE;
			pmii->fState	|= MFS_DEFAULT;
		}
		pmii->dwTypeData	= (LPWSTR) m_label;
		pmii->cch			= lstrlen(m_label) + 1;
		pmii->wID			= get_dlgid();
		return TRUE;
	}
	return CXUIElement::OnEvent(el, evID, wParam, lParam);
}

void CXUIMenuItem::updateDisabledState()
{
	if(m_disabled)
	{
		m_parent->raiseEvent(L"disable-menu-item", get_dlgid(), NULL);
	} else
	{
		m_parent->raiseEvent(L"enable-menu-item", get_dlgid(), NULL);
	}
}
