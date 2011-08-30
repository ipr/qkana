// may already exist in Qt, can't bother to look for it..
// simple smartpointer, use from stack always.
//
// redone as template..
//
// Ilkka Prusi, 2011

#ifndef QAUTOPTR_H
#define QAUTOPTR_H

// fwd. decl.
//class QObject;

template <class T> class qAutoPtr
{
protected:
	T *m_ob;
	
public:
	qAutoPtr(T *pOb)
	    : m_ob(pOb)
	{}
	~qAutoPtr()
	{
		delete m_ob;
	}

    T & operator*()
	{
		return *m_ob;
	}
    T * operator->()
	{
		return m_ob;
	}
};

#endif // QAUTOPTR_H
