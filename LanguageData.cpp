/////////////////////////////////////////////
// Ilkka Prusi, 2011

#include "LanguageData.h"

// replaced by std::auto_ptr (VC++ 2010 has it..)
//#include "qautoptr.h"

//#include <QFile>
#include <QByteArray>
#include <QTextCodec>
#include <QDebug>

// for std::auto_ptr
#include <memory>

// TODO: handle standard library exceptions also..
#include <exception>

// simpler to debug..
#include "MemoryMappedFile.h"


//////////// protected methods

bool CLanguageData::Lookup(QString &source, QByteArray &output)
{
	try
	{
		// note: expect byte-size count, not character-count
		// -> expect 16-bit size of characters
		Dbt dbkey(source.data(), source.size()*sizeof(wchar_t)+2);
		Dbt dbvalue; // <- check handling

		// flags as zero, no change in data,
		// key should be unique anyway..
		int iRet = m_db.get(0, &dbkey, &dbvalue, 0);
		if (iRet == 0) 
		{
			// check, better way to set raw data to QString?
			//output = (const char*)dbvalue.get_data();
			
			// just set raw data as-is
			output.setRawData((char*)dbvalue.get_data(), dbvalue.get_size());
			return true;
		}
	}
	catch (DbException dbe)
	{
		qDebug() << "exception caught: " << source << " " << dbe.what();
	}
	return false;
}

bool CLanguageData::Store(QString &Key, void *pValue, int64_t iValueSize)
{
	try
	{
		// note: expect byte-size count, not character-count
		// -> expect 16-bit size of characters
		Dbt dbkey(Key.data(), Key.size()*sizeof(wchar_t)+2);
		Dbt dbvalue(pValue, iValueSize);
		
		// should have unique keys, don't overwrite existing
		int iRet = m_db.put(0, &dbkey, &dbvalue, DB_NOOVERWRITE);
		if (iRet == 0) 
		{
			return true;
		}
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
bool CLanguageData::appendDictionary(uchar *pData, int64_t iSize, QTextCodec *codec)
{
	// split: kanji [kana] /translations
	int64_t iPos = 0;
	while (iPos < iSize)
	{
		if (pData[iPos] == '/')
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
	int64_t iValueSize = (iSize-iPos);
	uchar *pValuePos = (uchar*)(pData+iPos);

	// if kanji+kana -> keep both,
	// otherwise has just kanji
	iPos = kanji.indexOf('[');
	if (iPos < 0)
	{
		// just kanji: keep it and translation
		return Store(kanji, pValuePos, iValueSize+1);
	}
	
	// keep same translation for both kanji and kana separately
	// for easier lookup later

	// keep kanji as-is with translation
	if (Store(kanji.left(iPos), pValuePos, iValueSize+1) == false)
	{
		return false;
	}

	// keep kana without [] brackets with translation
	int iEnd = kanji.indexOf(']');
	if (Store(kanji.mid(iPos+1, (iEnd-iPos)-1), pValuePos, iValueSize+1) == false)
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
		QString dbPath = appPath.append("/qkanadictionary.db");
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
	// simplifies cleanup..
	//std::auto_ptr<QFile> dict(new QFile(dictFile));
	std::auto_ptr<CMemoryMappedFile> dict(new CMemoryMappedFile());
	
	//if (dict->open(QIODevice::ReadOnly) == false)
	if (dict->Create(dictFile.utf16()) == false)
	{
		return false;
	}

	// use memorymapping, let OS do buffering..
	//qint64 iSize = dict->size();
	//uchar *pView = dict->map(0, iSize);
	int64_t iSize = dict->GetSize();
	uchar *pView = (uchar*)dict->GetView();
	if (pView == NULL)
	{
		return false;
	}
	
	// just assume it has EDICT-format with EUC-JP encoding..
	// -> convert to Unicode
	QTextCodec *codec = QTextCodec::codecForName("EUC-JP");
	
	int64_t iStart = 0;
	while (iStart < iSize)
	{
		uchar *pPos = (pView+iStart);
		int64_t iEnd = iStart+1;
		while (iEnd < iSize)
		{
			// line separator in this format for some reason..
			//
			// for some weird reason,
			// this gives access violation at roughly halfway in the file..
			//
			// wtf?
			//
			if (pPos[iEnd-1] == (uchar)0x20
			   && pPos[iEnd] == (uchar)0x5B)
			{
				break;
			}
			++iEnd;
		}

		// skip empty lines (if any..)
		if ((iEnd-iStart) > 1)
		{
			// parse line to dictionary entries
			if (appendDictionary(pPos, iEnd-iStart, codec) == false)
			{
				// continue with next
			}
		}
		
		iStart = iEnd;
	}
	return true;
}

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
//
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
