#pragma once
#include "xuielement.h"

#define XUI_VSPLITTER_CLASS		L"XUI_VSPLITTER_CLASS"
#define XUI_HSPLITTER_CLASS		L"XUI_HSPLITTER_CLASS"

class CXUIVSplitter : public CXUIElement
{
	int		m_downX;
	BOOL	m_inCapture;
public:
	CXUIVSplitter(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIVSplitter(void);

	IMPLEMENT_INTERFACE(L"vsplitter")

	virtual void	Init();
	virtual void	value_INT(INT val);
	virtual INT		value_INT();

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void OnPaint(HDC hdc, LPRECT rcDraw);
	void OnLButtonDown(int x, int y);
	void OnLButtonUp(int x, int y);
	void OnMouseMove(int x, int y);
};

class CXUIHSplitter : public CXUIElement
{
	int		m_downY;
	BOOL	m_inCapture;
public:
	CXUIHSplitter(CXUIElement* parent, CXUIEngine* engine);
	virtual ~CXUIHSplitter(void);

	IMPLEMENT_INTERFACE(L"hsplitter")

	virtual void	Init();
	virtual void	value_INT(INT val);
	virtual INT		value_INT();

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void OnPaint(HDC hdc, LPRECT rcDraw);
	void OnLButtonDown(int x, int y);
	void OnLButtonUp(int x, int y);
	void OnMouseMove(int x, int y);
};
