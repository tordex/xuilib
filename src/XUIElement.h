#pragma once

#define XUI_ORIENT_HORIZONTAL	0
#define XUI_ORIENT_VERTICAL		1

#define XUI_MARGIN_LEFT			0
#define XUI_MARGIN_TOP			1
#define XUI_MARGIN_RIGHT		2
#define XUI_MARGIN_BOTTOM		3

#define IMPLEMENT_INTERFACE(name)	virtual BOOL QueryElement(LPWSTR elementName, LPVOID* pElement)\
	{\
		if(!lstrcmpiW(name, elementName))\
		{\
			if(pElement) \
			{ \
				*pElement = (LPVOID) this;\
			} \
			return TRUE;\
		}\
		if(pElement) \
		{ \
			*pElement = NULL;\
		} \
		return FALSE;\
	}

class CXUIElement
{
	friend class CXUIDialog;
	friend class CXUIRadioGroup;
	friend class CXUITabBox;
	friend class CXUIEngine;
protected:

	struct cell_info
	{
		int	size;
		int flex;
	};

	// data members
	CXUIElement*	m_parent;
	CXUIEngine*		m_engine;
	UINT			m_id;
	LPWSTR			m_strID;
	int				m_left;
	int				m_top;
	int				m_width;
	int				m_height;
	int				m_maxWidth;
	int				m_maxHeight;
	int				m_minWidth;
	int				m_minHeight;
	int				m_hidden;
	int				m_disabled;
	int				m_hFlex;
	int				m_vFlex;
	int				m_orient;
	int				m_cellSpaceX;
	int				m_cellSpaceY;
	int				m_cols;
	int				m_rows;
	HWND			m_hWnd;
	LPWSTR			m_lockid;

	int				m_marginLeft;
	int				m_marginRight;
	int				m_marginTop;
	int				m_marginBottom;

	//layout data
	int				m_layX;
	int				m_layY;
	int				m_layWidth;
	int				m_layHeight;

	CXUIElement**	m_childs;
	int				m_childCount;
protected:
	// internal functions
	void RowsColsSize( cell_info* cinf, int cols, cell_info* rinf, int rows );
	void ProcessFlex(cell_info* cinf, int count, int size, int cellSpace);
	LPWSTR getLocalizedString(IXMLDOMNode* item, LPCTSTR attrName);
public:
	CXUIElement(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIElement(void);

	IMPLEMENT_INTERFACE(L"")

	void clearChilds();
	BOOL raiseEvent(LPCWSTR evID, WPARAM wParam, LPARAM lParam);
	void processDefaultAction();

	virtual void getMinSize(SIZE& minSize);
	virtual void render(int x, int y, int width, int height);
	virtual BOOL loadDATA(IXMLDOMNode* node);
	virtual BOOL loadFile(LPCTSTR fileName, LPCTSTR defNode, HINSTANCE hInst = NULL);
	virtual void addChild(CXUIElement* child);
	virtual void Init();
	virtual void doDefaultAction(CXUIElement* el);
	virtual BOOL OnEvent(CXUIElement* el, LPCWSTR evID, WPARAM wParam, LPARAM lParam);
	virtual BOOL isInitialized();
	virtual UINT get_newDlgID();

	virtual BOOL onNotify(int idCtrl, LPNMHDR pnmh);
	virtual BOOL onCommnd(UINT code, UINT id, HWND hWnd);

	CXUIElement*	get_root();
	CXUIElement*	find(HWND hWnd);
	CXUIElement*	find(LPTSTR id);
	CXUIElement*	find(UINT id);

	CXUIElement*	get_parent() { return m_parent; }
	int				get_width() { return m_width; }
	int				get_height() { return m_height; }
	int				get_left() { return m_left; }
	int				get_top() { return m_top; }
	HWND			get_wnd() { return m_hWnd; };
	virtual int		get_margin(int mgidx) { return 0; }
	virtual HWND	get_parentWnd();
	CXUIElement*	get_childIDX(int idx) { return (idx >=0 && idx < m_childCount) ? m_childs[idx] : NULL; }
	int				get_childsCount() { return m_childCount; }
	LPWSTR			get_id() { return m_strID; }
	UINT			get_dlgid() { return m_id; }

	virtual INT		value_INT();
	virtual void	value_INT(INT val);
	virtual LPCWSTR	value_STR();
	virtual void	value_STR(LPCWSTR val);
	LPCWSTR			value_STRdef(LPCWSTR defVal);
	virtual	void	onCmd(LPWSTR cmd);

	BOOL			set_TabStopFocus();
	void			set_hidden(int hide);
	virtual void	updateHiddenState();
	void			set_disabled(int disable);
	virtual void	updateDisabledState();
	virtual int		get_disabled();
	virtual int		get_hidden();
	virtual void	recalcHeight(int width);

	void			size(int width, int height);
};
