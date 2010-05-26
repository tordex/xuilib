#include "xuilib.h"
#include "XUISelectFolder.h"
#include "xmltools.h"
#include <shlobj.h>

CXUISelectFolder::CXUISelectFolder(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_title		= NULL;
	m_saveto	= NULL;
	m_selFolder	= NULL;
}

CXUISelectFolder::~CXUISelectFolder(void)
{
	if(m_title)		delete m_title;
	if(m_saveto)	delete m_saveto;
	if(m_selFolder)	delete m_selFolder;
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
	if(m_selFolder)
	{
		bi.lpfn		= BrowseIconFldProc;
		bi.lParam	= (LPARAM) this;
	}
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

void CXUISelectFolder::value_STR( LPCWSTR val )
{
	if(m_selFolder)	delete m_selFolder;
	m_selFolder = NULL;
	if(val)
	{
		m_selFolder = new WCHAR[lstrlen(val) + 1];
		lstrcpy(m_selFolder, val);
	}
}

LPCWSTR CXUISelectFolder::value_STR()
{
	return m_selFolder;
}

int CALLBACK CXUISelectFolder::BrowseIconFldProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		{
			CXUISelectFolder* pThis = (CXUISelectFolder*) lpData;
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM) pThis->m_selFolder);
		}
		break;
	}
	return 0;
}
