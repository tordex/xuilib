#include "xuilib.h"

CXUISpacer::CXUISpacer(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
}

CXUISpacer::~CXUISpacer(void)
{
}

void CXUISpacer::Init()
{
	RECT rcDlg = {0, 0, 0, 0};
	if(m_width) rcDlg.right = m_width;
	if(m_height) rcDlg.bottom = m_height;
	MapDialogRect(m_parent->get_parentWnd(), &rcDlg);
	m_minWidth = rcDlg.right;
	m_minHeight = rcDlg.bottom;
}
