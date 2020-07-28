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

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
class QAction;
class QComboBox;
class QFontComboBox;
class QTextCharFormat;
class QMenu;
class QPrinter;
QT_END_NAMESPACE

/* MY ADD START */
#include <QTextCursor>
#include <QTextEdit>
#include "userscrolllist.h"
#include "userlist.h"
#include "user.h"
#include "logininfo.h"

class Symbol {
public:
    Symbol(QChar i, quint32 i1, int i2, std::vector<int>& vector, QTextCharFormat qtcf):
        c(i), siteid(i1), count(i2), fract(vector), format(qtcf){}
    Symbol(){

    }
    QChar c = 0;
    quint32 siteid = 0;
    int count = 0;
    std::vector<int> fract = {};
    QTextCharFormat format;         // ANCORA DA IMPLEMENTARE, probabilmente la property alignment sarà dura
};

class Message {
public:
    Message(int totA, int totR, quint32 fromUid, QList<Symbol>& symAdd, QList<Symbol>& symRem):
        totAdd(totA), totRem(totR), genFrom(fromUid), symToAdd(symAdd), symToRem(symRem){}
    Message(){}
    int totAdd = 0;
    int totRem = 0;
    quint32 genFrom = 0;

    QList<Symbol> symToAdd = {};
    QList<Symbol> symToRem = {};

};

class MyQTextEdit: public QTextEdit{
    Q_OBJECT
public:
    MyQTextEdit(QWidget* p);
    ~MyQTextEdit();
    void paintEvent(QPaintEvent *e);
    void localInsert(int i, QChar i1, QTextCharFormat f);
    void localErase(int i);
    int fractcmp(Symbol s1, Symbol s2);
    UserList* _userList = nullptr;
    UserScrollList* _userScrollList = nullptr;
    QWidget* container = nullptr;
    LoginInfo* loginInfo = nullptr;
private:

    quint32 _siteId = 0;
public:
    QMap<quint32, User> _users;
    QMap<quint32, QTextCursor> _cursors;
    quint32 getSiteId();
    QString to_string();
    void process(const Message &m);
private:
    std::vector<Symbol> _symbols;
    int _counter = 0;

public slots:
    /* changeBgcolor()
     * function to change user's bgcolor both retro and pro - actively
     * to be called by the client when selecting a color in the sidebar(?)
     * that is to be connect
     */
    void docuReady();
    void changeBgcolor(quint32, QColor);
    void updateProfile();
    void CatchChangeSignal(int pos, int rem, int add);      // move to private?
    void readMessage();
    void generateLink();


// last hot stuff
public:
    QTcpSocket* tcpSocket = nullptr;
    QDataStream in;                         // sarà da collegare al socket
    QDataStream out;                        // per ora non serve
    void addUser(const User &u);
    void removeUser(quint32);
    QStringList _files = {};
    void fakeNewFile();                // warning to be fixed!!!
    void fakeOpenFile();                // likewise
    void insertSymbols();
    std::vector<int> prefix(std::vector<int>, int, int);
    void adjustHeight();
    
signals:
    void readyToShow();

};

/* MY ADD END */

class TextEdit : public QMainWindow
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = 0);

    //bool load(const QString &f);

public slots:
    //void fileNew();
protected:
    void virtual closeEvent(QCloseEvent *e) override;

private slots:
//    void fileOpen();
//    bool fileSave();
//    bool fileSaveAs();
//    void filePrint();
//    void filePrintPreview();
    void filePrintPdf();

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    //void textAlign(QAction *a);

    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();

    void clipboardDataChanged();
    void about();
    //void printPreview(QPrinter *);

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    void setupViewActions();
    bool maybeSave();
    void setCurrentFileName(const QString &fileName);

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    //void alignmentChanged(Qt::Alignment a);

    QDockWidget *dock;
    QAction *actionDock;
    QAction *actionSave;
    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;
//    QAction *actionAlignLeft;
//    QAction *actionAlignCenter;
//    QAction *actionAlignRight;
//    QAction *actionAlignJustify;
    QAction *actionUndo;
    QAction *actionRedo;
#ifndef QT_NO_CLIPBOARD
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
#endif

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

    QToolBar *tb;
    QString fileName;
    MyQTextEdit *textEdit;                          // ONLY CHANGE

};

#endif // TEXTEDIT_H
