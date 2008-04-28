#include "xuilib.h"
#include "XUIProgress.h"
#include "xmltools.h"
#include <CommCtrl.h>

CXUIProgress::CXUIProgress(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
}

CXUIProgress::~CXUIProgress(void)
{
}

BOOL CXUIProgress::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	m_bMarquee			=	xmlGetAttributeValueBOOL(node, L"marquee",			FALSE);
	m_bSmooth			=	xmlGetAttributeValueBOOL(node, L"smooth",			FALSE);
	m_bSmoothreverse	=	xmlGetAttributeValueBOOL(node, L"smoothreverse",	FALSE);
	m_bVertical			=	xmlGetAttributeValueBOOL(node, L"vertical",			FALSE);

	m_lowLimit			=	xmlGetAttributeValueNonSTR<INT>(node, L"lowLimit",	0);
	m_highLimit			=	xmlGetAttributeValueNonSTR<INT>(node, L"highLimit",	100);
	m_pos				=	xmlGetAttributeValueNonSTR<INT>(node, L"pos",		0);
	return TRUE;
}

void CXUIProgress::Init()
{
	DWORD wStyle = WS_CHILD;

	if(m_bMarquee)			wStyle |= PBS_MARQUEE;
	if(m_bSmooth)			wStyle |= PBS_SMOOTH;
	if(m_bSmoothreverse)	wStyle |= PBS_SMOOTHREVERSE;
	if(m_bVertical)			wStyle |= PBS_VERTICAL;

	if(get_disabled())		wStyle |= WS_DISABLED;
	if(!get_hidden())		wStyle |= WS_VISIBLE;

	m_hWnd = CreateWindowEx(0, PROGRESS_CLASS, L"", wStyle, m_left, m_top, m_width, m_height, m_parent->get_parentWnd(), (HMENU) m_id, m_engine->get_hInstance(), NULL);
	set_limits(m_lowLimit, m_highLimit);
	set_pos(m_pos);

	RECT rcDlg = {0, 0, 80, 14};
	if(m_width) rcDlg.right = m_width;
	if(m_height) rcDlg.bottom = m_height;
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minWidth	= rcDlg.right;
	m_minHeight = rcDlg.bottom;
}

void CXUIProgress::set_limits( int low, int high )
{
	m_lowLimit	= low;
	m_highLimit	= high;
	if(m_hWnd)
	{
		SendMessage(m_hWnd, PBM_SETRANGE32, (WPARAM) low, (LPARAM) high);
	}
}

void CXUIProgress::set_pos( int pos )
{
	m_pos = pos;
	if(m_hWnd)
	{
		SendMessage(m_hWnd, PBM_SETPOS, (WPARAM) pos, NULL);
	}
}