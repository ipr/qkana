#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "LanguageData.h"

#include <QClipboard>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pClipBoard(nullptr),
    m_pLanguageData(nullptr)
{
    ui->setupUi(this);
	
	m_pLanguageData = new CLanguageData();

	//m_pClipBoard = new QClipboard(this);
	m_pClipBoard = QApplication::clipboard();
	if (m_pClipBoard != NULL)
	{
		connect(m_pClipBoard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(clipboardChanged(QClipboard::Mode)));
		ui->textBrowser->setText(m_pClipBoard->text());
	}
	
	m_pLanguageData->init(QApplication::applicationDirPath());
}

MainWindow::~MainWindow()
{
	m_pLanguageData->close();
	delete m_pLanguageData;
    delete ui;
}

void MainWindow::clipboardChanged(QClipboard::Mode mode)
{
	/*
	if (mode != QClipboard::Clipboard)
	{
		return;
	}
	*/
	
	ui->textBrowser->clear();
	ui->textBrowser->setText(m_pClipBoard->text());

	// TODO: show hiragana->romaji ?
	//QString text = m_pClipBoard->text(mode);
	//ui->textBrowser->setText(toRomaji(text));
}

/*
QString MainWindow::toRomaji(QString &text)
{
	return text;
}
*/
