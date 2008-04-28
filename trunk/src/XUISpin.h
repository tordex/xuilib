#pragma once
#include "xuielement.h"

#define XUI_SPIN_ALIGN_LEFT		0
#define XUI_SPIN_ALIGN_RIGHT	1


class CXUISpin : public CXUIElement
{
	int	m_buddyAlign;
	int m_upper;
	int m_lower;
	int m_pos;
public:
	CXUISpin(CXUIElement* parent, CXUIEngine* engine);
	~CXUISpin(void);
	IMPLEMENT_INTERFACE(L"spin")

	BOOL loadDATA(IXMLDOMNode* node);
	void Init();
	void render(int x, int y, int width, int height);
};
