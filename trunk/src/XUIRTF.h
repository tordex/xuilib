#pragma once
#include "xuielement.h"

#define XUI_RTF_ALIGN_LEFT		0
#define XUI_RTF_ALIGN_RIGHT		1
#define XUI_RTF_ALIGN_CENTER	3

class CXUIRTF : public CXUIElement
{
	LPWSTR	m_value;
	UINT	m_style;
public:
	CXUIRTF(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIRTF(void);

	IMPLEMENT_INTERFACE(L"rtf")

	virtual BOOL	loadDATA(IXMLDOMNode* node);
	virtual void	Init();
	virtual LPCWSTR	value_STR();
	virtual void	value_STR(LPCWSTR val);
	virtual BOOL	onNotify(int idCtrl, LPNMHDR pnmh);
};
