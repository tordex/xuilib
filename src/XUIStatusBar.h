#pragma once
#include "xuielement.h"

class CXUIStatusBar : public CXUIElement
{
	BOOL	m_bSizeGrip;
public:
	CXUIStatusBar(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIStatusBar(void);

	IMPLEMENT_INTERFACE(L"statusbar")

	virtual BOOL	loadDATA(IXMLDOMNode* node);
	virtual void	Init();
	virtual void	render(int x, int y, int width, int height);
};

class CXUISBPart : public CXUIElement
{
	int m_flex;
	int m_size;
	int	m_partIDX;
public:
	CXUISBPart(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUISBPart(void);

	IMPLEMENT_INTERFACE(L"sbpart")

	virtual BOOL	loadDATA(IXMLDOMNode* node);
	virtual void	Init();
	int				getFlex() { return m_flex; }
	int				getSize() { return m_size; }
	void			setPartIDX(int idx)	{ m_partIDX = idx; }
	virtual void	value_STR(LPCWSTR val);
};