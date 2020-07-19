/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QShortcut>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QEvent>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QLabel>
#include <QTextCodec>
#include <QTextEdit>
#include <QStatusBar>
#include <QToolBar>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QHBoxLayout>
#include <QListWidget>

#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif
#include <QPrinter>
#if QT_CONFIG(printpreviewdialog)
#include <QPrintPreviewDialog>
#endif
#endif
#endif

#include "textedit.h"
#include "Client/client.h"

#ifdef Q_OS_MAC
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif

/**
 * @brief TextEdit::TextEdit
 * @param parent
 */
TextEdit::TextEdit(QWidget *parent)
    : QMainWindow(parent)
{
#ifdef Q_OS_OSX
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowTitle(QCoreApplication::applicationName());

    textEdit = new QTextEdit(this);
    connect(textEdit, &QTextEdit::currentCharFormatChanged,
            this, &TextEdit::currentCharFormatChanged);
    connect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &TextEdit::cursorPositionChanged);

    list=new QListWidget();
    QWidget *centrale=new QWidget();
    QHBoxLayout *miolayout=new QHBoxLayout(centrale);
    QVBoxLayout *miolayoutVert=new QVBoxLayout();
    miolayout->addWidget(textEdit,1);
    setCentralWidget(centrale);
    //    miolayout->addWidget(list);

    miolayoutVert->addWidget(list);
//    miolayoutVert->addWidget(&progress);
    miolayout->addLayout(miolayoutVert);

    remoteStile=false;

    connect(this->list, SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(userListClicked(QListWidgetItem*)));

    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    setupUserActions();
    setupFileActions();
    setupEditActions();
    setupTextActions();

    setVisibleFileActions(false);
    setVisibleEditorActions(false);

    {
        QMenu *helpMenu = menuBar()->addMenu(tr("Help"));
        helpMenu->addAction(tr("About"), this, &TextEdit::about);
        helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    }

    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);
    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());

    connect(textEdit->document(), &QTextDocument::modificationChanged,
            actionSave, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::modificationChanged,
            this, &QWidget::setWindowModified);

    setWindowModified(textEdit->document()->isModified());
    actionSave->setEnabled(textEdit->document()->isModified());


#ifndef QT_NO_CLIPBOARD
    actionCut->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, actionCut, &QAction::setEnabled);
    actionCopy->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &TextEdit::clipboardDataChanged);
#endif

    textEdit->setFocus();
    setCurrentFileName(QString());

#ifdef Q_OS_MACOS
    // Use dark text on light background on macOS, also in dark mode.
    QPalette pal = textEdit->palette();
    pal.setColor(QPalette::Base, QColor(Qt::white));
    pal.setColor(QPalette::Text, QColor(Qt::black));
    textEdit->setPalette(pal);
#endif

    this->textEdit->installEventFilter(this);       // importante

    this->textCursor = this->textEdit->textCursor();

    this->client = new Client(this);
    connect(this->client, &Client::s_warning, this, &TextEdit::warningDialog, Qt::DirectConnection);

    client->connectToHost();
    connect(this, SIGNAL(cursorChanged(int&,int&)), this->client, SLOT(handleMyCursorChange(int&,int&)));
    connect(this->client, SIGNAL(spostaCursSignal(int&,int&,char&,QString&)), this, SLOT(spostaCursor(int&,int&,char&,QString&)));
    connect(this, SIGNAL(pasteSig(QString&)),this->client, SLOT(pasteSlot(QString&)));
    connect(this->client, &Client::s_setText, this, &TextEdit::setText, Qt::ConnectionType::DirectConnection);          /* aggiunge testo all'editor */
    connect(this->client, &Client::s_removeText, this, &TextEdit::removeText, Qt::ConnectionType::DirectConnection);    /* rimuove testo dall'editor */
    connect(this->client, &Client::s_loadEditor, this, &TextEdit::loadEditor, Qt::ConnectionType::DirectConnection);
    connect(this->client, &Client::s_clearEditor, this, &TextEdit::clear, Qt::ConnectionType::DirectConnection);        /* pulisce l'editor */
    connect(this->client, &Client::s_upCursor, this, &TextEdit::upCursor, Qt::DirectConnection);                        /* modifica lista cursori */
    connect(this->client, &Client::s_changeCursor, this, &TextEdit::changeCursor, Qt::DirectConnection);
    connect(this->client, &Client::s_changeTitle, this, &TextEdit::windowTitle, Qt::DirectConnection);
    connect(this->client, &Client::s_brows, this, &TextEdit::remoteBrows, Qt::DirectConnection);

    connect(this->client, &Client::s_setVisbleFileActions, this, &TextEdit::setVisibleFileActions, Qt::DirectConnection);
    connect(this->client, &Client::s_setVisbleEditorActions, this, &TextEdit::setVisibleEditorActions, Qt::DirectConnection);

    connect(this->client, &Client::s_logOut, this, &TextEdit::acceptLogout, Qt::DirectConnection);

    setupStatusBar();
}

/*******************************************************************
 *  Prove
 * */
void TextEdit::setupStatusBar(){
    statusBar()->showMessage(tr("Status message: bentornato"), 2000);   // 2 secondi
    connect(client,&Client::toStatusBar, this, &TextEdit::statusBarOutput);
}


bool TextEdit::eventFilter(QObject* obj, QEvent* event) {
    //qDebug()<<QApplication::clipboard()->mimeData()->text();
    if (obj == this->textEdit) {
        /*
         * modifica shortcut di copia e incolla
         */
        if (event->type() == QEvent::ShortcutOverride)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->modifiers().testFlag(Qt::ControlModifier) == true)
            {
                return true;
            }

        }

        if (!this->client->isLogged()) {
//            this->statusBar()->showMessage("Utente non loggato", 2000);
            return false;   // eseguo normalmente
        }

        if (this->client->remoteFile == nullptr) {
            return false;
        }

        QTextCursor s = textEdit->textCursor();
        int poss = s.position();
        int anchor = s.anchor();
        if (event->type() == QEvent::KeyPress) {    // verifica key di interesse

            QKeyEvent* e = static_cast<QKeyEvent*>(event);

            if (e->isAutoRepeat()) {
                return true;       // salta le azioni ripetute
            }

            if (e->text() == "") { return false; } // SALTA I PULSANTI CHE NON INSERISCONO CARATTERI

            if (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete) {    // 16777219 , 16777223
                // PULSANTI DI CANCELLAZIONE
                if (s.anchor() > s.position())
                {
                    while (s.anchor() > s.position())
                    {
                        this->cancellamento(anchor, Qt::Key_Backspace);
                        anchor--;
                    }
                }

                else if (s.anchor() < s.position())
                {
                    while (s.anchor() < s.position())
                    {
                        this->cancellamento(poss, Qt::Key_Backspace);
                        poss--;
                    }
                }

                else if (s.anchor() == s.position())
                {
                    this->cancellamento(poss, e->key());

                }

                return true;

            }
            else {
                // PULSANTI DI INSERIMENTO
                if (!e->text().front().isPrint() && e->text().front() != '\xd') { return false; }

                QChar c = e->text().front();        // carattere da inserire
                QTextCharFormat format = textEdit->textCursor().charFormat();   // formato del carattere

                if (s.hasSelection()){
                    if (s.anchor() > s.position()){
                        while (s.anchor() > s.position()){
                            this->cancellamento(anchor, Qt::Key_Backspace);
                            anchor--;
                        }
                    } else if (s.anchor() < s.position()) {
                        while (s.anchor() < s.position()){
                            this->cancellamento(poss, Qt::Key_Backspace);
                            poss--;
                        }
                    }
                }

                if(!this->inserimento(poss, c, format)){
                    return false;
                }

                return true;
            }
        }
        return false;
    }

    return false;
}

bool TextEdit::inserimento(int posCursor, QChar car, QTextCharFormat format)
{
    QTextCursor s = this->textEdit->textCursor();

    if(this->client->inserimentoLocale(posCursor, car, format)){
        qDebug()<<"ERRORE DI INSERIMENTO LOCALE (TEXTEDIT LINE 375)\n";
        return true;
    }

    return false;
}

bool TextEdit::cancellamento(int posCursor, int key)
{
    // gestisco limiti dell'editor tra delete e backspace
    if (key == Qt::Key_Backspace && posCursor > 0){
        posCursor--;
    } else if (key == Qt::Key_Delete){
        // Qt::key_delete
    }

    this->client->cancellamentoLocale(posCursor);
/*
    QTextCursor s = this->textEdit->textCursor();
    s.setPosition(posCursor);
    s.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveMode::KeepAnchor);
    QChar car = s.selectedText().front();

    double index = this->client->remoteFile->getLocalIndexDelete(posCursor);


    if (index < 0){
        this->statusBar()->showMessage("impossibile cancellare in locale", 1000);
    } else if (this->client->remoteDelete(car, index)) {// INSERIMENTO REMOTO, manda comando a server
        // true se server risp OK

        this->statusBar()->showMessage("At position: " + QString::number(posCursor) + " Text delete: " + car, 1000);

    } else {
        this->statusBar()->showMessage("impossibile inserire da remoto", 1000);
    }
*/
    return true;
}

/**************************************************************/

void TextEdit::closeEvent(QCloseEvent *e)
{
    e->accept();
    /* richiede se si vuole chiudere o no
     * if (maybeSave())        e->accept();    else        e->ignore(); */
}

void TextEdit::setupUserActions()
{
    QToolBar *tb = addToolBar(tr("User Actions"));
    QMenu *menu = menuBar()->addMenu(tr("User"));

    const QIcon loginIcon = QIcon::fromTheme("document-new", QIcon(rsrcPath + "/login.png"));
    actionLogin = menu->addAction(loginIcon, tr("&Login"), this, &TextEdit::LoginDialog);
  //  login->setShortcut(QKeySequence::ZoomIn); tr("Ctrl+a")
    tb->addAction(actionLogin);
    //login->setCheckable(true);

    const QIcon registerIcon = QIcon::fromTheme("document-new", QIcon(rsrcPath + "/registration.png"));
    actionRegistration = menu->addAction(registerIcon, tr("&Register"), this, &TextEdit::RegisterDialog);
   // registration->setShortcut(QKeySequence::ZoomIn); //tr("Ctrl+a")
    tb->addAction(actionRegistration);
    //login->setCheckable(true);

    const QIcon logoutIcon = QIcon::fromTheme("document-new", QIcon(rsrcPath + "/logout.png"));
    actionLoguot = menu->addAction(logoutIcon, tr("&Logout"), this, &TextEdit::LogoutDialog);
  //  login->setShortcut(QKeySequence::ZoomIn); //tr("Ctrl+a")
    tb->addAction(actionLoguot);
    //login->setCheckable(true);
}

void TextEdit::setupFileActions()
{
    QToolBar *tb = addToolBar(tr("File Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&File"));
    QAction *a;

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(rsrcPath + "/filenew.png"));
    actionNewRemote = menu->addAction(newIcon,  tr("&New remote file"), this, &TextEdit::fileNew);
    tb->addAction(actionNewRemote);

    const QIcon remoteBrowse = QIcon::fromTheme("document-open", QIcon(rsrcPath + "/remoteBrows.png"));
    actionBrowsRemote = menu->addAction(remoteBrowse, tr("&Remote Browse..."), this, &TextEdit::remoteBrows);
    tb->addAction(actionBrowsRemote);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(rsrcPath + "/filesave.png"));
    actionSave = menu->addAction(saveIcon, tr("&Save"), this, &TextEdit::fileSave);
    actionSave->setShortcut(QKeySequence::Save);
    actionSave->setEnabled(false);
    tb->addAction(actionSave);

    const QIcon addUser = QIcon::fromTheme("addUser", QIcon(rsrcPath + "/addUserIcon.png"));
    actionManageUser = menu->addAction(addUser, tr("&add User to File..."), this, &TextEdit::remoteAddUser);
    tb->addAction(actionManageUser);

    tb->addSeparator();

#ifndef QT_NO_PRINTER
    const QIcon printIcon = QIcon::fromTheme("document-print", QIcon(rsrcPath + "/fileprint.png"));
    actionPrint = menu->addAction(printIcon, tr("&Print..."), this, &TextEdit::filePrint);
    actionPrint->setPriority(QAction::LowPriority);
    actionPrint->setShortcut(QKeySequence::Print);
    tb->addAction(actionPrint);

    const QIcon filePrintIcon = QIcon::fromTheme("fileprint", QIcon(rsrcPath + "/fileprint.png"));
    menu->addAction(filePrintIcon, tr("Print Preview..."), this, &TextEdit::filePrintPreview);

    const QIcon exportPdfIcon = QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png"));
    actionExportPDF = menu->addAction(exportPdfIcon, tr("&Export PDF..."), this, &TextEdit::filePrintPdf);
    actionExportPDF->setPriority(QAction::LowPriority);
    actionExportPDF->setShortcut(Qt::CTRL + Qt::Key_D);
    tb->addAction(actionExportPDF);

    menu->addSeparator();
#endif

    const QIcon exitFileIcon = QIcon::fromTheme("document-new", QIcon(rsrcPath + "/fileexit.png"));
    actionExitFile = menu->addAction(exitFileIcon,  tr("&Exit from this file"), this, &TextEdit::fileExit); // slot di connessione
    tb->addAction(actionExitFile);

    menu->addSeparator();

    a = menu->addAction(tr("Save &As..."), this, &TextEdit::fileSaveAs);
    a->setPriority(QAction::LowPriority);
    menu->addSeparator();

    a = menu->addAction(tr("&Quit"), this, &QWidget::close);
    a->setShortcut(Qt::CTRL + Qt::Key_Q);
}

void TextEdit::setupEditActions()
{
    QToolBar *tb = addToolBar("Edit");
    QMenu *menu = menuBar()->addMenu(tr("Edit"));

    //menu->addSeparator();

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(rsrcPath + "/editcut.png"));
    actionCut = menu->addAction(cutIcon, tr("Cu&t"), this, &TextEdit::goCut, QKeySequence::Cut);
    tb->addAction(actionCut);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(rsrcPath + "/editcopy.png"));
    actionCopy = menu->addAction(copyIcon, tr("&Copy"), this, &TextEdit::goCopy, QKeySequence::Copy);
    tb->addAction(actionCopy);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(rsrcPath + "/editpaste.png"));
    actionPaste = menu->addAction(pasteIcon, tr("&Paste"), this, &TextEdit::goPaste, QKeySequence::Paste);
    tb->addAction(actionPaste);

    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());

#endif
}

void TextEdit::goPaste(){
    QString s(QApplication::clipboard()->mimeData()->text());
    QTextCursor cur = textEdit->textCursor();
    int pos = cur.position(), anc = cur.anchor();
    cur.setPosition(pos, QTextCursor::MoveAnchor);
    QTextCharFormat currentFormat = cur.charFormat();

    this->cursorEnable(false);
    int i=0;
    if(client->isLogged()){
        for(QChar c : s){
            client->inserimentoLocale(pos+i, c, currentFormat);
            i++;
        }
    }
    this->cursorEnable(true);
}

void TextEdit::goCut()
{
    this->textEdit->copy();
    QString s(QApplication::clipboard()->mimeData()->text());
    int size = s.size();
    QTextCursor c = textEdit->textCursor();
    int pos = c.position();
    int anc = c.anchor();
    pos = std::min(pos, anc);
    for (int i = 0; i<size; i++) {
        this->client->cancellamentoLocale(pos);
    }

}

void TextEdit::goCopy()
{
    this->textEdit->copy();
}

void TextEdit::clear()
{
    this->textEdit->clear();
}

/**
 * @brief TextEdit::refresh
 * scrive sull'editor dopo cambiamenti in symMap
 */
void TextEdit::refresh()
{

}

void TextEdit::statusBarOutput(QString s)
{
    this->statusBar()->showMessage(s, 2000);
}

void TextEdit::setText(QChar c, QTextCharFormat f, int posCursor)
{
    QTextCursor s = textEdit->textCursor();
    s.setPosition(posCursor, QTextCursor::MoveAnchor);

    s.insertText(c, f);
}

void TextEdit::removeText(int posCursor)
{
    QTextCursor s = textEdit->textCursor();
    s.setPosition(posCursor, QTextCursor::MoveAnchor);
    s.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveMode::KeepAnchor);
    s.removeSelectedText();
}

void TextEdit::loadEditor(QString str)
{
    this->clear();
    this->textEdit->setText(str);
}

void TextEdit::windowTitle(QString utente = "", QString nomeFile = "", QString docid = "")
{
    setWindowTitle(tr("%1 @ %2 (%3) - %4").arg(utente, nomeFile, docid, QCoreApplication::applicationName()));
    setWindowModified(false);
}

void TextEdit::setupTextActions()
{
    QToolBar *tb = addToolBar(tr("Format Actions"));
    QMenu *menu = menuBar()->addMenu(tr("F&ormat"));


    const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png"));
    actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
    actionTextItalic = menu->addAction(italicIcon, tr("&Italic"), this, &TextEdit::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    tb->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    const QIcon underlineIcon = QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png"));
    actionTextUnderline = menu->addAction(underlineIcon, tr("&Underline"), this, &TextEdit::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    tb->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);

    menu->addSeparator();

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = menu->addAction(pix, tr("&Color..."), this, &TextEdit::textColor);
    tb->addAction(actionTextColor);

    tb = addToolBar(tr("Format Actions"));
    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(tb);

    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textFamily);

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textSize);
}

bool TextEdit::load(const QString &f)
{
    if (!QFile::exists(f))
        return false;
    QFile file(f);
    if (!file.open(QFile::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    QTextCodec *codec = Qt::codecForHtml(data);
    QString str = codec->toUnicode(data);
    if (Qt::mightBeRichText(str)) {
        textEdit->setHtml(str);
    } else {
        str = QString::fromLocal8Bit(data);
        textEdit->setPlainText(str);
    }

    setCurrentFileName(f);
    return true;
}

void TextEdit::setCurrentFileName(const QString &fileName)
{
    this->fileName = fileName;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1 - %2").arg(shownName, QCoreApplication::applicationName()));
    setWindowModified(false);
}

void TextEdit::LoginDialog()
{

    class LoginDialog* loginDialog = new class LoginDialog( this );
    connect( loginDialog, SIGNAL (acceptLogin(QString&,QString&)), this->client, SLOT (handleLogin(QString&,QString&)) );
    loginDialog->exec();
}

void TextEdit::ConnectDialog()
{
    connect(this, SIGNAL(connectSig()), this->client, SLOT(connectSlot()));
    emit connectSig();

}

void TextEdit::warningDialog(QString str)
{
    QMessageBox msgBox;
    msgBox.setText(str);
    msgBox.exec();
}

void TextEdit::LogoutDialog()
{
    const QMessageBox::StandardButton ret = QMessageBox::warning(this,
                   "Disconnessione in corso",
                   "Si sta per effettuare la disconnessione dell'utente: \" " +
                        this->client->getUsername() + " \".\nContinuare con la disconnesione?",
                   QMessageBox::Yes | QMessageBox::No,
                   QMessageBox::No);

    switch (ret){
    case QMessageBox::Yes : // invio logout
        this->client->handleLogout();
        break;
    default: // esco
        break;
    }
    connect( this, SIGNAL (acceptLogoff()), this->client, SLOT (handleLogoff()) );
}

void TextEdit::RegisterDialog()
{
    class RegisterDialog* registerdialog = new class RegisterDialog( this );
    connect( registerdialog, SIGNAL (acceptRegistration(QString&,QString&)), this->client, SLOT (handleRegistration(QString&,QString&)) );
    registerdialog->exec();
}


/**
 * @brief TextEdit::fileNews
 */
void TextEdit::fileNew()
{
    client->handleNuovoFile();
}

void TextEdit::fileExit()
{
    if (this->client->remoteFile == nullptr){
        return;
    }
    qDebug() << "sono in " << Q_FUNC_INFO;
    const QMessageBox::StandardButton ret = QMessageBox::warning(this,
                   "Esci dal documento corrente",
                   "Si sta per uscire dal file corrente: \" " +
                        this->client->remoteFile->getNomeFile() + " \".\nContinuare?",
                   QMessageBox::Yes | QMessageBox::No,
                   QMessageBox::No);

    switch (ret){
    case QMessageBox::Yes : // invio logout
        this->client->handleFileExit();
        this->list->clear();
        this->mappaEtichette.clear();
        this->mappaCursori.clear();
        break;
    default: // esco
        break;
    }
}

void TextEdit::fileOpen()
{
    QFileDialog fileDialog(this, tr("Open File..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setMimeTypeFilters(QStringList() << "text/html" << "text/plain" );
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString fn = fileDialog.selectedFiles().first();
    if (load(fn))
        statusBar()->showMessage(tr("Opened \"%1\"").arg(QDir::toNativeSeparators(fn)));

    else
        statusBar()->showMessage(tr("Could not open \"%1\"").arg(QDir::toNativeSeparators(fn)));
}

bool TextEdit::fileSave()
{
    if (fileName.isEmpty())
        return fileSaveAs();
    if (fileName.startsWith(QStringLiteral(":/")))
        return fileSaveAs();

    QTextDocumentWriter writer(fileName);
    bool success = writer.write(textEdit->document());
    if (success) {
        textEdit->document()->setModified(false);
        statusBar()->showMessage(tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName)));
    } else {
        statusBar()->showMessage(tr("Could not write to file \"%1\"")
                                 .arg(QDir::toNativeSeparators(fileName)));
    }
    return success;
}

void TextEdit::remoteBrows()
{
    if (this->client->isLogged()){
        QMap<QString, QString> comando;
        comando.insert(CMD, BROWSE);
        this->client->sendMsg(comando);
    }
}

/**
 * @brief TextEdit::remoteAddFile
 * invia messaggio di add file al server\
 */
void TextEdit::remoteAddFile()
{
    qDebug() << "Sono in TextEdit::remoteAddFile";
    if (this->client->isLogged()){
        client->handleNuovoFile();
    }

}

void TextEdit::remoteAddUser()
{
    if (this->client->isLogged()){
        this->client->handleAddUser();
    }
}

void TextEdit::remoteRemoveFile()
{
    // TODO
}


bool TextEdit::fileSaveAs()
{
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
    mimeTypes << "application/vnd.oasis.opendocument.text" << "text/html" << "text/plain";
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.setDefaultSuffix("odt");
    if (fileDialog.exec() != QDialog::Accepted)
        return false;
    const QString fn = fileDialog.selectedFiles().first();
    setCurrentFileName(fn);
    return fileSave();
}

void TextEdit::filePrint()
{
#if QT_CONFIG(printdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textEdit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        textEdit->print(&printer);
    delete dlg;
#endif
}

void TextEdit::filePrintPreview()
{
#if QT_CONFIG(printpreviewdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &TextEdit::printPreview);
    preview.exec();
#endif
}

void TextEdit::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    textEdit->print(printer);
#endif
}

void TextEdit::filePrintPdf()
{
#ifndef QT_NO_PRINTER
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    textEdit->document()->print(&printer);
    statusBar()->showMessage(tr("Exported \"%1\"")
                             .arg(QDir::toNativeSeparators(fileName)));
#endif
}

void TextEdit::textBold()
{
    QTextCharFormat fmt;

    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);

    QString s="bold";
    QString p="";
    if(!remoteStile)emit stileTesto(s,p);
}

void TextEdit::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);

    QString s="underline";
    QString p="";
    if(!remoteStile)emit stileTesto(s,p);
}

void TextEdit::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);

    QString s="italic";
    QString p="";
    if(!remoteStile)emit stileTesto(s,p);
}

void TextEdit::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);

    //TODO da controllare!!!
    QString p="font";
    if(!remoteStile)emit stileTesto(p,const_cast<QString&>(f));
}

void TextEdit::textSize(const QString &p)
{
    qreal pointSize = p.toDouble();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
    QString l="size";
    if(!remoteStile)emit stileTesto(l,const_cast<QString&>(p));
}

void TextEdit::textStyle(int styleIndex)
{
    QTextCursor cursor = textEdit->textCursor();
    QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;

    switch (styleIndex) {
    case 1:
        style = QTextListFormat::ListDisc;
        break;
    case 2:
        style = QTextListFormat::ListCircle;
        break;
    case 3:
        style = QTextListFormat::ListSquare;
        break;
    case 4:
        style = QTextListFormat::ListDecimal;
        break;
    case 5:
        style = QTextListFormat::ListLowerAlpha;
        break;
    case 6:
        style = QTextListFormat::ListUpperAlpha;
        break;
    case 7:
        style = QTextListFormat::ListLowerRoman;
        break;
    case 8:
        style = QTextListFormat::ListUpperRoman;
        break;
    default:
        break;
    }

    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();

    if (style == QTextListFormat::ListStyleUndefined) {
        blockFmt.setObjectIndex(-1);
        int headingLevel = styleIndex >= 9 ? styleIndex - 9 + 1 : 0; // H1 to H6, or Standard
        blockFmt.setHeadingLevel(headingLevel);
        cursor.setBlockFormat(blockFmt);

        int sizeAdjustment = headingLevel ? 4 - headingLevel : 0; // H1 to H6: +3 to -2
        QTextCharFormat fmt;
        fmt.setFontWeight(headingLevel ? QFont::Bold : QFont::Normal);
        fmt.setProperty(QTextFormat::FontSizeAdjustment, sizeAdjustment);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.mergeCharFormat(fmt);
        textEdit->mergeCurrentCharFormat(fmt);
    } else {
        QTextListFormat listFmt;
        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
}
void TextEdit::myColorChange(QString& nome){
    QColor col(nome);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);


    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);

    QString s=col.name();
    QString p="color";
}

void TextEdit::textColor()
{
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);

    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);


    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);

    QString s=col.name();
    QString p="color";
    emit stileTesto(p,s);
}

void TextEdit::myTextAlign(QString& a)
{
    if (a == "left"){
        textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    } else if (a == "center"){
        textEdit->setAlignment(Qt::AlignHCenter);
    } else if (a == "right"){
        textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    } else if (a == "giustificato"){
        textEdit->setAlignment(Qt::AlignJustify);
    }
}

void TextEdit::setVisibleFileActions(bool set)
{
    if ( actionNewRemote != nullptr &&
         actionBrowsRemote != nullptr &&
         actionSave != nullptr &&
         actionManageUser != nullptr &&
         actionPrint != nullptr &&
         actionExportPDF != nullptr &&
         actionExitFile != nullptr )
    {
        actionNewRemote->setEnabled(set);
        actionBrowsRemote->setEnabled(set);
        actionSave->setEnabled(set);
        actionManageUser->setEnabled(set);
        actionPrint->setEnabled(set);
        actionExportPDF->setEnabled(set);
        actionExitFile->setEnabled(set);
    } else {
        this->statusBarOutput("azione = nullptr");
    }

}

void TextEdit::setVisibleEditorActions(bool set)
{
    this->textEdit->setEnabled(set);    // accende/spegne l'editor

    actionTextBold->setEnabled(set);
    actionTextItalic->setEnabled(set);
    actionTextUnderline->setEnabled(set);
    actionTextColor->setEnabled(set);
    // vale anche per copia incolla
    actionCut->setEnabled(set);
    actionCopy->setEnabled(set);
    actionPaste->setEnabled(set);
}

void TextEdit::acceptLogout()
{
    this->textEdit->clear();
    this->setVisibleEditorActions(false);
    this->setVisibleFileActions(false);
    this->mappaEtichette.clear();
    this->mappaCursori.clear();

}

void TextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void TextEdit::cursorPositionChanged()
{    /*
     *  Posizione del cursore nel testo
     */
    QTextCursor cursore = textEdit->textCursor();
    QString str = QString::number(cursore.position());
    int posy=textEdit->textCursor().blockNumber(); /**********************QUESTO è L'INDICE DI RIGA**********************/
    int posx=textEdit->textCursor().positionInBlock();/******************QUESTO è L'INDICE ALL'INTERNO DELLA RIGA************/
    int anchor=textEdit->textCursor().anchor();
    //qDebug()<<"cursor at:"<<posx<<posy<<"\n"; DEBUG

    /****************** QUA INSERISCO ME STESSO NELLA LISTA DELLE PERSONE ONLINE E DEI CURSORI ************/
    if(mappaCursori.contains(client->getUsername())){

        // muovo il cursore
        QTextCursor *s1= mappaCursori.find(client->getUsername()).value();

        s1->movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
        s1->movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,posx);
        s1->movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,posy);
        int poss = s1->position();
        s1->movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
        s1->movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,anchor);

        if(anchor<=poss){
            s1->movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor, poss-anchor);
        }
        else{
            s1->movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor, anchor-poss);
        }

    }
    else{//qDebug()<<"probabilmente sono offline";
    }

    if(mappaEtichette.contains(client->getUsername())){
        QLabel* l=mappaEtichette.find(client->getUsername()).value();
        QRect s=textEdit->cursorRect();
        l->move(s.left(), s.bottom());
    }
    int pos=this->textEdit->textCursor().position();
    emit cursorChanged(pos,anchor);
    //statusBar()->showMessage(str, 0);

}

void TextEdit::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());

#endif
}

void TextEdit::about()
{
    QMessageBox::about(this, tr("About"), tr("This example demonstrates Qt's "
        "rich text editing facilities in action, providing an example "
        "document for you to experiment with."));
}

void TextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    int pos=cursor.position();
    int anchor=cursor.anchor();
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);

    this->cursorEnable(false);

    QTextCharFormat form = cursor.charFormat();
    if(pos>anchor){
        for(int i=anchor; i<pos; i++){
            this->client->inserimentoLocale(i, QChar(), form);
        }
    } else {
        for(int i=pos; i<anchor; i++){
            this->client->inserimentoLocale(i, QChar(), form);
        }
    }

    this->cursorEnable(true);
}

void TextEdit::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void TextEdit::colorChanged(const QColor &c)
{
    QPixmap pix(20, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void TextEdit::spostaCursor(int& pos,int& anchor,char& car ,QString& user){
    //ATTENZIONE!!! oltre a gestire il cursore gestisce anche l'inserimento
    qDebug()<< "sono in " << Q_FUNC_INFO;
    qDebug()<<pos<<car<<user;

    if(!mappaCursori.contains(user)){
        //Se non ho mai visto questo user lo metto nella lista di user

        this->list->addItem(user);
        //Se non ho mai visto questo user creo un nuovo Cursore

        mappaCursori.insert(user,new QTextCursor(textEdit->document()));

        //muovo il cursore

        QTextCursor *s= mappaCursori.find(user).value();

        disconnect(textEdit, &QTextEdit::cursorPositionChanged,
                this, &TextEdit::cursorPositionChanged);

        if(anchor>pos){
            s->movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
            s->movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,anchor);
            s->movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,anchor-pos);
        }

        else if(anchor<=pos){
            s->movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
            s->movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,pos);
            s->movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,pos-anchor);
        }


        QLabel *lab=new QLabel(user, textEdit);
        mappaEtichette.insert(user,lab);
        auto cc=textEdit->textCursor();
        textEdit->setTextCursor(* mappaCursori.value(user));
        QRect r=textEdit->cursorRect();
        lab->move(r.left(), r.bottom());
        lab->setFont(QFont("Arial",6,12,true));
        lab->setStyleSheet("QLabel { background-color : rgba(255, 0, 0,64); color : blue; }");
        lab->show();
        textEdit->setTextCursor(cc);
        qDebug()<<"------------------";
        qDebug()<<mappaCursori.find(user).value()->position();
        qDebug()<<"------------------";

        connect(textEdit, &QTextEdit::cursorPositionChanged,
                this, &TextEdit::cursorPositionChanged);
    }

    else if(mappaCursori.contains(user)){

        disconnect(textEdit, &QTextEdit::cursorPositionChanged,this, &TextEdit::cursorPositionChanged);

        // muovo il cursore
        QTextCursor *s1= mappaCursori.find(user).value();

        if(anchor>pos){
            s1->movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
            s1->movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,anchor);
            s1->movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,anchor-pos);
        }

        else if(anchor<=pos){
            s1->movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
            s1->movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,pos);
            s1->movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,pos-anchor);
        }

        QLabel *lab=mappaEtichette.find(user).value();
        mappaEtichette.insert(user,lab);
        auto cc=textEdit->textCursor();
        textEdit->textCursor().setPosition(mappaCursori.find(user).value()->position());
        textEdit->setTextCursor(* mappaCursori.value(user));
        QRect r=textEdit->cursorRect();
        lab->move(r.left(), r.bottom());
        lab->show();
        textEdit->setTextCursor(cc);
        qDebug()<<"------------------";
        qDebug()<<textEdit->cursorRect().x();
        qDebug()<<user;
        qDebug()<<mappaCursori.find(user).value()->position();
        qDebug()<<"------------------";

        connect(textEdit, &QTextEdit::cursorPositionChanged,
                this, &TextEdit::cursorPositionChanged);
    }

    QList<QListWidgetItem *> tmp = list->findItems(user, Qt::MatchExactly);
    if (tmp.size() != 1) {
        return;
    }
    QListWidgetItem *pItem = tmp.first();
    if (pItem->checkState() == Qt::Unchecked){
        mappaEtichette.value(pItem->text())->setVisible(false);
    } else {
        mappaEtichette.value(pItem->text())->setVisible(true);
    }

    return ;
}

void TextEdit::changeCursor(QString &nomeUser, int pos)
{
    disconnect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &TextEdit::cursorPositionChanged);

    auto oldCursor = this->textEdit->textCursor();
    this->textEdit->textCursor().setPosition(pos);
    this->mappaCursori.value(nomeUser)->setPosition(pos);
    QRect r = this->textEdit->rect();
    this->mappaEtichette.value(nomeUser)->move(r.left(), r.bottom());
    this->textEdit->textCursor() = oldCursor;

    connect(textEdit, &QTextEdit::cursorPositionChanged,
                this, &TextEdit::cursorPositionChanged);
}

void TextEdit::deleteListSlot(){
   //svuota la lista perchè non sono più connesso;
   while(this->list->count()>0){
       delete this->list->takeItem(0);
   }
}

void TextEdit::userListClicked(QListWidgetItem* item)
{
    //rendo visibile il cursore appena cliccato
    qDebug() << "sono in " << Q_FUNC_INFO;
    qDebug() << item->text();
    QTextCursor oldCursor = this->textEdit->textCursor();
    textEdit->setFocus(); //rimette il focus al widget che fa text editor altrimenti scompare il cursore

    disconnect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &TextEdit::cursorPositionChanged);

    if(mappaCursori.contains(item->text())){
        if (item->checkState() == Qt::Unchecked){
            // item->setCheckState(Qt::Unchecked);
            mappaEtichette.value(item->text())->setVisible(false);
        } else {
            // item->setCheckState(Qt::Checked);
            textEdit->setTextCursor(* mappaCursori.value(item->text()));
            mappaEtichette.value(item->text())->setVisible(true);
            textEdit->textCursor() = oldCursor;
        }

    } else {
        qDebug()<<"errore utente non trovato";
    }
    connect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &TextEdit::cursorPositionChanged);
}

void TextEdit::cancellaAtCursor(int& posX,int& posY,int& anchor,char& car ,QString& user){

    if(!mappaCursori.contains(user)){
        //Se non ho mai visto questo user lo metto nella lista di user
        this->list->addItem(user);
        //Se non ho mai visto questo user creo un nuovo Cursore
        mappaCursori.insert(user,new QTextCursor(textEdit->document()));
        //muovo il cursore
        QTextCursor *s= mappaCursori.find(user).value();
        s->movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
        s->movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,posX);
        s->movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,posY);
        int poss=s->position();
        s->movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
        s->movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,anchor);

        if(anchor<=poss){
            s->movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,poss-anchor);
        } else{
            s->movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,anchor-poss);
        }

        if(car=='\x3'){
            s->deletePreviousChar();
        } else if(car=='\x7'){
            s->deleteChar();
        }
    } else{
        // muovo il cursore
        QTextCursor *s1= mappaCursori.find(user).value();
        s1->movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
        s1->movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,posX);
        s1->movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,posY);
        int poss=s1->position();
        s1->movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
        s1->movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,anchor);

        if(anchor<=poss){
            s1->movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,poss-anchor);
        } else{
            s1->movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,anchor-poss);
        }

        if(car=='\x3'){
            s1->deletePreviousChar();
        } else if(car=='\x7'){
            s1->deleteChar();
        }
    }
}

void TextEdit::nuovoFile(QString& filename)
{
    if (load(filename)){
        statusBar()->showMessage(tr("Opened \"%1\"").arg(QDir::toNativeSeparators(filename)));
    } else {
        statusBar()->showMessage(tr("NOT Opened \"%1\"").arg(QDir::toNativeSeparators(filename)));
        QMessageBox Messaggio;
        Messaggio.critical(nullptr,"OpenFile ERROR",filename);
        Messaggio.setFixedSize(500,200);
    }
    return;
}

void TextEdit::addMeSlot()
{
    return;
}

void TextEdit::upCursor(QStringList &list)
{
    qDebug() << "sono in " << Q_FUNC_INFO;
    QStringList old = mappaEtichette.keys();
    for (QString nome : list){
        if (old.contains(nome)){
            old.removeOne(nome);
        } else {
            if (!mappaCursori.contains(nome)){
                QTextCursor* s = new QTextCursor(textEdit->document());
                mappaCursori.insert(nome, s);
            }
            if (!mappaEtichette.contains(nome)){
                QLabel *lbl = new QLabel(nome, textEdit);
                QRect r = textEdit->cursorRect();
                lbl->move(r.left(), r.bottom());
                lbl->setFont(QFont("Arial",8,14,true));
                lbl->setStyleSheet("QLabel { background-color : rgba(255, 0, 0, 64); color : blue; }");
                lbl->setVisible(false);
                mappaEtichette.insert(nome, lbl);
            }
            QListWidgetItem *item = new QListWidgetItem(nome);
            item->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::CheckState::Unchecked);
            this->list->addItem(item);
        }
    }
    // rimuovi vecchi
    bool find = false;
    for (QString exNome : old){
        mappaCursori.remove(exNome);
        mappaEtichette.value(exNome)->setVisible(false);
        mappaEtichette.remove(exNome);
        int t = this->list->count();
        for (int i = 0; i < t && find == false; i++){
            QListWidgetItem *item = this->list->item(i);
            QString str = item->text();
            if ( exNome == item->text() ){
                this->list->takeItem(i);
                find = true;
            }
        }
    }
}

void TextEdit::cursorEnable(bool set)
{
    if (set){
        connect(textEdit, &QTextEdit::cursorPositionChanged,
                this, &TextEdit::cursorPositionChanged);
    } else {
        disconnect(textEdit, &QTextEdit::cursorPositionChanged,
                this, &TextEdit::cursorPositionChanged);
    }
}

void TextEdit::nuovoStileSlot(QString& stile,QString& param){
    remoteStile=true;
    if(stile=="bold"){
        this->actionTextBold->toggle();
        this->textBold();
    } else if(stile=="underline"){
        this->actionTextUnderline->toggle();
        this->textUnderline();
    } else if(stile=="italic"){
        this->actionTextItalic->toggle();
        this->textItalic();
    } else if(stile=="font"){
        this->textFamily(param);
    } else if(stile=="size"){
        this->textSize(param);
    } else if(stile=="color"){
        this->myColorChange(param);
    } else if(stile=="align"){
        this->myTextAlign(param);
    }

    remoteStile=false;
    return;
}

void TextEdit::salvaMappa(){
    QFile *f= new QFile("mappa");
    f->open(QIODevice::ReadWrite | QIODevice::Text);

    QByteArray s;
    QDataStream out(&s, QIODevice::WriteOnly);
    QDataStream in(&s, QIODevice::ReadOnly);

    out << this->client->remoteFile->symVec;

    QMap<double,Symbol> deserial;
    in >> deserial;                  // DEBUG

    qDebug()<<deserial.keys();
    f->write(s);

    f->close();
}
