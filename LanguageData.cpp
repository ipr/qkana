#include "LanguageData.h"
#include "qautoptr.h"

#include <QFile>
#include <QByteArray>


//////////// protected methods

// read entry from given table
/*
bool CLanguageData::Read(CAbstractTable *pTable, Dbt &Key, Dbt &Value)
{
 // test reading each key-value pair with cursor
 Dbc *dbcp = NULL;
 db.cursor(NULL, &dbcp, 0);
 Dbt selkey;
 Dbt seldata;

 while (dbcp->get(&selkey, &seldata, DB_NEXT) == 0) 
 {
	 char *key_string = (char *)selkey.get_data();
	 char *data_string = (char *)seldata.get_data();
	 std::cout << key_string << " : " << data_string << std::endl;
 }
 dbcp->close();
}
*/

// write given entry to given table
/*
bool CLanguageData::Write(CAbstractTable *pTable, Dbt &Key, Dbt &Value)
{
 // test adding some data
 char *pKey = "key";
 char *pVal = "value";
 Dbt inskey(pKey, strlen(pKey)+1);
 Dbt insdata(pVal, strlen(pVal)+1);

 iRet = db.put(0, &inskey, &insdata, DB_NOOVERWRITE);
 if (iRet == DB_KEYEXIST) 
 {
	 std::cerr << "Key exists" << std::endl;
 }
}
*/

bool CLanguageData::Lookup(QString &source, QString &output)
{
	Dbt dbkey(source.data(), source.size()+1);
    //Dbt dbvalue(Value.data(), Value.size());
    Dbt dbvalue; // <- check handling

	// check flags ->
	int iRet = m_db.get(0, &dbkey, &dbvalue, 0);
	
	// check error return ->
    if (iRet == DB_KEYEMPTY) 
	{
		return false;
	}
	return true;
}

bool CLanguageData::Write(QByteArray &Key, QByteArray &Value)
{
	Dbt dbkey(Key.data(), Key.size()+1);
    Dbt dbvalue(Value.data(), Value.size());
	
	// TODO: check flag DB_APPEND (append translation value)
	int iRet = m_db.put(0, &dbkey, &dbvalue, DB_NOOVERWRITE);
    if (iRet == DB_KEYEXIST) 
    {
	    //qDebug() << "Key exists: " << Key.data();
		return false;
    }
	return true;
}


// split line and store to db:
// - line can be: kanji [kana] /translation1/translation2/..
// - or: kanji /translation/..
bool CLanguageData::appendDictionary(QByteArray &line)
{
	// split kanji [kana] and translations
	int iSplit = line.indexOf('/');
	QByteArray kanji = line.left(iSplit);
	QByteArray translation = line.right(line.size()-iSplit);

	// if kanji+kana -> keep both,
	// otherwise has just kanji
	iSplit = kanji.indexOf('[');
	if (iSplit < 0)
	{
		// just kanji: keep it and translation
		return Write(kanji, translation);
	}
	
	// keep same translation for both kanji and kana separately
	// for easier lookup later
	
	if (Write(kanji.left(iSplit), translation) == false)
	{
		return false;
	}
	
	int iEnd = kanji.indexOf(']');
	if (Write(kanji.mid(iSplit, iSplit+iEnd), translation) == false)
	{
		return false;
	}
	
	return true;
}


//////////// public methods

CLanguageData::CLanguageData()
 : m_db(0,0)
{
}

CLanguageData::~CLanguageData()
{
	close();
}

// TODO: path of app..?
bool CLanguageData::init(QString &appPath)
{
	// TODO: separate db-file for "reading" and "meaning" data?
	// (no tables in berkeley db and need many-to-many relations..)
	//

	int iRet = m_db.open(NULL, appPath.append("qclip.db"), NULL, DB_BTREE, DB_CREATE, 0664);
	if (iRet != 0)
	{
		// some more error handling?
		return false;
	}
	return true;
}

bool CLanguageData::close()
{
	m_db.close(0);
	return true;
}

// append dictionary-data from specified file into database:
// this is just me being lazy, using EDICT-files to populate db..
//
bool CLanguageData::includeDictionary(QString &dictFile)
{
	// simplifies cleanup later..
	qAutoPtr dict(new QFile(dictFile));
	
	if (dict->open(QIODevice::ReadOnly) == false)
	{
		return false;
	}

	// use memorymapping, let os do buffering..
	qint64 iSize = dict->size();
	uchar *pView = dict->map(0, iSize);
	if (pView == NULL)
	{
		return false;
	}

	// just assume it has EDICT-format with EUC-JP encoding..
	// -> convert to Unicode
	QTextCodec *codec = QTextCodec::codecForName("EUC-JP");
	
	qint64 iStart = 0;
	while (iStart < iSize)
	{
		qint64 iEnd = iStart+1;
		while (pView != '\n'
		       && iEnd < iSize)
		{
			++iEnd;
		}
		
		// get line: kanji [kana] /translation/..
		// note: use EUC-JP -> Unicode conversion for simplicity later..
		//
		QString line = codec->toUnicode(pView+iStart, iEnd-iStart);
		appendDictionary(QByteArray(line));
		
		iStart = iEnd;
	}

	return true;
}


// each "reading" in kanji+kana
// may have multiple "meanings"
// (terms not exactly correct but helps..)
/*
QStringList CLanguageData::getMeaning(QString &reading)
{
	// 
}
*/

// add new meaning to given reading in kanji+kana
// (terms not exactly correct but helps..)
/*
bool CLanguageData::addMeaning(QString &reading, QString &meaning)
{
}
*/

// keep whole sentence (for testing, now)
/*
bool CLanguageData::keepSentence(QString &reading)
{
}
*/

// get rough translation by dictionary lookup,
// don't even attempt grammar/syntax handling..
//
QString CLanguageData::getText(QString &source)
{
	QString output;
	
	// quick&dirty way:
	// just lookup translations
	// in descending order by length..
	
	int iStart = 0;
	while (iStart < source.length())
	{
		// this will need optimization later..
		//
		int iTmpLen = source.length();
		while (iTmpLen > 0)
		{
			if (Lookup(source.left(iTmpLen), output) == true)
			{
				break;
			}
		}
		
		// no translation
		if (iTmpLen == 0)
		{
			// output character as-is..
			output += source.left(1);
			iStart += 1;
		}
		else
		{
			// skip the part which we found translation for
			iStart += iTmpLen;
		}
	}
	
	return output;
}

