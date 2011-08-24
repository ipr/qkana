#ifndef LANGUAGEDATA_H
#define LANGUAGEDATA_H

#include <QString>

// use db for persisent storage
#include <db_cxx.h>

class CAbstractTable
{
protected:
	uint64_t m_nCount;
	QString m_szName;
	
public:
	CAbstractTable()
	    : m_nCount(0)
	    , m_szName()
	{}
};

/*
class CSentenceTable : protected CAbstractTable
{
};

class CReadingTable : protected CAbstractTable
{
};

class CMeaningTable : protected CAbstractTable
{
};
*/

class CLanguageData
{
protected:
	Db m_db;

	//bool Read(CAbstractTable *pTable, Dbt &Key, Dbt &Value);
	//bool Write(CAbstractTable *pTable, Dbt &Key, Dbt &Value);
	
public:
    CLanguageData();
    virtual ~CLanguageData();
	
	bool init(QString &appPath);
	bool close();
	
	// get meanings of single reading
	//QStringList getMeanings(QString &reading);
	
	// add meaning to reading (new or existing)
	//bool addMeaning(QString &reading, QString &meaning);
	
	// keep whole sentence (for testing, now)
	//bool keepSentence(QString &reading);
};

#endif // LANGUAGEDATA_H
