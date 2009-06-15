#include "xuilib.h"
#include "XUISelectFolder.h"
#include "xmltools.h"
#include <shlobj.h>

CXUISelectFolder::CXUISelectFolder(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_title		= NULL;
	m_saveto	= NULL;
}

CXUISelectFolder::~CXUISelectFolder(void)
{
	if(m_title)		delete m_title;
	if(m_saveto)	delete m_saveto;
}

BOOL CXUISelectFolder::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;

	if(m_title) delete m_title;
	m_title = getLocalizedString(node, TEXT("title"));
	if(m_saveto) delete m_saveto;
	m_saveto = xmlGetAttributeSTR(node, TEXT("saveto"));

	return TRUE;
}

void CXUISelectFolder::Init()
{
	m_minWidth	= 0;
	m_minHeight = 0;
}

void CXUISelectFolder::doDefaultAction( CXUIElement* el )
{
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.hwndOwner = m_parent->get_parentWnd();
	TCHAR displayName[MAX_PATH];
	displayName[0] = 0;
	bi.pszDisplayName = displayName;
	bi.lpszTitle = m_title;
	bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if(pidl)
	{
		SHGetPathFromIDList(pidl, displayName);
		CoTaskMemFree(pidl);

		if(displayName[0] && m_saveto)
		{
			CXUIElement* root = get_root();
			CXUIElement* txt = root->find(m_saveto);
			if(txt)
			{
				txt->value_STR(displayName);
				txt->raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
			} else
			{
				raiseEvent(XUI_EVENT_FSSELECTED, 0, (LPARAM) displayName);
			}
		} else
		{
			raiseEvent(XUI_EVENT_FSSELECTED, 0, (LPARAM) displayName);
		}
	}
}
