#pragma once

class CMyXUIEngine : public CXUIEngine
{
	LPWSTR	m_imagesPath;
public:
	CMyXUIEngine(HINSTANCE hInst, LPCWSTR projectFile);
	virtual ~CMyXUIEngine(void);

	virtual void DrawImage(HDC hdc, int x, int y, int width, int height, LPWSTR imgUrl);
	virtual BOOL isViewer();
};
