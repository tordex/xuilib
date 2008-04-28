#include <windows.h>

#ifndef __HTMLVIEWER_H__
#define __HTMLVIEWER_H__

class CHTMLStringVector
{
	TCHAR **m_pszStrings;
	int *m_pnStrings;
	int *m_pnIndexValues;
	int m_nStrings;
public:
	CHTMLStringVector();
	~CHTMLStringVector();
	void Clean();
	int AddString(TCHAR *szString,int nSize=-1);
	int SetString(int nIndex, TCHAR *szString, int nSize=-1);
	TCHAR *GetString(int nIndex,int *pnSize=NULL);
	int GetCount();
	int SetCount(int nCount);
	TCHAR *operator [] (int nIndex);
	CHTMLStringVector &operator =(CHTMLStringVector &s);
	int MakeIndex(TCHAR *szString, int nLenght);
	int GetIndex(int nIndex);
	void DeleteString(int nIndex);

	int MoveString(int nFrom, int nTo);
};
class CHTMLStringTable
{
	CHTMLStringVector **m_pVektors;
	CHTMLStringVector m_err;
	int m_nVektors;
	int m_nRowSize;
public:
	CHTMLStringTable();
	~CHTMLStringTable();
	int AddRow(TCHAR **pszRow, int nRow);
	int AddRow();
	int InsertRow(int nRow);
	int AddString(int nRow, TCHAR *szString, int nString=-1);
	int DeleteRow(int nRow);
	int SetString(int nRow, int nIndex, TCHAR *szString, int nString=-1);
	TCHAR* GetString(int nRow, int nIndex,int *pnSize=NULL);
	int GetCount();

	CHTMLStringVector &operator [] (int nRow);
	CHTMLStringTable &operator = (CHTMLStringTable &st);

	int MoveRow(int nFrom, int nTo);

	void Reset();
	int SplitString(TCHAR *szData,TCHAR *szFieldSep, TCHAR *szRowSep);
};

enum TYPES{
	TEXT,
	IMAGE,
	BREAK,
	COMPOUND
};

///////////////////////////////////////////////////////////////////////////////
//
// CHTMLProgress
//
// This class should be implemented by the user of CHTMLViewer.
//
///////////////////////////////////////////////////////////////////////////////
class CHTMLProgress
{
public:
	virtual void LinkClicked(TCHAR *szLink) = 0;
	virtual void SetCursor(BOOL bLink,TCHAR *szLink) = 0;
	virtual void SetTitle(TCHAR *szTitle) = 0;

	virtual void UpdateScreen(RECT &r) { };
};

class CHTMLBitmaps;
class CHTMLViewer;

class CHTMLItem
{
public:
	int m_nType;
	SIZE m_scBounds;
	POINT m_pPos;
	int m_nFrames;
	int m_nFrameSize;
	COLORREF m_colFrame;
	TCHAR m_szFont[100];
	int m_nFontSize;
	int m_nFontBold;
	int m_nFontItalic;
	COLORREF m_colText;
	COLORREF m_colBkg;
	int m_nHFormat;
	int m_nVFormat;
	TCHAR *m_szText;
	int m_nBmp;
	int m_nBmpBkg;
	int m_nRow;
	int m_nCol;
	int m_nColSpan;
	int m_nRowSpan;
	int m_nWidth;
	int m_nTableWidth;
	int m_nTableFrameSize;
	int m_nHeight;
	int m_nTableHeight;
	int m_nCellPadding;
	int m_nCellSpacing;
	int m_nLink;
	HANDLE m_hEvent;
	int m_nMinColumnWidth;
	int m_nAMinColumnWidth;
	int m_nMaxColumnWidth;
	int m_nColumnWidth;
	BOOL m_bSelected;
	BOOL m_bNoBreak;
	int m_nControlID;
	int m_nAbsoluteAlign;
	int m_bClear;

	CHTMLItem **m_ppSubItems;
	int m_nSubItems;

	CHTMLItem();
	~CHTMLItem();

	static void GetStringSize(TCHAR *szFont,int nFontSize, int bBold, int bItalic, SIZE *sc,TCHAR *szString,int nMaxWidth,int nLen);
	void Offset(int nX, int nY);
	// Text-object
	static CHTMLItem *NewText(TCHAR *szText,int nFrames,int nFrameSize, COLORREF colFrame,TCHAR *szFont,int nFontSize,
		int nFontBold,int nFontItalic,COLORREF colText,COLORREF colBkg,int nHFormat,int nVFormat,int nLink,BOOL bNoBreak);
	// Break-object
	static CHTMLItem *NewBreak(int nHeight,int nWidth,int nHFormat,int bClear);
	// Compound-object
	static CHTMLItem *NewCompound(int nRow, int nCol, int nWidth,int nHFormat,int nVFormat, COLORREF colBkg,BOOL bBorder,int nColSpan,int nRowSpan,int nCellPadding,int nCellSpacing,int nTableWidth,int nBmpBkg,int nHeight,int nTableHeight,COLORREF colBorder,int nAbsoluteAlign);
};

class CHTMLViewer
{
	int m_nItemCount;
	int m_nLevelCount;
	CHTMLItem **m_ppItems;
	int m_nItems;
	CHTMLProgress *m_pProgress;
	CHTMLStringTable m_Links;
	COLORREF m_bgCol;
	int m_nBmpBkg;
	int m_nActiveLink;
	int m_nListLevel;
	BOOL m_bReading;
	BOOL m_bWaitForImages;

	void AddItem(CHTMLItem **&ppItems, int &nItems);

	void AddHTML(TCHAR *szHTMLData, CHTMLItem **&ppItems, int &nItems,TCHAR *szFont,int nFontSize,COLORREF colText,TCHAR *szBaseUrl,int nLevel,int nHFormat,BOOL bNoBreak);
	void CalcPositions(RECT &rPos, CHTMLItem **&ppItems, int &nItems);
	void Draw(HDC hDC, RECT &r, CHTMLItem **&ppItems, int &nItems,int nX, int nY);
	int OnMouseOver(CHTMLItem **&ppItems, int &nItems,int nX, int nY,BOOL bClick);

	void GetProperties(TCHAR *szData,CHTMLStringTable &st);
	COLORREF GetCol(TCHAR *szData);
	void AdjustHR(int nWidth, CHTMLItem **&ppItems, int &nItems);
	void Select(RECT &r,CHTMLItem **&ppItems, int &nItems);
	void CopySelected(CHTMLItem **&ppItems, int &nItems,TCHAR *&szCopy,int &nLen);
	void UpdateImage(CHTMLItem **&ppItems, int &nItems,int nImage);
	void UpdateLink(CHTMLItem **&ppItems, int &nItems,int nLink);
public:
	CHTMLViewer(CHTMLProgress *pProgress);
	~CHTMLViewer();
	void SetHTML(TCHAR *szHTMLData,TCHAR *szFont,int nFontSize,COLORREF colText,TCHAR *szBaseUrl);
	void CalcPositions(RECT &rPos);
	void Draw(HDC hDC,RECT &r, int nX, int nY);

	void OnMouseOver(int x, int y);
	void OnMouseClick(int x, int y);

	void Statistics(int &nItems, int &nImages,int &nLinks,int &nLevel);
	void Select(RECT &r);
	void CopySelected();

	void AddBaseToUrl(TCHAR *szUrl,TCHAR *szFullUrl,TCHAR *szBase);

	void UpdateImage(int nImage);
	void UpdateLink(TCHAR *szLink);
	BOOL IsReading();
};

#endif