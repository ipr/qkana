// may already exist in Qt, can't bother to look for it..
// simple smartpointer, use from stack always.
//
// Ilkka Prusi, 2011

#ifndef QAUTOPTR_H
#define QAUTOPTR_H

// fwd. decl.
class QObject;

class qAutoPtr
{
protected:
	QObject *m_ob;
	
public:
	qAutoPtr(QObject *pOb)
	    : m_ob(pOb)
	{}
	~qAutoPtr()
	{
		delete m_ob;
	}

    qAutoPtr & operator*()
	{
		return *m_ob;
	}
    qAutoPtr * operator->()
	{
		return m_ob;
	}
};

#endif // QAUTOPTR_H
