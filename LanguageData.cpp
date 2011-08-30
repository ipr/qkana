/////////////////////////////////////////////
// Ilkka Prusi, 2011

#include "LanguageData.h"

// replaced by std::auto_ptr (VC++ 2010 has it..)
//#include "qautoptr.h"

#include <QFile>
#include <QByteArray>
#include <QTextCodec>
#include <QDebug>

// for std::auto_ptr
#include <memory>

// TODO: handle standard library exceptions also..
#include <exception>


//////////// protected methods

bool CLanguageData::Lookup(QString &source, QByteArray &output)
{
	try
	{
		// note: expect byte-size count, not character-count
		// -> expect 16-bit size of characters
		Dbt dbkey(source.data(), source.size()*2+1);
		//Dbt dbvalue(Value.data(), Value.size());
		Dbt dbvalue; // <- check handling

		qDebug() << "looking for: " << source;
		
		// flags as zero, no change in data,
		// key should be unique anyway..
		int iRet = m_db.get(0, &dbkey, &dbvalue, 0);
		if (iRet == 0) 
		{
			// check, better way to set raw data to QString?
			output = (const char*)dbvalue.get_data();
			return true;
		}
		
		qDebug() << "Key not found: " << source;
	}
	catch (DbException dbe)
	{
		qDebug() << "exception caught: " << source << " " << dbe.what();
	}
	return false;
}

bool CLanguageData::Store(QString &Key, QByteArray &Value)
{
	try
	{
		// note: expect byte-size count, not character-count
		// -> expect 16-bit size of characters
		Dbt dbkey(Key.data(), Key.size()*2+1);
		Dbt dbvalue(Value.data(), Value.size()+1);
		
		qDebug() << "storing: " << Key << " " << Value;
		
		// TODO: check flag DB_APPEND (append translation value)
		// or read existing first?
		//
		// should have unique keys
		int iRet = m_db.put(0, &dbkey, &dbvalue, DB_NOOVERWRITE);
		if (iRet == 0) 
		{
			return true;
		}
		
		qDebug() << "failed to store: " << Key << " " << Value << " result: " << iRet;
	}
	catch (DbException dbe)
	{
		qDebug() << "exception caught: " << Key << " " << dbe.what();
	}
	return false;
}


// split line and store to db:
// - line can be: kanji [kana] /translation1/translation2/..
// - or: kanji /translation/..
bool CLanguageData::appendDictionary(uchar *pData, qint64 iSize, QTextCodec *codec)
{
	// split kanji [kana] and translations
	qint64 iPos = 0;
	while (iPos < iSize)
	{
		uchar *puc = (pData+iPos);
		if (*puc == '/')
		{
			break;
		}
		++iPos;
	}
	
	if (iPos >= iSize)
	{
		// no translation in line?
		return false;
	}
	
	// use EUC-JP -> Unicode conversion for simplicity later..
	QString kanji = codec->toUnicode((char*)pData, iPos);

	// rest of line is translation(s)
	QByteArray translation((char*)(pData+iPos), iSize-iPos);

	// if kanji+kana -> keep both,
	// otherwise has just kanji
	iPos = kanji.indexOf('[');
	if (iPos < 0)
	{
		// just kanji: keep it and translation
		return Store(kanji, translation);
	}
	
	// keep same translation for both kanji and kana separately
	// for easier lookup later
	
	if (Store(kanji.left(iPos), translation) == false)
	{
		return false;
	}
	
	int iEnd = kanji.indexOf(']');
	if (Store(kanji.mid(iPos+1, (iEnd-iPos)-1), translation) == false)
	{
		return false;
	}
	
	return true;
}


//////////// public methods

CLanguageData::CLanguageData()
 : m_db(0,0)
 , m_bIsOpen(false)
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

	try
	{
		QString dbPath = appPath.append("qkanadictionary.db");
		int iRet = m_db.open(NULL, dbPath.toLocal8Bit(), 
		                     NULL, DB_BTREE, DB_CREATE, 0664);
		if (iRet == 0)
		{
			m_bIsOpen = true;
			return true;
		}
		
		// TODO: error to user?
		qDebug() << "failed to open/create db-file: " << dbPath << " result: " << iRet;
	}
	catch (DbException dbe)
	{
		qDebug() << "exception caught: " << dbe.what();
	}
	return false;
}

// note: closing can be done by destructor or user,
// extra safety necessary here..
bool CLanguageData::close()
{
	try
	{
		// flush to disk and close cleanly,
		// check that and ignore if nothing to do.
		if (m_bIsOpen == true)
		{
			m_db.sync(0);
			m_db.close(0);
			m_bIsOpen = false;
		}
		return true;
	}
	catch (DbException dbe)
	{
		qDebug() << "exception caught: " << dbe.what();
	}
	catch (...)
	{
		qDebug() << "unknown exception caught";
	}
	return false;
}

// append dictionary-data from specified file into database:
// this is just me being lazy, using EDICT-files to populate db..
//
bool CLanguageData::includeDictionary(QString &dictFile)
{
	// simplifies cleanup later..
	//qAutoPtr dict(new QFile(dictFile));
	std::auto_ptr<QFile> dict(new QFile(dictFile));
	
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
		uchar *pPos = (pView+iStart);
		qint64 iEnd = iStart+1;
		while (iEnd < iSize)
		{
			uchar *puc = (pPos+iEnd);
			if (*puc == '\n')
			{
				break;
			}
			++iEnd;
		}

		// skip empty lines (if any..)
		if ((iEnd-iStart) > 1)
		{
			// parse line to dictionary entries
			appendDictionary(pPos, iEnd-iStart, codec);
		}
			
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
		// TODO: skip punctuation or allow whole phrases?
		//
		// TODO: better way to store translations..
		//
		QByteArray tmp;
		int iTmpLen = source.length();
		while (iTmpLen > 0)
		{
			if (Lookup(source.left(iTmpLen), tmp) == true)
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
			// append translation to output
			output += tmp;
			// skip the part of source which we found translation for
			iStart += iTmpLen;
		}
	}
	
	return output;
}

// "romanized" (latin-alphabet) representation
// of kana-writing, mostly for debugging only..
QString CLanguageData::toRomaji(QString &source)
{
	QString output;
	int iPos = 0;
	
	while (iPos < source.count())
	{
		QChar ch = source[iPos];
		if (ch.digitValue() >= 0x3041 && ch.digitValue() <= 0x309F)
		{
			// hiragana character in unicode table
			
			/*
			if (ch == "\u3063")
			{
				// small tsu -> 
				// append consonant of following kana?
				// (or vowel of last kana?)
			}
			*/
		}
		else if (ch.digitValue() >= 0x30A0 && ch.digitValue() <= 0x30FF)
		{
			// katakana character in unicode table
		}
		else
		{
			// maybe kanji or other -> output as-is
			output += ch;
		}
		++iPos;
	}
	return output;
}
