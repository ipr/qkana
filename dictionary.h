#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QDialog>

namespace Ui {
    class dictionary;
}

// fwd. decl.
//class Db;

class dictionary : public QDialog
{
    Q_OBJECT

public:
    explicit dictionary(QWidget *parent = 0);
    ~dictionary();
    
    //void SetDictionary(Db *pDb);

private:
    Ui::dictionary *ui;
    //Db *m_pDb;
};

#endif // DICTIONARY_H
