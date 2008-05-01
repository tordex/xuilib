#include "xuilib.h"
#include "XUITab.h"
#include "xmltools.h"

#define DEF_MARGIN		2

CXUITab::CXUITab(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_label			= NULL;
	m_caption		= NULL;
	m_image			= NULL;
	ZeroMemory(&m_rc, sizeof(RECT));
	m_selected		= FALSE;
	m_hover			= FALSE;
	m_bInitialized	= FALSE;
	m_bSetFocus		= FALSE;
	m_helpTopic		= NULL;
}

CXUITab::~CXUITab(void)
{
	if(m_label)			delete m_label;
	if(m_caption)		delete m_caption;
	if(m_image)			delete m_image;
	if(m_helpTopic)		delete m_helpTopic;
}

BOOL CXUITab::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_label)			delete m_label;
	if(m_caption)		delete m_caption;
	if(m_image)			delete m_image;
	if(m_helpTopic)		delete m_helpTopic;

	m_label			= getLocalizedString(node, TEXT("label"));
	m_caption		= getLocalizedString(node, TEXT("caption"));
	m_image			= xmlGetAttributeSTR(node, TEXT("icon"));
	m_helpTopic		= xmlGetAttributeSTR(node, TEXT("helpTopic"));
	m_selected		= xmlGetAttributeValueBOOL(node,	TEXT("selected"),		FALSE);
	m_bSetFocus		= xmlGetAttributeValueBOOL(node,	TEXT("setFocus"),		FALSE);
	m_value			= xmlGetAttributeValueNonSTR<INT>(node, L"value", 0);
	if(m_selected)
	{
		m_hidden = FALSE;
	} else
	{
		m_hidden = TRUE;
	}
	return TRUE;
}

void CXUITab::selected( int bSelected )
{
	m_selected = bSelected;
	if(m_selected)
	{
		m_hidden = FALSE;
		if(!m_bInitialized)
		{
			m_bInitialized = TRUE;
			raiseEvent(XUI_EVENT_TAB_INIT, 0, NULL);
		}
		raiseEvent(XUI_EVENT_TAB_ACTIVATE, 0, NULL);
	} else
	{
		m_hidden = TRUE;
		raiseEvent(XUI_EVENT_TAB_DEACTIVATE, 0, NULL);
	}
	updateHiddenState();
	if(bSelected && m_bSetFocus)
	{
		if(m_childCount)
		{
			for(int i=0; i < m_childCount; i++)
			{
				if(m_childs[i]->set_TabStopFocus())
				{
					break;
				}
			}
		}
	}
}

void CXUITab::Draw( HDC hdc, int imageAlign, int imageSize, LPRECT rcItem )
{
	FillRect(hdc, rcItem, (HBRUSH) (COLOR_WINDOW + 1));
	COLORREF textColor = GetSysColor(COLOR_WINDOWTEXT);;
	if(m_selected)
	{
		textColor = GetSysColor(COLOR_HIGHLIGHTTEXT);

		COLORREF bgColor1 = tswLightColor(GetSysColor(COLOR_HIGHLIGHT), 20);
		COLORREF bgColor2 = tswLightColor(GetSysColor(COLOR_HIGHLIGHT), -20);

		RECT rcDraw = *rcItem;
		rcDraw.left		+= DEF_MARGIN;
		rcDraw.right	-= DEF_MARGIN;

		CXUITabSwitcher::DrawGradient(hdc, rcDraw, bgColor1, bgColor2);

	} else if(m_hover)
	{
		COLORREF bgColor = tswLightColor(GetSysColor(COLOR_HIGHLIGHT), 50);

		COLORREF bgColor1 = tswLightColor(bgColor, 10);
		COLORREF bgColor2 = tswLightColor(bgColor, -10);

		RECT rcDraw = *rcItem;
		rcDraw.left		+= DEF_MARGIN;
		rcDraw.right	-= DEF_MARGIN;

		CXUITabSwitcher::DrawGradient(hdc, rcDraw, bgColor1, bgColor2);
	}

	RECT	rcText	= *rcItem;
	int		iconX	= 0;
	int		iconY	= 0;
	UINT	dtAdd	= 0;
	switch(imageAlign)
	{
	case XUI_TAB_IMAGE_ALIGN_LEFT:
		rcText.left += DEF_MARGIN * 2 + imageSize + DEF_MARGIN * 2;
		rcText.right -= DEF_MARGIN * 2;
		iconX = rcItem->left + DEF_MARGIN * 2;
		iconY = rcItem->top + (rcItem->bottom - rcItem->top) / 2 - imageSize / 2;
		break;
	case XUI_TAB_IMAGE_ALIGN_TOP:
		rcText.top		+= DEF_MARGIN + imageSize + DEF_MARGIN;
		rcText.bottom	-= DEF_MARGIN;
		iconY = rcItem->top + DEF_MARGIN;
		iconX = rcItem->left + (rcItem->right - rcItem->left) / 2 - imageSize / 2;
		dtAdd = DT_CENTER;
		break;
	}

	SetTextColor(hdc, textColor);
	SetBkMode(hdc, TRANSPARENT);
	DrawText(hdc, m_label, -1, &rcText, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | dtAdd);

	m_engine->DrawImage(hdc, iconX, iconY, imageSize, imageSize, m_image);
}

void CXUITab::GetTabSize( HDC hdc, int imageAlign, int imageSize, SIZE& sz )
{
	RECT rcDraw = {0, 0, 3, 3};
	DrawText(hdc, m_label, -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
	int w = 0;
	int h = 0;
	switch(imageAlign)
	{
	case XUI_TAB_IMAGE_ALIGN_LEFT:
		w = DEF_MARGIN * 2 + imageSize + DEF_MARGIN * 2 + (rcDraw.right - rcDraw.left) + DEF_MARGIN * 2;
		h = DEF_MARGIN + max(imageSize, rcDraw.bottom - rcDraw.top) + DEF_MARGIN;
		break;
	case XUI_TAB_IMAGE_ALIGN_TOP:
		w = DEF_MARGIN * 2 + max(imageSize, (rcDraw.right - rcDraw.left)) + DEF_MARGIN * 2;
		h = DEF_MARGIN + imageSize + DEF_MARGIN + (rcDraw.bottom - rcDraw.top) + DEF_MARGIN;
		break;
	}
	sz.cx = w + 10;
	if(imageAlign == XUI_TAB_IMAGE_ALIGN_TOP)
	{
		if(sz.cx < imageSize + imageSize * 2 / 3) sz.cx = imageSize + imageSize * 2 / 3;
	}
	sz.cy = h;
}

void CXUITab::Init()
{
	CXUIElement::Init();

	if(m_selected)
	{
		if(!m_bInitialized)
		{
			m_bInitialized = TRUE;
			raiseEvent(XUI_EVENT_TAB_INIT, 0, NULL);
		}
		raiseEvent(XUI_EVENT_TAB_ACTIVATE, 0, NULL);
	}
}

BOOL CXUITab::isInitialized()
{
	return m_bInitialized;
}

INT CXUITab::value_INT()
{
	return m_value;
}

void CXUITab::reinit()
{
	if(m_bInitialized)
	{
		m_bInitialized = TRUE;
		raiseEvent(XUI_EVENT_TAB_INIT, 0, NULL);
	}
}

void CXUITab::init()
{
	m_bInitialized = TRUE;
	raiseEvent(XUI_EVENT_TAB_INIT, 0, NULL);
}

void CXUITab::onCmd( LPWSTR cmd )
{
	if(!lstrcmpi(cmd, L"help"))
	{
		if(m_helpTopic)
		{
			HWND hWnd = NULL;
			CXUIElement* root = get_root();
			if(root)
			{
				hWnd = root->get_wnd();
			}
			m_engine->openHelpTopic(hWnd, m_helpTopic, 0);
		}
	}
}