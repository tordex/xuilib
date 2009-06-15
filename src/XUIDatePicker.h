#pragma once
#include "xuielement.h"

#define XUI_DATE_FORMAT_SHORT		0
#define XUI_DATE_FORMAT_LONG		1
#define XUI_DATE_FORMAT_TIME		2

class CXUIDatePicker : public CXUIElement
{
	int		m_format;
	BOOL	m_showNone;
	BOOL	m_useSpin;
public:
	CXUIDatePicker(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIDatePicker(void);

	IMPLEMENT_INTERFACE(L"datepicker")

	virtual BOOL loadDATA(IXMLDOMNode* node);
	virtual void Init();
	virtual HWND get_parentWnd();

	BOOL isNone();
	void getDate(LPSYSTEMTIME pTm);
	void setDate(LPSYSTEMTIME pTm, BOOL isNone = FALSE);
	void setFormat(LPWSTR format);
};
