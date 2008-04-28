#include "xuilib.h"
#include "XUIExt.h"
#include "xmltools.h"

CXUIExt::CXUIExt(CXUIElement* parent, CXUIEngine* engine, LPWSTR text, LPWSTR ext) : CXUIElement(parent, engine)
{
	m_text		= NULL;
	m_extension	= NULL;

	if(text)
	{
		m_text = new WCHAR[lstrlen(text) + 1];
		lstrcpy(m_text, text);
	}
	if(ext)
	{
		m_extension = new WCHAR[lstrlen(ext) + 1];
		lstrcpy(m_extension, ext);
	}
}

CXUIExt::~CXUIExt(void)
{
	if(m_text)		delete m_text;
	if(m_extension)	delete m_extension;
}

BOOL CXUIExt::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_text)		delete m_text;
	if(m_extension)	delete m_extension;
	
	m_text		= getLocalizedString(node, TEXT("text"));
	m_extension = xmlGetAttributeSTR(node, TEXT("extension"));
	
	return TRUE;
}