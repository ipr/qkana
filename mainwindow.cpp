/////////////////////////////////////////////
// Ilkka Prusi, 2011


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "LanguageData.h"

#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pClipBoard(nullptr),
    m_pLanguageData(nullptr),
    m_bDisableTranslate(false)
{
    ui->setupUi(this);

	// initialize before trying to translate
	m_pLanguageData = new CLanguageData();
	m_pLanguageData->init(QApplication::applicationDirPath());

	// capture from clipboard directly..
	m_pClipBoard = QApplication::clipboard();
	if (m_pClipBoard != NULL)
	{
		connect(m_pClipBoard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(clipboardChanged(QClipboard::Mode)));
		ui->textBrowser->setText(m_pClipBoard->text());
	}
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
	
	QString text = m_pClipBoard->text();
	
	// show original capture
	ui->textBrowser->setText(text);

	// allow disabling automated translation..
	if (m_bDisableTranslate == false)
	{
		// use dictionary..
		ui->textBrowser->setText(m_pLanguageData->getText(text));
		
		// TODO: show hiragana->romaji ?
		//ui->textBrowser->setText(m_pLanguageData->toRomaji(text));
	}
}

void MainWindow::on_actionDictionary_triggered()
{
	QString szFile = QFileDialog::getOpenFileName(this, tr("Open file"));
	if (szFile != NULL)
	{
		if (m_pLanguageData->includeDictionary(szFile) == false)
		{
			ui->statusBar->showMessage("Error including dictionary");
		}
		else
		{
			ui->statusBar->showMessage("Dictionary included");
		}
	}
}

void MainWindow::on_actionDisable_translation_triggered(bool checked)
{
	m_bDisableTranslate = checked;
}
