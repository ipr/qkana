#include "dictionary.h"
#include "ui_dictionary.h"

// use db for persistent storage
//#include <db_cxx.h>


dictionary::dictionary(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dictionary) //,
    //m_pDb(nullptr)
{
    ui->setupUi(this);
}

dictionary::~dictionary()
{
    delete ui;
}

/*
void dictionary::SetDictionary(Db *pDb)
{
    m_pDb = pDb;
}
*/

