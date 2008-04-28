#pragma once
#include "xuielement.h"

class CXUIProgress : public CXUIElement
{
	BOOL	m_bMarquee;
	BOOL	m_bSmooth;
	BOOL	m_bSmoothreverse;
	BOOL	m_bVertical;
	int		m_lowLimit;
	int		m_highLimit;
	int		m_pos;
public:
	CXUIProgress(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIProgress(void);

	IMPLEMENT_INTERFACE(L"progress")

	virtual BOOL loadDATA(IXMLDOMNode* node);
	virtual void Init();

	void set_limits(int low, int high);
	void set_pos(int pos);
};
