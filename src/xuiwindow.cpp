#include "xuilib.h"
#include "xuiwindow.h"
#include ".\xuiwindow.h"
#pragma comment(lib, "comsupp.lib")

CXUIWindow::CXUIWindow(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
}

CXUIWindow::~CXUIWindow()
{
}

