/////////////////////////////////////////////
// Ilkka Prusi, 2011

#ifndef LANGUAGEDATA_H
#define LANGUAGEDATA_H

#include <QString>

// use db for persistent storage
#include <db_cxx.h>


class QByteArray;
class QTextCodec;

class CLanguageData
{
protected:
	Db m_db;
	bool m_bIsOpen;

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
