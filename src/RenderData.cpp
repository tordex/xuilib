#include "xuilib.h"
#include "RenderData.h"

CRenderData::CRenderData(int count)
{
	m_items = new cell_info[count];
	ZeroMemory(m_items, sizeof(cell_info) * count);
	m_bNormalized	= FALSE;
	m_count			= count;
}

CRenderData::~CRenderData(void)
{
	delete m_items;
}

cell_info* CRenderData::get( int idx )
{
	return &m_items[idx];
}

int CRenderData::getColWidth( int col )
{
	int width = 0;
	for(int i=0; i < m_count; i++)
	{
		if(m_items[i].col == col && width < m_items[i].cx && m_items[i].colSpan == 1)
		{
			width = m_items[i].cx;
		}
	}
	return width;
}

void CRenderData::setColWidth( int col, int width )
{
	for(int i=0; i < m_count; i++)
	{
		if(m_items[i].col == col && m_items[i].colSpan == 1)
		{
			m_items[i].cx = width;
		}
	}
}

int CRenderData::getRowHeight( int row )
{
	int height = 0;
	for(int i=0; i < m_count; i++)
	{
		if(m_items[i].row == row && height < m_items[i].cy && m_items[i].rowSpan == 1)
		{
			height = m_items[i].cy;
		}
	}
	return height;
}

void CRenderData::setRowHeight( int row, int height )
{
	for(int i=0; i < m_count; i++)
	{
		if(m_items[i].row == row && m_items[i].rowSpan == 1)
		{
			m_items[i].cy = height;
		}
	}
}

int CRenderData::getRows()
{
	int rows = 0;
	for(int i=0; i < m_count; i++)
	{
		if(rows < m_items[i].row)
		{
			rows = m_items[i].row;
		}
	}
	return rows + 1;
}

int CRenderData::getCols()
{
	int cols = 0;
	for(int i=0; i < m_count; i++)
	{
		if(cols < m_items[i].col)
		{
			cols = m_items[i].col;
		}
	}
	return cols + 1;
}

void CRenderData::normalize(int cellSpaceX, int cellSpaceY)
{
	int cols = 0;
	int rows = 0;
	for(int i=0; i < m_count; i++)
	{
		if(cols < m_items[i].col)
		{
			cols = m_items[i].col;
		}
		if(rows < m_items[i].row)
		{
			rows = m_items[i].row;
		}
	}
	cols++;
	rows++;

	// find the max width and height of the flex cells
	int flexWidth	= 0;
	int flexHeight	= 0;
	for(int i=0; i < cols; i++)
	{
		if(getColFlex(i))
		{
			int width = getColWidth(i);
			if(flexWidth < width) flexWidth = width;
		}
	}
	for(int i=0; i < rows; i++)
	{
		if(getRowFlex(i))
		{
			int height = getRowHeight(i);
			if(flexHeight < height) flexHeight = height;
		}
	}

	for(int i=0; i < cols; i++)
	{
		if(getColFlex(i))
		{
			setColWidth(i, flexWidth);
		} else
		{
			setColWidth(i, getColWidth(i));
		}
	}
	for(int i=0; i < rows; i++)
	{
		if(getRowFlex(i))
		{
			setRowHeight(i, flexHeight);
		} else
		{
			setRowHeight(i, getRowHeight(i));
		}
	}
	processSpan(cellSpaceX, cellSpaceY);
}

void CRenderData::processFlex( int width, int height, int cellSpaceX, int cellSpaceY )
{
	int cols = getCols();
	int rows = getRows();

	// process columns flex
	{
		int sz = width - (cols - 1) * cellSpaceX;
		int flex = 0;
		for(int i=0; i < cols; i++)
		{
			int colFlex = getColFlex(i);
			if(!colFlex)
			{
				sz -= getColWidth(i);
			} else
			{
				flex += colFlex;
			}
		}
		if(sz > 0 && flex)
		{
			for(int i=0; i < cols; i++)
			{
				int colFlex = getColFlex(i);
				if(colFlex)
				{
					setColWidth(i, sz * colFlex / flex);
				}
			}
		}
	}
	// process rows flex
	{
		int sz = height - (rows - 1) * cellSpaceY;
		int flex = 0;
		for(int i=0; i < rows; i++)
		{
			int rowFlex = getRowFlex(i);
			if(!rowFlex)
			{
				sz -= getRowHeight(i);
			} else
			{
				flex += rowFlex;
			}
		}
		if(sz > 0 && flex)
		{
			for(int i=0; i < rows; i++)
			{
				int rowFlex = getRowFlex(i);
				if(rowFlex)
				{
					setRowHeight(i, sz * rowFlex / flex);
				}
			}
		}
	}

	processSpan(cellSpaceX, cellSpaceY);
}

int CRenderData::getRowFlex( int row )
{
	int flex = 0;
	for(int i=0; i < m_count; i++)
	{
		if(m_items[i].row == row && flex < m_items[i].vFlex)
		{
			flex = m_items[i].vFlex;
		}
	}
	return flex;
}

int CRenderData::getColFlex( int col )
{
	int flex = 0;
	for(int i=0; i < m_count; i++)
	{
		if(m_items[i].col == col && flex < m_items[i].hFlex)
		{
			flex = m_items[i].hFlex;
		}
	}
	return flex;
}

void CRenderData::processSpan( int cellSpaceX, int cellSpaceY )
{
	for(int i=0; i < m_count; i++)
	{
		if(m_items[i].colSpan > 1)
		{
			int width = cellSpaceX * (m_items[i].colSpan - 1);
			for(int col = m_items[i].col; col < m_items[i].col + m_items[i].colSpan; col++)
			{
				width += getColWidth(col);
			}
			if(width < m_items[i].cx)
			{
				int colWidth = m_items[i].cx - width + getColWidth(m_items[i].col);
				setColWidth(m_items[i].col, colWidth);
			} else
			{
				m_items[i].cx = width;
			}
		}
		if(m_items[i].rowSpan > 1)
		{
			int height = cellSpaceY * (m_items[i].rowSpan - 1);
			for(int row = m_items[i].row; row < m_items[i].row + m_items[i].rowSpan; row++)
			{
				height += getRowHeight(row);
			}
			if(height < m_items[i].cy)
			{
				int rowHeight = m_items[i].cy - height + getRowHeight(m_items[i].row);
				setRowHeight(m_items[i].row, rowHeight);
			} else
			{
				m_items[i].cy = height;
			}
		}
	}
}