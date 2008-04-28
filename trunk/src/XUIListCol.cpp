#include "xuilib.h"
#include "XUIListCol.h"
#include "xmltools.h"

CXUIListCol::CXUIListCol(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
	m_caption = NULL;
}

CXUIListCol::~CXUIListCol(void)
{
	if(m_caption)	delete m_caption;
}

BOOL CXUIListCol::loadDATA( IXMLDOMNode* node )
{
	if(!CXUIElement::loadDATA(node)) return FALSE;
	if(m_caption)	delete m_caption;

	m_caption	= getLocalizedString(node, TEXT("caption"));
	m_size		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("size"),	0);
	m_flex		= xmlGetAttributeValueNonSTR<int>(node,	TEXT("flex"),	0);

	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT) SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
	LPWSTR testStr = L"W";
	RECT rcDraw = {0, 0, 3, 3};
	DrawText(hdc, testStr, -1, &rcDraw, DT_CALCRECT | DT_EDITCONTROL);
	m_size = (rcDraw.right - rcDraw.left) * m_size;
	SelectObject(hdc, oldFont);
	ReleaseDC(NULL, hdc);

	return TRUE;
}
