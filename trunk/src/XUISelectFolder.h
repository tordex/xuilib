#pragma once
#include "xuielement.h"

class CXUISelectFolder : public CXUIElement
{
	LPWSTR	m_title;
public:
	CXUISelectFolder(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUISelectFolder(void);
};
