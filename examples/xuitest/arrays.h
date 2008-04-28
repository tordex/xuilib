#if !defined(AFX_ARRAYS_H__C6B70AC1_2D8D_4916_9E55_A40EB385DFA6__INCLUDED_)
#define AFX_ARRAYS_H__C6B70AC1_2D8D_4916_9E55_A40EB385DFA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template <class T> class CTArray
{
	int m_count;
	T* m_items;
public:
	CTArray();
	virtual ~CTArray();
	T& operator[](int i) { return m_items[i]; }
	int GetCount() { return m_count; }
	void Add(T* vItem);
	void Insert(int idx, T* vItem);
	void Clear();
	void Delete(int index);
	CTArray<T>& operator=(CTArray<T>& v);
	T* GetItems() { return m_items; }
	int GetSizeOfItems() { return m_count * sizeof(T); }
	void SetSize(int newSize);
};

template <class T> 
CTArray<T>::CTArray()
{
	m_count = 0;
	m_items = 0;
}

template <class T> 
CTArray<T>::~CTArray()
{
	Clear();
}

template <class T> 
void CTArray<T>::Add(T* vItem)
{
	if(!m_items)
	{
		m_items = (T*) malloc(sizeof(T));
		memcpy(m_items, vItem, sizeof(T));
		m_count = 1;
	} else
	{
		m_items = (T*) realloc(m_items, sizeof(T) * (m_count + 1));
		memcpy(m_items + m_count, vItem, sizeof(T));
		m_count++;
	}
}

template <class T> 
void CTArray<T>::Insert(int idx, T* vItem)
{
	if(!m_items)
	{
		Add(vItem);
		return;
	} else
	{
		m_items = (T*) realloc(m_items, sizeof(T) * (m_count + 1));
		for(int i = m_count; i > idx; i--)
		{
			memcpy(m_items + i, m_items + i - 1, sizeof(T));
		}
		memcpy(m_items + idx, vItem, sizeof(T));
		m_count++;
	}
}

template <class T> 
void CTArray<T>::Clear()
{
	if(m_items)
	{
		free(m_items);
	}
	m_items = 0;
	m_count = 0;
}

template <class T> 
void CTArray<T>::Delete(int index)
{
	if(index >= 0 && index < m_count)
	{
		for(int i=index; i < m_count-1; i++)
		{
			memcpy(m_items + i, m_items + i + 1, sizeof(T));
		}
		if(m_count > 1)
		{
			m_items = (T*) realloc(m_items, sizeof(T) * (m_count - 1));
			m_count--;
		} else
		{
			Clear();
		}
	}
}


template <class T> 
CTArray<T>& CTArray<T>::operator=(CTArray<T>& v)
{
	Clear();
	if(v.m_count)
	{
		m_items = (T*) malloc(sizeof(T) * v.m_count);
		memcpy(m_items, v.m_items, sizeof(T) * v.m_count);
		m_count = v.m_count;
	}
	return *this;
}

template <class T> 
void CTArray<T>::SetSize(int newSize)
{
	if(!m_items)
	{
		m_items = (T*) malloc(sizeof(T) * newSize);
		m_count = newSize;
		ZeroMemory(m_items, sizeof(T) * newSize);
	} else
	{
		m_items = (T*) realloc(m_items, sizeof(T) * newSize);
		m_count = newSize;
		ZeroMemory(m_items, sizeof(T) * newSize);
	}
}

#endif // !defined(AFX_ARRAYS_H__C6B70AC1_2D8D_4916_9E55_A40EB385DFA6__INCLUDED_)
