#pragma once

struct LOC_STR
{
	LPWSTR	id;
	LPWSTR	attr;
	LPWSTR	loc;
};

class CXUIEngine
{
protected:
	HINSTANCE	m_hInst;

	LOC_STR*	m_strings;
	int			m_stringsCount;
public:
	CXUIEngine(HINSTANCE hInst);
	virtual ~CXUIEngine(void);

	HINSTANCE get_hInstance() { return m_hInst; }

	void clearStrings();
	void loadStrings(LPWSTR stringsFile);
	
	LPWSTR getString(LPCWSTR id, LPCWSTR attr = NULL);
	LPWSTR getStringDef(LPCWSTR id, LPCWSTR attr, LPWSTR defString);

	virtual CXUIElement*	createXUIElement(LPCWSTR name, CXUIElement* parent);
	virtual void			DrawImage(HDC hdc, int x, int y, int width, int height, LPWSTR imgUrl);
	virtual void			DrawFrame(HDC hdc, int x, int y, int width, int height, LPWSTR imgUrl, int frames, int frame, int framesOrient);
	virtual void			openHelpTopic(HWND hWnd, LPCWSTR topic, UINT flags);
	virtual BOOL			isViewer();
};
