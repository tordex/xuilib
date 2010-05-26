#pragma once
#include "xuielement.h"

class CXUISelectFolder : public CXUIElement
{
	LPWSTR	m_title;
	LPWSTR	m_saveto;
	LPWSTR	m_selFolder;
public:
	CXUISelectFolder(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUISelectFolder(void);

	IMPLEMENT_INTERFACE(L"selectfolder")

	BOOL loadDATA(IXMLDOMNode* node);
	void Init();
	virtual void	value_STR(LPCWSTR val);
	virtual LPCWSTR	value_STR();

	virtual void doDefaultAction(CXUIElement* el);
private:
	static int CALLBACK BrowseIconFldProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
};
