/////////////////////////////////////////////
// Ilkka Prusi, 2011

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QClipboard>

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
	
	void on_actionDisable_translation_triggered(bool checked);
	
    void on_actionView_translation_database_triggered();
    
private:
    Ui::MainWindow *ui;
	QClipboard *m_pClipBoard;
	CLanguageData *m_pLanguageData;
	
	// disable automatic translations
	bool m_bDisableTranslate;
};

#endif // MAINWINDOW_H
