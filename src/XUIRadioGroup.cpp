#include "xuilib.h"

CXUIRadioGroup::CXUIRadioGroup(CXUIElement* parent, CXUIEngine* engine) : CXUIElement(parent, engine)
{
}

CXUIRadioGroup::~CXUIRadioGroup(void)
{
}

void CXUIRadioGroup::Init()
{
	RECT rcMap = { 4, 3, 7, 7};
	MapDialogRect(m_hWnd, &rcMap);
	m_cellSpaceX	= rcMap.left;
	m_cellSpaceY	= rcMap.top;

	for(int i=0; i < m_childCount; i++)
	{
		CXUIRadio* btn = NULL;
		m_childs[i]->QueryElement(TEXT("radio"), (LPVOID*) &btn);
		if(btn)
		{
			btn->set_group(i ? FALSE : TRUE);
		}
		m_childs[i]->Init();
	}
}

INT CXUIRadioGroup::value_INT()
{
	int ret = 0;
	for(int i=0; i < m_childCount; i++)
	{
		CXUIRadio* btn = NULL;
		m_childs[i]->QueryElement(TEXT("radio"), (LPVOID*) &btn);
		if(btn)
		{
			if(btn->value_INT())
			{
				ret = btn->get_value();
				break;
			}
		}
	}
	return ret;
}

void CXUIRadioGroup::value_INT( INT val )
{
	for(int i=0; i < m_childCount; i++)
	{
		CXUIRadio* btn = NULL;
		m_childs[i]->QueryElement(TEXT("radio"), (LPVOID*) &btn);
		if(btn)
		{
			if(btn->get_value() == val)
			{
				btn->value_INT(1);
			} else
			{
				btn->value_INT(0);
			}
		}
	}
}

BOOL CXUIRadioGroup::OnEvent( CXUIElement* el, LPCWSTR evID, WPARAM wParam, LPARAM lParam )
{
	if(el && el->QueryElement(L"radio", NULL) && IS_SAME_STR(evID, XUI_EVENT_CHANGED))
	{
		return raiseEvent(XUI_EVENT_CHANGED, 0, NULL);
	}
	return CXUIElement::OnEvent(el, evID, wParam, lParam);
}
