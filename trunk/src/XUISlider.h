#pragma once
#include "xuielement.h"

#define XUI_SLIDER_TYPE_HORIZONTAL	0
#define XUI_SLIDER_TYPE_VERTICAL	1

class CXUISlider : public CXUIElement
{
	BOOL	m_bAutoTicks;
	BOOL	m_bNoTicks;
	int		m_minValue;
	int		m_maxValue;
	int		m_pos;
	int		m_ticFreq;
	int		m_type;
	BOOL	m_bInvert;
public:
	CXUISlider(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUISlider(void);

	IMPLEMENT_INTERFACE(L"slider")

	virtual void Init();
	virtual BOOL loadDATA(IXMLDOMNode* node);

	virtual INT		value_INT();
	virtual void	value_INT(INT val);

	int maxValue() { return m_maxValue; }
	int minValue() { return m_minValue; }
};
