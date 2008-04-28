#include "xuilib.h"
#include "XUIOpenHelp.h"
#include "xmltools.h"

CXUIOpenHelp::CXUIOpenHelp(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_topic = NULL;
}

CXUIOpenHelp::~CXUIOpenHelp(void)
{
	if(m_topic)
	{
		delete m_topic;
	}
	m_topic = NULL;
}

BOOL CXUIOpenHelp::loadDATA( IXMLDOMNode* node )
{
	BOOL ret = CXUIElement::loadDATA(node);
	if(ret)
	{
		if(m_topic) delete m_topic;
		m_topic = NULL;
		m_topic = xmlGetAttributeSTR(node, L"topic");
	}
	return ret;
}

void CXUIOpenHelp::doDefaultAction( CXUIElement* el )
{
	if(m_topic)
	{
		HWND hWnd = NULL;
		CXUIElement* root = get_root();
		if(root)
		{
			hWnd = root->get_wnd();
		}
		m_engine->openHelpTopic(hWnd, m_topic, 0);
	}
}