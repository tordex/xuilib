#include "xuilib.h"
#include "XUISelectFile.h"
#include "xmltools.h"

CXUISelectFile::CXUISelectFile(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_allowMultiSelect	= FALSE;
	m_dontAddRecent		= FALSE;
	m_enableSizing		= FALSE;
	m_explorer			= FALSE;
	m_fileMustExist		= FALSE;
	m_forceHidden		= FALSE;
	m_hideReadOnly		= FALSE;
	m_longNames			= FALSE;
	m_noNetworkButton	= FALSE;
	m_noReadOnlyReturn	= FALSE;
	m_noTestFileCreate	= FALSE;
	m_noValidate		= FALSE;
	m_overwriteprompt	= FALSE;
	m_pathmustexist		= FALSE;
	m_readonly			= FALSE;
	m_shareaware		= FALSE;
	m_showhelp			= FALSE;
	m_createprompt		= FALSE;

	m_saveto			= NULL;
	m_title				= NULL;
	m_type				= XUI_SELECTFILE_TYPE_OPEN;
	m_fileName			= NULL;
	m_defExt			= NULL;
}

CXUISelectFile::~CXUISelectFile(void)
{
	if(m_title)		delete m_title;
	if(m_saveto)	delete m_saveto;
	if(m_fileName)	delete m_fileName;
}

BOOL CXUISelectFile::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;

	m_type				= xmlGetAttributeValueSTRArray(node, TEXT("type"), XUI_SELECTFILE_TYPE_OPEN, L"open\0save\0");
	if(m_title) delete m_title;
	m_title = getLocalizedString(node, TEXT("title"));
	if(m_saveto) delete m_saveto;
	m_saveto = xmlGetAttributeSTR(node, TEXT("saveto"));
	if(m_defExt) delete m_saveto;
	m_defExt = xmlGetAttributeSTR(node, TEXT("defExt"));
	
	m_allowMultiSelect	= xmlGetAttributeValueBOOL(node,	TEXT("allowMultiSelect"),	FALSE);
	m_dontAddRecent		= xmlGetAttributeValueBOOL(node,	TEXT("dontAddRecent"),		TRUE);
	m_enableSizing		= xmlGetAttributeValueBOOL(node,	TEXT("enableSizing"),		TRUE);
	m_explorer			= xmlGetAttributeValueBOOL(node,	TEXT("explorer"),			FALSE);
	m_fileMustExist		= xmlGetAttributeValueBOOL(node,	TEXT("fileMustExist"),		FALSE);
	m_forceHidden		= xmlGetAttributeValueBOOL(node,	TEXT("forceHidden"),		FALSE);
	m_hideReadOnly		= xmlGetAttributeValueBOOL(node,	TEXT("hideReadOnly"),		TRUE);
	m_longNames			= xmlGetAttributeValueBOOL(node,	TEXT("longNames"),			TRUE);
	m_noNetworkButton	= xmlGetAttributeValueBOOL(node,	TEXT("noNetworkButton"),	FALSE);
	m_noReadOnlyReturn	= xmlGetAttributeValueBOOL(node,	TEXT("noReadOnlyReturn"),	FALSE);
	m_noTestFileCreate	= xmlGetAttributeValueBOOL(node,	TEXT("noTestFileCreate"),	FALSE);
	m_noValidate		= xmlGetAttributeValueBOOL(node,	TEXT("noValidate"),			FALSE);
	m_overwriteprompt	= xmlGetAttributeValueBOOL(node,	TEXT("overwriteprompt"),	TRUE);
	m_pathmustexist		= xmlGetAttributeValueBOOL(node,	TEXT("pathmustexist"),		TRUE);
	m_readonly			= xmlGetAttributeValueBOOL(node,	TEXT("readonly"),			FALSE);
	m_shareaware		= xmlGetAttributeValueBOOL(node,	TEXT("shareaware"),			FALSE);
	m_showhelp			= xmlGetAttributeValueBOOL(node,	TEXT("showhelp"),			FALSE);
	m_createprompt		= xmlGetAttributeValueBOOL(node,	TEXT("createprompt"),		TRUE);
	m_bQuoteSpaces		= xmlGetAttributeValueBOOL(node,	TEXT("quoteSpaces"),		FALSE);

	return TRUE;
}

void CXUISelectFile::Init()
{
	m_minWidth	= 0;
	m_minHeight = 0;
}

void CXUISelectFile::doDefaultAction( CXUIElement* el )
{
	LPTSTR filter	= NULL;
	LPTSTR tmp		= NULL;
	int	   len		= 0;
	for(int i=0; i < m_childCount; i++)
	{
		CXUIExt* ext = NULL;
		if(m_childs[i]->QueryElement(TEXT("ext"), (LPVOID*) &ext))
		{
			if(ext->get_text() && ext->get_extension())
			{
				int lenText = lstrlen(ext->get_text()) + 1;
				int lenExt  = lstrlen(ext->get_extension()) + 1;

				if(filter)
				{
					filter = (LPTSTR) realloc(filter, (len + lenText + lenExt) * sizeof(TCHAR));
				} else
				{
					filter = (LPTSTR) malloc((len + lenText + lenExt) * sizeof(TCHAR));
				}

				tmp = filter + len;
				lstrcpy(tmp, ext->get_text());
				tmp += lenText;
				lstrcpy(tmp, ext->get_extension());
				tmp += lenExt;
				len += lenText + lenExt;
			}
		}
	}
	if(filter)
	{
		filter = (LPTSTR) realloc(filter, (len + 1) * sizeof(TCHAR));
		filter[len] = 0;
	}

	TCHAR fileName[500];
	fileName[0] = 0;
	if(m_fileName)
	{
		lstrcpy(fileName, m_fileName);
	}
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize			= sizeof(ofn);
	ofn.hwndOwner			= m_parent->get_parentWnd();
	ofn.hInstance			= m_engine->get_hInstance();
	ofn.lpstrFilter			= filter;
	ofn.lpstrCustomFilter	= NULL;
	ofn.lpstrFile			= fileName;
	ofn.nMaxFile			= 500;
	ofn.lpstrTitle			= m_title;
	ofn.Flags				= 0;
	ofn.lpstrDefExt			= m_defExt;

	if(m_allowMultiSelect)		ofn.Flags |= OFN_ALLOWMULTISELECT;
	if(m_dontAddRecent)			ofn.Flags |= OFN_DONTADDTORECENT;
	if(m_enableSizing)			ofn.Flags |= OFN_ENABLESIZING;
	if(m_explorer)				ofn.Flags |= OFN_EXPLORER;
	if(m_fileMustExist)			ofn.Flags |= OFN_FILEMUSTEXIST;
	if(m_forceHidden)			ofn.Flags |= OFN_FORCESHOWHIDDEN;
	if(m_hideReadOnly)			ofn.Flags |= OFN_HIDEREADONLY;
	if(m_longNames)				ofn.Flags |= OFN_LONGNAMES;
	if(m_noNetworkButton)		ofn.Flags |= OFN_NONETWORKBUTTON;
	if(m_noReadOnlyReturn)		ofn.Flags |= OFN_NOREADONLYRETURN;
	if(m_noTestFileCreate)		ofn.Flags |= OFN_NOTESTFILECREATE;
	if(m_noValidate)			ofn.Flags |= OFN_NOVALIDATE;
	if(m_overwriteprompt)		ofn.Flags |= OFN_OVERWRITEPROMPT;
	if(m_pathmustexist)			ofn.Flags |= OFN_PATHMUSTEXIST;
	if(m_readonly)				ofn.Flags |= OFN_READONLY;
	if(m_shareaware)			ofn.Flags |= OFN_SHAREAWARE;
	if(m_showhelp)				ofn.Flags |= OFN_SHOWHELP;
	if(m_createprompt)			ofn.Flags |= OFN_CREATEPROMPT;

	BOOL ret = FALSE;
	if(m_type == XUI_SELECTFILE_TYPE_OPEN)
	{
		ret = GetOpenFileName(&ofn);
	} else
	{
		ret = GetSaveFileName(&ofn);
	}

	if(fileName[0] && m_saveto && ret)
	{
		if(m_bQuoteSpaces)
		{
			PathQuoteSpaces(fileName);
		}
		CXUIElement* root = get_root();
		CXUIElement* txt = root->find(m_saveto);
		if(txt)
		{
			txt->value_STR(fileName);
			txt->raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
		} else
		{
			raiseEvent(XUI_EVENT_FSSELECTED, 0, (LPARAM) fileName);
		}
	} else if(ret)
	{
		raiseEvent(XUI_EVENT_FSSELECTED, 0, (LPARAM) fileName);
	}

	if(filter)
	{
		free(filter);
	}
}

void CXUISelectFile::addExt( LPWSTR text, LPWSTR ext )
{
	CXUIExt* el = new CXUIExt(this, m_engine, text, ext);
	addChild(el);
}

void CXUISelectFile::clearExt()
{
	clearChilds();
}

void CXUISelectFile::fileName( LPWSTR flName )
{
	if(m_fileName)	delete m_fileName;
	m_fileName = NULL;
	if(flName)
	{
		m_fileName = new WCHAR[lstrlen(flName) + 1];
		lstrcpy(m_fileName, flName);
	}
}
