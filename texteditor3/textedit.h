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


#ifndef TEXTEDIT_H
#define TEXTEDIT_H
#include "Utils/utils.h"
#include <QShortcut>
#include <BrowseWindow/browsewindow.h>
#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <Client/client.h>
#include <Highlighter/highlighter.h>
#include <LoginDialog/logindialog.h>
#include <RegisterDialog/registerdialog.h>
#include <QListWidgetItem>
#include <shared_editor/editor.h>
#include <QPainter>
#include <QAbstractTextDocumentLayout>

#include"NuovoFileRemotoWindoow/nuovofileremoto.h"
#include "shared_editor/eventbuffer.h"

class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QMenu;
class QPrinter;

class TextEdit : public QMainWindow
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = nullptr);

    bool load(const QString &f);


public slots:
    void fileNew();
    void loginDialog();
    void logoutDialog();
    void registerDialog();
    void connectDialog();


protected:
    void closeEvent(QCloseEvent *e) override;

private slots:

    /*************** Comandi per gestione cursore e caratteri **************/
    void salvaMappa();
    void spostaCursor(int& posX,int& posY,int& anchor,char& car,QString& user); //ATTENZIONE: questo oltre al cursore gestisce inserimento caratteri
    void changeCursor(QString& nomeUser, int pos);
    void cancellaAtCursor(int& posX,int& posY,int& anchor,char& car,QString& user);
    void deleteListSlot();
    void nuovoFile(QString& filename);
    void userListClicked(QListWidgetItem* item);
    void addMeSlot();
    void upCursor(QStringList &list);

    void nuovoStileSlot(QString& stile,QString& param);

    /*************** Comandi per gestione file remoti **************/
    void remoteBrows();
    void remoteAddFile();
    void remoteAddUser();
    void remoteRemoveFile();

    /***************************************************************/
    void fileOpen();
    bool fileSave();
    bool fileSaveAs();
    void filePrint();
    void filePrintPreview();
    void filePrintPdf();

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction *a);

    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();

    void clipboardDataChanged();
    void about();
    void printPreview(QPrinter *);

private:
    void setupUserActions();
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    bool maybeSave();
    void setCurrentFileName(const QString &fileName);

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void myColorChange(QString& nome);
    void alignmentChanged(Qt::Alignment a);
    void myTextAlign(QString& a);

    //Editor* editor;
    QTextCursor textCursor;
    QShortcut* sh;
    QListWidget *list;
    QMap<QString,QTextCursor*> mappaCursori; //mappa con nome utente e relativo cursore;
    QMap<QString,QLabel*> mappaEtichette; //mappa con nome utente e relativo cursore;
    QAction* registration;
    Highlighter* Evidenziatore;
    Client *client;
    QAction *actionSave;
    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;
    QAction *actionAlignLeft;
    QAction *actionAlignCenter;
    QAction *actionAlignRight;
    QAction *actionAlignJustify;
    QAction *actionUndo;
    QAction *actionRedo;
#ifndef QT_NO_CLIPBOARD
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
#endif

    // QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;
    bool remoteStile;
    QToolBar *tb;
    QString fileName;
    QTextEdit *textEdit;

 // prove
public:

    int cursorPos;

    void setVisibleFileActions(bool set);

private:
    void setupStatusBar();
    bool eventFilter(QObject *obj, QEvent *e) override;

    bool inserimento(int posCursor, QChar car, QTextCharFormat format);
    bool cancellamento(int posCursor, int key);

public slots:
    void goPaste();
    void clear();
    void refresh();
    void statusBarOutput(QString s);
    void setText(QChar c, QTextCharFormat f, int posCursor);   // inserisce in editor il carattere ricevuto
    void removeText(int posCursor);
    void loadEditor(QString str);
    void windowTitle(QString utente = "*", QString nomeFile = "*", QString docid = "*");

signals:
    void cursorChanged (int& posx, int& posy, int& anchor);
    void acceptLogout();
    void stileTesto(QString& stile,QString& param);
    void connectSig();
    void pasteSig(QString& clipboard);

};

#endif // TEXTEDIT_H
