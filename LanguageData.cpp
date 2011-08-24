#include "LanguageData.h"

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
