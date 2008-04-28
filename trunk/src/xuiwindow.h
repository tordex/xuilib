#pragma	once
#include <atlbase.h>
#include <MsXml.h>
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
#include "XUIElement.h"

class CXUIWindow : public CXUIElement
{
public:
	CXUIWindow(CXUIElement* node, CXUIEngine* engine);
	virtual ~CXUIWindow();
};
