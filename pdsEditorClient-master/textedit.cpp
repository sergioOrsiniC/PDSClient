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

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QTextCodec>
#include <QTextEdit>
#include <QStatusBar>
#include <QToolBar>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMimeData>
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
#include "client.h"
#include "profiledialog.h"

// my include
#include <QPainter>
#include <QtNetwork>
#include "userlist.h"
#include <QDockWidget>
#include "logininfo.h"


#ifdef Q_OS_MAC
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif


TextEdit::TextEdit(QWidget *parent)
    : QMainWindow(parent)
{
#ifdef Q_OS_OSX
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowTitle(QCoreApplication::applicationName());

    textEdit = new MyQTextEdit(this);

    dock = new QDockWidget(tr("Users"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(textEdit->container);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    connect(textEdit, &QTextEdit::currentCharFormatChanged,
            this, &TextEdit::currentCharFormatChanged);
    connect(textEdit, &QTextEdit::cursorPositionChanged,this, &TextEdit::cursorPositionChanged);

    setCentralWidget(textEdit);

    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    setupFileActions();
    setupEditActions();
    setupTextActions();
    setupViewActions();

    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);
    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());

    connect(textEdit->document(), &QTextDocument::modificationChanged,
            this, &QWidget::setWindowModified);
    connect(textEdit->document(), &QTextDocument::undoAvailable,
            actionUndo, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::redoAvailable,
            actionRedo, &QAction::setEnabled);

    setWindowModified(textEdit->document()->isModified());
    actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

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

}

void TextEdit::closeEvent(QCloseEvent *e)
{
//    if (maybeSave())
//        e->accept();
//    else
//        e->ignore();
    //maybe do a dialog or remove it
    e->accept();
}

void TextEdit::setupFileActions()
{
    QToolBar *tb = addToolBar(tr("File Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&File"));


#ifndef QT_NO_PRINTER

    const QIcon exportPdfIcon = QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png"));
    QAction *a = menu->addAction(exportPdfIcon, tr("&Export PDF..."), this, &TextEdit::filePrintPdf);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    tb->addAction(a);

    menu->addSeparator();
#endif

    a = menu->addAction(tr("&Quit"), this, &QWidget::close);
    a->setShortcut(Qt::CTRL + Qt::Key_Q);

    QAction *profile = menu->addAction(tr("&Update Profile"), textEdit, &MyQTextEdit::updateProfile);
    tb->addAction(profile);

    QAction *link = menu->addAction(tr("&Generate Link"), textEdit, &MyQTextEdit::generateLink);
    tb->addAction(link);
}

void TextEdit::setupViewActions()
{
    QToolBar *tb = addToolBar(tr("View Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&View"));
    menu->addAction(dock->toggleViewAction());
    tb->addAction(dock->toggleViewAction());

    menu->addSeparator();

}

void TextEdit::setupEditActions()
{
    QToolBar *tb = addToolBar(tr("Edit Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Edit"));

    const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(rsrcPath + "/editundo.png"));
    actionUndo = menu->addAction(undoIcon, tr("&Undo"), textEdit, &QTextEdit::undo);
    actionUndo->setShortcut(QKeySequence::Undo);
    tb->addAction(actionUndo);

    const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon(rsrcPath + "/editredo.png"));
    actionRedo = menu->addAction(redoIcon, tr("&Redo"), textEdit, &QTextEdit::redo);
    actionRedo->setPriority(QAction::LowPriority);
    actionRedo->setShortcut(QKeySequence::Redo);
    tb->addAction(actionRedo);
    menu->addSeparator();

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(rsrcPath + "/editcut.png"));
    actionCut = menu->addAction(cutIcon, tr("Cu&t"), textEdit, &QTextEdit::cut);
    actionCut->setPriority(QAction::LowPriority);
    actionCut->setShortcut(QKeySequence::Cut);
    tb->addAction(actionCut);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(rsrcPath + "/editcopy.png"));
    actionCopy = menu->addAction(copyIcon, tr("&Copy"), textEdit, &QTextEdit::copy);
    actionCopy->setPriority(QAction::LowPriority);
    actionCopy->setShortcut(QKeySequence::Copy);
    tb->addAction(actionCopy);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(rsrcPath + "/editpaste.png"));
    actionPaste = menu->addAction(pasteIcon, tr("&Paste"), textEdit, &QTextEdit::paste);
    actionPaste->setPriority(QAction::LowPriority);
    actionPaste->setShortcut(QKeySequence::Paste);
    tb->addAction(actionPaste);
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());
#endif
}

void TextEdit::setupTextActions()
{
    QToolBar *tb = addToolBar(tr("Format Actions"));
    QMenu *menu = menuBar()->addMenu(tr("F&ormat"));

    const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png"));
    actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
    actionTextItalic = menu->addAction(italicIcon, tr("&Italic"), this, &TextEdit::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
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

    comboStyle = new QComboBox(tb);
    tb->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");
    comboStyle->addItem("Heading 1");
    comboStyle->addItem("Heading 2");
    comboStyle->addItem("Heading 3");
    comboStyle->addItem("Heading 4");
    comboStyle->addItem("Heading 5");
    comboStyle->addItem("Heading 6");

    connect(comboStyle, QOverload<int>::of(&QComboBox::activated), this, &TextEdit::textStyle);

    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textFamily);

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    for (int size : standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textSize);
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

    setWindowTitle(tr("%1[*] - %2").arg(shownName, QCoreApplication::applicationName()));
    setWindowModified(false);
}

void TextEdit::filePrintPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
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

    QTextDocument* doc = new QTextDocument();
    QTextCursor pdf(doc);

    pdf.beginEditBlock();
    QTextCursor init(textEdit->document());
    for(int i=0; !init.atEnd(); i++){
        init.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        QTextCharFormat newFormat(init.charFormat());
        newFormat.clearBackground();
        pdf.insertText(init.selectedText(), newFormat);
        init.setPosition(i);
    }
    pdf.endEditBlock();


    doc->print(&printer);
    statusBar()->showMessage(tr("Exported \"%1\"")
                             .arg(QDir::toNativeSeparators(fileName)));

    delete doc;

//! [0]
#endif
}

void TextEdit::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
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

void TextEdit::textColor()
{
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void TextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void TextEdit::cursorPositionChanged()
{

    QTextList *list = textEdit->textCursor().currentList();
    if (list) {
        switch (list->format().style()) {
        case QTextListFormat::ListDisc:
            comboStyle->setCurrentIndex(1);
            break;
        case QTextListFormat::ListCircle:
            comboStyle->setCurrentIndex(2);
            break;
        case QTextListFormat::ListSquare:
            comboStyle->setCurrentIndex(3);
            break;
        case QTextListFormat::ListDecimal:
            comboStyle->setCurrentIndex(4);
            break;
        case QTextListFormat::ListLowerAlpha:
            comboStyle->setCurrentIndex(5);
            break;
        case QTextListFormat::ListUpperAlpha:
            comboStyle->setCurrentIndex(6);
            break;
        case QTextListFormat::ListLowerRoman:
            comboStyle->setCurrentIndex(7);
            break;
        case QTextListFormat::ListUpperRoman:
            comboStyle->setCurrentIndex(8);
            break;
        default:
            comboStyle->setCurrentIndex(-1);
            break;
        }
    } else {
        int headingLevel = textEdit->textCursor().blockFormat().headingLevel();
        comboStyle->setCurrentIndex(headingLevel ? headingLevel + 8 : 0);
    }
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
    QMessageBox::about(this, tr("About"), tr("This is our project"));
}

void TextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
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
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}


MyQTextEdit::MyQTextEdit(QWidget* p) : QTextEdit(p){

    container = new QWidget();
    auto layout = new QHBoxLayout;
    container->setLayout(layout);

    _userList = new UserList(nullptr, true, QBoxLayout::Direction::TopToBottom);
    _userScrollList = new UserScrollList(_userList);
    layout->addWidget(_userScrollList);

    tcpSocket = new QTcpSocket;
    loginInfo = new LoginInfo;
    Client client(this, tcpSocket, loginInfo);
    _siteId = client.exec();
    if(_siteId == 0) {
        exit(0);
    }

    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    connect(tcpSocket, &QIODevice::readyRead, this, &MyQTextEdit::readMessage);

    adjustHeight();

}

/* QDataStream operators */
QDataStream &operator<<(QDataStream& out, const Symbol& sen){
    return out << sen.c << sen.count << sen.format << sen.siteid <<QVector<int>(sen.fract.begin(), sen.fract.end());
}
QDataStream &operator>>(QDataStream& in, Symbol& rec){
    QVector<int> qvect;
    in >> rec.c >> rec.count >> rec.format >> rec.siteid >> qvect;
    rec.fract = std::vector<int>(qvect.begin(), qvect.end());
    return in;
}

QDataStream &operator<<(QDataStream& out, const Message& sen){
    return out << sen.totAdd << sen.totRem << sen.genFrom << sen.symToAdd << sen.symToRem;
}
QDataStream &operator>>(QDataStream& in, Message& rec){
    return in >> rec.totAdd >> rec.totRem >> rec.genFrom >> rec.symToAdd >> rec.symToRem;
}


QDataStream &operator<<(QDataStream& out, const User& sen){
    return out << sen.uid << sen.icon << sen.nick << sen.color << sen.startCursor;
}
QDataStream &operator>>(QDataStream& in, User& rec){
    return in >> rec.uid >> rec.icon >> rec.nick >> rec.color >> rec.startCursor;
}
template<class T>
QDataStream &operator<<(QDataStream& stream, const std::vector<T>& val){
    stream << static_cast<quint32>(val.size());
    for(auto& singleVal : val)
        stream << singleVal;
    return stream;
}

template<class T>
QDataStream &operator>>(QDataStream& stream, std::vector<T>& val){
    quint32 vecSize;
    val.clear();
    stream >> vecSize;
    val.reserve(vecSize);
    T tempVal;
    while(vecSize--){
        stream >> tempVal;
        val.push_back(tempVal);
    }
    return stream;
}
/* -------------------- */

void MyQTextEdit::CatchChangeSignal(int pos, int rem, int add){

    QList<Symbol> _add = {};
    QList<Symbol> _rem = {};

    std::vector<Symbol> _remNew;

    if(rem != 0){
        if(rem > (int) _symbols.size()){
            if(add)
                add -= rem - _symbols.size();
            rem = _symbols.size();
        }
        _rem = {_symbols.begin()+pos, _symbols.begin()+pos+rem};
        _symbols.erase(_symbols.begin()+pos, _symbols.begin()+pos+rem);

    }
    if(add != 0){

        auto supportCursor = QTextCursor(this->document());

        for(int i=0; i<add; i++){
            supportCursor.setPosition(pos+i);

            localInsert(pos+i, document()->characterAt(pos+i), supportCursor.charFormat());

            supportCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        }

        supportCursor.setPosition(pos);
        supportCursor.setPosition(pos+add, QTextCursor::KeepAnchor);

        QTextCharFormat newFormat;
        newFormat.setBackground(_users.value(_siteId).color);

        supportCursor.mergeCharFormat(newFormat);

        _add = {_symbols.begin()+pos, _symbols.begin()+pos+add};

    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    qDebug("sending a message");
    out << 'm';
    out << Message(add, rem, _siteId, _add, _rem);

    tcpSocket->write(block);

}

std::vector<int> MyQTextEdit::prefix(std::vector<int> id, int depth, int substitute)
{
    std::vector<int> idCopy = {};
    for (int cpt = 0; cpt <= depth; cpt++) {
        if (cpt < (int) id.size()) {
            idCopy.push_back(id.at(cpt));
        }
        else {
            idCopy.push_back(substitute);
        }
    }
    return idCopy;
}

void MyQTextEdit::localInsert(int index, QChar value, QTextCharFormat charFormat)
{
    std::vector<int> myfract = {};

    auto before = (int) _symbols.size() > index-1 ? _symbols.at(index-1).fract : std::vector<int>();
    auto after = (int) _symbols.size() > index ? _symbols.at(index).fract : std::vector<int>();

    int depth = 0;
    int interval = 0;

    while (interval < 1) {
        interval = prefix(after, depth, 1000).back() - prefix(before, depth, 0).back() - 1;
        depth++;
    }

    int step = std::min(2, interval);

    int addVal = QRandomGenerator::global()->bounded(0, step) + 1;
    myfract = prefix(before, depth-1, 0);

    myfract.back() += addVal;

    Symbol mysym{value, _siteId, _counter, myfract, charFormat};
    _symbols.insert(std::next(_symbols.begin(), index), mysym);

    _counter++;
}

void MyQTextEdit::localErase(int i) {

    _symbols.erase(_symbols.begin()+i);

}

void MyQTextEdit::paintEvent(QPaintEvent *event) {

    // To draw everything else
    QTextEdit::paintEvent(event);

    // for every foreign cursor, paint it and draw nick
    for(auto u: _users){
        if(u.uid != _siteId && _cursors.contains(u.uid)){
            // paint User's cursor with its color
            const QRect qRect = cursorRect(_cursors.find(u.uid).value());
            QPainter qPainter(viewport());
            qPainter.fillRect(qRect, u.color);

            // get size of Rect that can contain User's nick
            QRect nickRect = qPainter.boundingRect(qRect, 0, u.nick);

            // check if nickRect overflows the textedit view either upward or rightward
            // then move it accordingly to keep it inside
            if(viewport()->rect().top() < nickRect.top())
                nickRect.moveTop(qRect.bottom());
            if(viewport()->rect().right() < nickRect.right())
                nickRect.moveRight(qRect.right());

            // finally draw User's nick
            qPainter.drawText(nickRect, u.nick);

            // border around nick to better visibility
            QPen pen = qPainter.pen();
            pen.setStyle(Qt::SolidLine);
            qPainter.setPen(pen);
            qPainter.drawRect(nickRect.adjusted(0, 0, -pen.width(), -pen.width()));

        }
    }
}

void MyQTextEdit::addUser(const User &u) {

    // if User is not already present and it's not me then I have to create its Text Cursor
    // I should this evertime nonethelessif(!_users.contains(u.uid) && u.uid!=_siteId)
        _cursors.insert(u.uid, QTextCursor(this->document()));

    int pos = 1;
    for(auto ul: _userList->getItems()) {
        if(ul->userModel.uid == u.uid) {
            _userList->removeItem(ul);
        }
        pos++;
    }

    auto ul = new UserListItem(u);
    connect(ul, &UserListItem::colorSelected, this, &MyQTextEdit::changeBgcolor);

    _userList->addItem(ul, pos);


    // insert or replace new User
    _users.insert(u.uid, u);

}

void MyQTextEdit::removeUser(quint32 uid) {

    _cursors.remove(uid);

    for(auto ul: _userList->getItems()) {
        if(ul->userModel.uid == uid) {
            // just change its status
            ul->changeStatus(false);
        }
    }

}

int MyQTextEdit::fractcmp(Symbol s1, Symbol s2) {
    int digit = 0;
    int cmp;

    auto v1 = s1.fract;
    auto v2 = s2.fract;
    while ((int) v1.size() > digit && (int) v2.size() > digit)
    {
        cmp = v1.at(digit) - v2.at(digit);
        if(cmp!=0)
            return cmp;
        digit++;
    }

    // until now vectors are equal but one may continue
    if((int) v1.size() > digit && v1.at(digit) > 0)
        return 1;

    if((int) v2.size() > digit && v2.at(digit) == 0)
         return -1;

    cmp = s1.siteid - s2.siteid;
    if(cmp!=0)
        return cmp;

    cmp = s1.count - s2.count;

    return cmp;
}

void MyQTextEdit::process(const Message& m) {

    disconnect(document(), &QTextDocument::contentsChange,
            this, &MyQTextEdit::CatchChangeSignal);

    _cursors.find(m.genFrom)->beginEditBlock();

    int lowbound;
    int upbound;
    int index;
    Symbol curr;

    /* let's look for syms to erase */
    for(auto mi = m.symToRem.begin(); mi != m.symToRem.end(); mi++){

        lowbound = 0;
        upbound = _symbols.size();

        while(lowbound < upbound){
            index = (upbound+lowbound) /2;
            curr = _symbols.at(index);

            /* to check if it's the sym I'm looking for siteid & count are enough */
            if (curr.siteid == mi->siteid && curr.count == mi->count) {

                _symbols.erase(_symbols.begin()+index);

                _cursors.find(m.genFrom)->setPosition(index);
                _cursors.find(m.genFrom)->deleteChar();

                break;
            }

            if(fractcmp(curr, *mi) > 0)
                upbound = index;
            else
                lowbound = index +1;
        }

        //then it was already removed
    }

    /* let's look for syms to add */
    for(auto mi = m.symToAdd.begin(); mi != m.symToAdd.end(); mi++){

        lowbound = 0;
        upbound = _symbols.size();

        while(lowbound<upbound){

            index = (upbound+lowbound) /2;
            curr = _symbols.at(index);

            if(fractcmp(curr, *mi) > 0)
                upbound = index;
            else
                lowbound = index+1;
        }

        _symbols.insert(_symbols.begin() + upbound, *mi);

        // game changer
        QTextCharFormat newFormat(mi->format);
        newFormat.setBackground(_users.find(mi->siteid).value().color);

        _cursors.find(m.genFrom)->setPosition(upbound);
        _cursors.find(m.genFrom)->insertText(mi->c, newFormat);
    }


    _cursors.find(m.genFrom)->endEditBlock();

    adjustHeight();

    connect(document(), &QTextDocument::contentsChange,
            this, &MyQTextEdit::CatchChangeSignal);

}

void MyQTextEdit::adjustHeight()
{
    QTextDocument* doc = this->document();
    QTextBlock currentBlock = doc->firstBlock();

    // to add space between text lines
    while (currentBlock.isValid()) {
        QTextCursor cursor(currentBlock);
        QTextBlockFormat blockFormat = currentBlock.blockFormat();
        blockFormat.setLineHeight(200, QTextBlockFormat::ProportionalHeight);
        cursor.setBlockFormat(blockFormat);
        currentBlock = currentBlock.next();
    }
}

void MyQTextEdit::changeBgcolor(quint32 uid, QColor newColor){

    if(!_users.contains(uid)){
        // cant change color of Users I dont know of
        return;
    }
    User u = _users.value(uid);

    // proactive
    u.color = newColor;
    _users.insert(uid, u);

    // retroactive
    // redraw every char made by said user, got any better ideas?
    // could be parallelized just in case

    disconnect(document(), &QTextDocument::contentsChange,
            this, &MyQTextEdit::CatchChangeSignal);

    QTextCursor init(document()->begin());


    // ultima prova stupida
    init.beginEditBlock();
    document()->clear();
    for(auto it = _symbols.begin(); it!=_symbols.end(); it++){
        // you don't have the users yet, how can you find their color?
        QTextCharFormat newFormat(it->format);
        newFormat.setBackground(_users.find(it->siteid).value().color);
        init.insertText(it->c, newFormat);
    }
    init.endEditBlock();

    connect(document(), &QTextDocument::contentsChange,
            this, &MyQTextEdit::CatchChangeSignal);

}

void MyQTextEdit::updateProfile()
{
    // pointer warning
    User* toChange = new User(_users.find(_siteId).value());
    QString *uname = new QString ();
    QString *pw = new QString ();
    ProfileDialog dialog(this, toChange, uname, pw);
    qDebug() << "user nick before: " << toChange->nick;
    if(dialog.exec() == QDialog::Accepted) {
        qDebug() << "user nick after: " << toChange->nick;
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);

        qDebug("sending an updated profile");
        out << 'u';
        out << User(*toChange);
        QString output = (uname->isEmpty()) ? QString ("Unchanged") : QString(*uname);
        out << output;
        output = (pw->isEmpty()) ? QString ("Unchanged") : QString(*pw);
        out << output;

        tcpSocket->write(block);
    }
}


void MyQTextEdit::readMessage()
{

    Message msg;
    Symbol sym;
    User usr;

    quint32 uid;
    int op;

    do {
        in.startTransaction();
        in >> op;
        switch(op){
        case 'm':
            in >> msg;
            if(in.commitTransaction())
                process(msg);
            break;
        case 'u':
            in >> usr;
            if(in.commitTransaction())
                addUser(usr);
            break;
        case 'd':
            in >> uid;
            if(in.commitTransaction())
                removeUser(uid);
                //_users.remove(uid);
            break;
        case 't':
            in >> _symbols;
            qDebug() << "received #" << _symbols.size() << " symbols";
            if(in.commitTransaction()){
                insertSymbols();
                connect(document(), &QTextDocument::contentsChange,
                    this, &MyQTextEdit::CatchChangeSignal);
            }
            break;
        default:
            in.rollbackTransaction();
        }
        if(in.status() == QDataStream::Ok)
            qDebug() << "received '" << char(op) << "' message";
    } while(in.status() == QDataStream::Ok);

}

void MyQTextEdit::generateLink()
{
    QUrl link = QUrl("texteditor://" + loginInfo->host + "/" + loginInfo->file);
    link.setPort(loginInfo->port);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(link.toString());
    QMessageBox msgBox;
    msgBox.setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse);
    msgBox.setText("The link copied in your clipboard: " + link.toString());
    msgBox.exec();
}

void MyQTextEdit::insertSymbols(){
    QTextCursor init(document());

    init.beginEditBlock();
    for(auto it = _symbols.begin(); it!=_symbols.end(); it++){

        // you don't have the users yet, how can you find their color?
        QTextCharFormat newFormat(it->format);
        newFormat.setBackground(_users.find(it->siteid).value().color);
        init.insertText(it->c, newFormat);
    }
    init.endEditBlock();

    qDebug() << "document text received: " << document()->toPlainText();

}
