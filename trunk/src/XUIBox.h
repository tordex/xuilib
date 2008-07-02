#pragma once
#include "xuielement.h"

class CXUIBox :	public CXUIElement
{
	int m_defOrient;
public:
	CXUIBox(CXUIElement* parent, CXUIEngine* engine, int defOrient);
	~CXUIBox(void);

	IMPLEMENT_INTERFACE(L"box")

	int		get_margin(int mgidx);
	BOOL	loadDATA(IXMLDOMNode* node);
	void	Init();
};
