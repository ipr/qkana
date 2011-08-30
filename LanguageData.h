/////////////////////////////////////////////
// Ilkka Prusi, 2011

#ifndef LANGUAGEDATA_H
#define LANGUAGEDATA_H

#include <QString>

// use db for persistent storage
#include <db_cxx.h>

/////////////
// TODO: check approach..
// BDB just stores key/value pairs
// and has no concept of tables (though it has partitions..)
// -> for many-to-many relations we may need 
// some abstraction of data relations and way to store that..
// -> find better way ?
//
// (kanji+kana combination and choice of characters may change meaning
// even if pronounced the same..)
//

/*
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

class QByteArray;
class QTextCodec;

class CLanguageData
{
protected:
	Db m_db;
	bool m_bIsOpen;

	//bool Read(CAbstractTable *pTable, Dbt &Key, Dbt &Value);
	//bool Write(CAbstractTable *pTable, Dbt &Key, Dbt &Value);
	
	bool Lookup(QString &source, QByteArray &output);
	bool Store(QString &Key, QByteArray &Value);
	
	bool appendDictionary(uchar *pData, qint64 iSize, QTextCodec *codec);
	
public:
    CLanguageData();
    virtual ~CLanguageData();
	
	bool init(QString &appPath);
	bool close();

	// include dictionary file in language database
	// (append missing information)
	bool includeDictionary(QString &dictFile);
	
	// get meanings of single reading
	//QStringList getMeanings(QString &reading);
	
	// add meaning to reading (new or existing)
	//bool addMeaning(QString &reading, QString &meaning);
	
	// keep whole sentence (for testing, now)
	//bool keepSentence(QString &reading);
	
	// translate source with dictionary-lookup
	QString getText(QString &source);

	// "romanized" (latin-alphabet) representation
	// of kana-writing, mostly for debugging only..
	QString toRomaji(QString &source);
	
};

#endif // LANGUAGEDATA_H
