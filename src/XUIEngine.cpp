#include "xuilib.h"
#include "XUIEngine.h"
#include "xmltools.h"

CXUIEngine::CXUIEngine(HINSTANCE hInst)
{
	m_hInst			= hInst;
	m_strings		= NULL;
	m_stringsCount	= 0;
}

CXUIEngine::~CXUIEngine(void)
{
	clearStrings();
}

CXUIElement* CXUIEngine::createXUIElement( LPCWSTR name, CXUIElement* parent )
{
	CXUIElement* ret = NULL;
	if(!StrCmpI(name, TEXT("window")))
	{
		ret = new CXUIWindow(parent, this);
	} else if(!StrCmpI(name, TEXT("dialog")))
	{
		ret = new CXUIDialog(NULL, this, parent);
	} else if(!StrCmpI(name, TEXT("textbox")))
	{
		ret = new CXUITextBox(parent, this);
	} else if(!StrCmpI(name, TEXT("box")))
	{
		ret = new CXUIBox(parent, this, -1);
	} else if(!StrCmpI(name, TEXT("button")))
	{
		ret = new CXUIButton(parent, this);
	} else if(!StrCmpI(name, TEXT("groupbox")))
	{
		ret = new CXUIGroupbox(parent, this);
	} else if(!StrCmpI(name, TEXT("label")))
	{
		ret = new CXUILabel(parent, this);
	} else if(!StrCmpI(name, TEXT("checkbox")))
	{
		ret = new CXUICheckbox(parent, this);
	} else if(!StrCmpI(name, TEXT("radio")))
	{
		ret = new CXUIRadio(parent, this);
	} else if(!StrCmpI(name, TEXT("combobox")))
	{
		ret = new CXUIComboBox(parent, this);
	} else if(!StrCmpI(name, TEXT("menupopup")))
	{
		ret = new CXUIMenuPopup(parent, this);
	} else if(!StrCmpI(name, TEXT("menuitem")))
	{
		ret = new CXUIMenuItem(parent, this);
	} else if(!StrCmpI(name, TEXT("radiogroup")))
	{
		ret = new CXUIRadioGroup(parent, this);
	} else if(!StrCmpI(name, TEXT("spin")))
	{
		ret = new CXUISpin(parent, this);
	} else if(!StrCmpI(name, TEXT("tabbox")))
	{
		ret = new CXUITabBox(parent, this);
	} else if(!StrCmpI(name, TEXT("tabpanel")))
	{
		ret = new CXUITabPanel(parent, this);
	} else if(!StrCmpI(name, TEXT("vbox")))
	{
		ret = new CXUIBox(parent, this, XUI_ORIENT_VERTICAL);
	} else if(!StrCmpI(name, TEXT("hbox")))
	{
		ret = new CXUIBox(parent, this, XUI_ORIENT_HORIZONTAL);
	} else if(!StrCmpI(name, TEXT("spacer")))
	{
		ret = new CXUISpacer(parent, this);
	} else if(!StrCmpI(name, TEXT("menubar")))
	{
		ret = new CXUIMenuBar(parent, this);
	} else if(!StrCmpI(name, TEXT("listitem")))
	{
		ret = new CXUIListItem(parent, this);
	} else if(!StrCmpI(name, TEXT("menuseparator")))
	{
		ret = new CXUIMenuSeparator(parent, this);
	} else if(!StrCmpI(name, TEXT("tabswitcher")))
	{
		ret = new CXUITabSwitcher(parent, this);
	} else if(!StrCmpI(name, TEXT("tab")))
	{
		ret = new CXUITab(parent, this);
	} else if(!StrCmpI(name, TEXT("color")))
	{
		ret = new CXUIColor(parent, this);
	} else if(!StrCmpI(name, TEXT("slider")))
	{
		ret = new CXUISlider(parent, this);
	} else if(!StrCmpI(name, TEXT("hotkey")))
	{
		ret = new CXUIHotkey(parent, this);
	} else if(!StrCmpI(name, TEXT("picture")))
	{
		ret = new CXUIPicture(parent, this);
	} else if(!StrCmpI(name, TEXT("listbox")))
	{
		ret = new CXUIListBox(parent, this);
	} else if(!StrCmpI(name, TEXT("line")))
	{
		ret = new CXUILine(parent, this);
	} else if(!StrCmpI(name, TEXT("url")))
	{
		ret = new CXUIUrl(parent, this);
	} else if(!StrCmpI(name, TEXT("list")))
	{
		ret = new CXUIList(parent, this);
	} else if(!StrCmpI(name, TEXT("selectfile")))
	{
		ret = new CXUISelectFile(parent, this);
	} else if(!StrCmpI(name, TEXT("ext")))
	{
		ret = new CXUIExt(parent, this);
	} else if(!StrCmpI(name, TEXT("listcol")))
	{
		ret = new CXUIListCol(parent, this);
	} else if(!StrCmpI(name, TEXT("checkslist")))
	{
		ret = new CXUIChecksList(parent, this);
	} else if(!StrCmpI(name, TEXT("checkitem")))
	{
		ret = new CXUICheckItem(parent, this);
	} else if(!StrCmpI(name, TEXT("freedraw")))
	{
		ret = new CXUIFreeDraw(parent, this);
	} else if(!StrCmpI(name, TEXT("disabler")))
	{
		ret = new CXUIDisabler(parent, this);
	} else if(!StrCmpI(name, TEXT("sendcmd")))
	{
		ret = new CXUISendCMD(parent, this);
	} else if(!StrCmpI(name, TEXT("progress")))
	{
		ret = new CXUIProgress(parent, this);
	} else if(!StrCmpI(name, TEXT("openhelp")))
	{
		ret = new CXUIOpenHelp(parent, this);
	} else if(!StrCmpI(name, TEXT("datepicker")))
	{
		ret = new CXUIDatePicker(parent, this);
	} else if(!StrCmpI(name, TEXT("selectfolder")))
	{
		ret = new CXUISelectFolder(parent, this);
	} else if(!StrCmpI(name, TEXT("toolbar")))
	{
		ret = new CXUIToolbar(parent, this);
	} else if(!StrCmpI(name, TEXT("toolbarbutton")))
	{
		ret = new CXUIToolbarButton(parent, this);
	} else if(!StrCmpI(name, TEXT("rtf")))
	{
		ret = new CXUIRTF(parent, this);
	} else if(!StrCmpI(name, TEXT("statusbar")))
	{
		ret = new CXUIStatusBar(parent, this);
	} else if(!StrCmpI(name, TEXT("sbpart")))
	{
		ret = new CXUISBPart(parent, this);
	} else if(!StrCmpI(name, TEXT("vsplitter")))
	{
		ret = new CXUIVSplitter(parent, this);
	} else if(!StrCmpI(name, TEXT("hsplitter")))
	{
		ret = new CXUIHSplitter(parent, this);
	}	return ret;
}

void CXUIEngine::DrawImage( HDC hdc, int x, int y, int width, int height, LPWSTR imgUrl )
{

}

void CXUIEngine::clearStrings()
{
	for(int i=0; i < m_stringsCount; i++)
	{
		if(m_strings[i].id)		delete m_strings[i].id;
		if(m_strings[i].attr)	delete m_strings[i].attr;
		if(m_strings[i].loc)	delete m_strings[i].loc;
	}
	if(m_strings)	free(m_strings);
	m_strings		= NULL;
	m_stringsCount	= 0;
}

void CXUIEngine::loadStrings( LPWSTR stringsFile )
{
	BOOL ret = FALSE;
	CComPtr<IXMLDOMDocument> doc;
	doc.CoCreateInstance(CLSID_DOMFreeThreadedDocument, NULL, CLSCTX_INPROC_SERVER);
	doc->put_async(VARIANT_FALSE);
	doc->put_validateOnParse(VARIANT_FALSE);
	doc->put_resolveExternals(VARIANT_FALSE);
	VARIANT_BOOL bSuccess = VARIANT_FALSE;
	if(doc->load(CComVariant(stringsFile), &bSuccess) == S_OK && bSuccess == VARIANT_TRUE)
	{
		IXMLDOMNode* node = xmlOpenNode(doc, L"strings");
		IXMLDOMNode* nodeString = xmlOpenNode(node, L"string");
		while(nodeString)
		{
			if(!m_stringsCount)
			{
				m_strings = (LOC_STR*) malloc(sizeof(LOC_STR));
			} else
			{
				m_strings = (LOC_STR*) realloc(m_strings, sizeof(LOC_STR) * (m_stringsCount + 1));
			}
			m_strings[m_stringsCount].id	= xmlGetAttributeSTR(nodeString, TEXT("id"));
			m_strings[m_stringsCount].attr	= xmlGetAttributeSTR(nodeString, TEXT("attr"));
			m_strings[m_stringsCount].loc	= xmlGetAttributeSTR(nodeString, TEXT("loc"));
			m_stringsCount++;

			IXMLDOMNode* nextNode = NULL;
			nodeString->get_nextSibling(&nextNode);
			nodeString->Release();
			nodeString = nextNode;
		}
		node->Release();
	}
}

LPWSTR CXUIEngine::getString( LPCWSTR id, LPCWSTR attr /*= NULL*/ )
{
	for(int i=0; i < m_stringsCount; i++)
	{
		if(!lstrcmpi(id, m_strings[i].id))
		{
			if(m_strings[i].attr && attr)
			{
				if(!lstrcmpi(attr, m_strings[i].attr))
				{
					return m_strings[i].loc;
				}
			} else if(!m_strings[i].attr && !attr)
			{
				return m_strings[i].loc;
			}
		}
	}
	return NULL;
}

LPWSTR CXUIEngine::getStringDef( LPCWSTR id, LPCWSTR attr, LPWSTR defString )
{
	LPWSTR ret = getString(id, attr);
	if(!ret)
	{
		ret = defString;
	}
	return ret;
}

void CXUIEngine::openHelpTopic( HWND hWnd, LPCWSTR topic, UINT flags )
{

}

BOOL CXUIEngine::isViewer()
{
	return FALSE;
}

void CXUIEngine::DrawFrame( HDC hdc, int x, int y, int width, int height, LPWSTR imgUrl, int frames, int frame, int framesOrient )
{

}

void CXUIEngine::DrawDisabledImage( HDC hdc, int x, int y, int width, int height, LPWSTR imgUrl )
{
	DrawImage(hdc, x, y, width, height, imgUrl);
}
