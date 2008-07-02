#pragma once
#include "xuielement.h"

#define XUI_SELECTFILE_TYPE_OPEN	0
#define XUI_SELECTFILE_TYPE_SAVE	1

class CXUISelectFile : public CXUIElement
{
	BOOL	m_allowMultiSelect;
	BOOL	m_dontAddRecent;
	BOOL	m_enableSizing;
	BOOL	m_explorer;
	BOOL	m_fileMustExist;
	BOOL	m_forceHidden;
	BOOL	m_hideReadOnly;
	BOOL	m_longNames;
	BOOL	m_noNetworkButton;
	BOOL	m_noReadOnlyReturn;
	BOOL	m_noTestFileCreate;
	BOOL	m_noValidate;
	BOOL	m_overwriteprompt;
	BOOL	m_pathmustexist;
	BOOL	m_readonly;
	BOOL	m_shareaware;
	BOOL	m_showhelp;
	BOOL	m_createprompt;

	LPWSTR	m_defExt;
	LPWSTR	m_title;
	LPWSTR	m_saveto;
	LPWSTR	m_fileName;
	UINT	m_type;
public:
	CXUISelectFile(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUISelectFile(void);

	IMPLEMENT_INTERFACE(L"selectfile")

	BOOL loadDATA(IXMLDOMNode* node);
	void Init();

	virtual void doDefaultAction(CXUIElement* el);

	void clearExt();
	void addExt(LPWSTR text, LPWSTR ext);
	void   fileName(LPWSTR flName);
	LPWSTR fileName() { return m_fileName; }
};
