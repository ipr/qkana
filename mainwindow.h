/////////////////////////////////////////////
// Ilkka Prusi, 2011

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QClipboard>

//class QClipboard;
class CLanguageData;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void clipboardChanged(QClipboard::Mode mode);

private slots:
	void on_actionDictionary_triggered();
	
private:
    Ui::MainWindow *ui;
	QClipboard *m_pClipBoard;
	CLanguageData *m_pLanguageData;
};

#endif // MAINWINDOW_H
