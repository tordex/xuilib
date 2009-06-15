#pragma once

struct cell_info
{
	int		cx;
	int		cy;
	int		row;
	int		col;
	int		hFlex;
	int		vFlex;
	int 	colSpan;
	int 	rowSpan;
};

class CRenderData
{
	cell_info*	m_items;
	int			m_count;
	BOOL		m_bNormalized;
public:
	CRenderData(int count);
	~CRenderData(void);

	cell_info*	get(int idx);
	int			getColWidth(int col);
	void		setColWidth(int col, int width);

	int			getRowHeight(int row);
	void		setRowHeight(int row, int height);

	int			getRows();
	int			getCols();
	
	int			getRowFlex(int row);
	int			getColFlex(int col);

	void		normalize(int cellSpaceX, int cellSpaceY);

	void processSpan( int cellSpaceX, int cellSpaceY );
	void		processFlex(int width, int height, int cellSpaceX, int cellSpaceY);
};
