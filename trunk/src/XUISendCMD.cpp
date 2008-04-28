#include "xuilib.h"
#include "XUISendCMD.h"
#include "xmltools.h"

CXUISendCMD::CXUISendCMD(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_sendto	= NULL;
	m_cmd		= NULL;
}

CXUISendCMD::~CXUISendCMD(void)
{
	if(m_sendto)	delete m_sendto;
	if(m_cmd)		delete m_cmd;
}

BOOL CXUISendCMD::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_sendto)	delete m_sendto;
	if(m_cmd)		delete m_cmd;
	m_sendto	= xmlGetAttributeSTR(node,					TEXT("sendto"));
	m_cmd		= xmlGetAttributeSTR(node,					TEXT("cmd"));
	return TRUE;
}

void CXUISendCMD::doDefaultAction( CXUIElement* el )
{
	if(m_sendto && m_cmd)
	{
		CXUIElement* dstEl = get_root()->find(m_sendto);
		if(dstEl)
		{
			dstEl->onCmd(m_cmd);
		}
	}
}