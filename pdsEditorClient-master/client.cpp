/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QtWidgets>
#include <QtNetwork>

#include "client.h"
#include "textedit.h"

Client::Client(QWidget *parent, QTcpSocket* parentSocket, LoginInfo* info)
    : QDialog(parent)
    , hostCombo(new QComboBox)
    , portLineEdit(new QLineEdit)
    , nextButton(new QPushButton(tr("Next")))
    , linkLineEdit(new QLineEdit)
    , openLinkButton(new QPushButton(tr("Open link")))
    , tcpSocket(parentSocket)
    , loginCombo(new QComboBox)
    , userLineEdit(new QLineEdit)
    , pwdLineEdit(new QLineEdit)
    , nickLineEdit(new QLineEdit)
    , openCombo(new QComboBox)
    , fileCombo(new QComboBox)
    , loginInfo(info)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    hostCombo->setEditable(true);
    // find out name of this machine
    QString name = QHostInfo::localHostName();
    if (!name.isEmpty()) {
        hostCombo->addItem(name);
        QString domain = QHostInfo::localDomainName();
        if (!domain.isEmpty())
            hostCombo->addItem(name + QChar('.') + domain);
    }
    if (name != QLatin1String("localhost"))
        hostCombo->addItem(QString("localhost"));
    // find out IP addresses of this machine
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // add non-localhost addresses
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (!ipAddressesList.at(i).isLoopback())
            hostCombo->addItem(ipAddressesList.at(i).toString());
    }
    // add localhost addresses
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i).isLoopback())
            hostCombo->addItem(ipAddressesList.at(i).toString());
    }

    portLineEdit->setValidator(new QIntValidator(1, 65535, this));

    auto userLabel = new QLabel(tr("User name:"));
    userLabel->setBuddy(userLineEdit);
    auto pwdLabel = new QLabel(tr("User password:"));
    pwdLabel->setBuddy(pwdLineEdit);
    auto nickLabel = new QLabel(tr("User nickName:"));
    nickLabel->setBuddy(nickLineEdit);

    auto hostLabel = new QLabel(tr("&Server name:"));
    hostLabel->setBuddy(hostCombo);
    auto portLabel = new QLabel(tr("&Server port:"));
    portLabel->setBuddy(portLineEdit);

    auto fileLabel = new QLabel(tr("&File name:"));
    fileLabel->setBuddy(fileCombo);

    auto linkLabel = new QLabel(tr("&Invitation link:"));
    linkLabel->setBuddy(linkLineEdit);


    statusLabel = new QLabel(tr("This examples requires that you run the "
                                "TextEdit Server as well."));

    nextButton->setDefault(true);
    nextButton->setEnabled(true);

    loginCombo->addItems({"login", "signup"});
    loginCombo->setEnabled(false);
    userLineEdit->setEnabled(false);
    pwdLineEdit->setEnabled(false);
    nickLineEdit->setEnabled(false);

    openCombo->addItems({"open", "new"});
    openCombo->setEnabled(false);
    fileCombo->setEnabled(false);

    auto quitButton = new QPushButton(tr("Quit"));

    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(openLinkButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(nextButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    connect(openLinkButton, &QAbstractButton::clicked,
            this, &Client::openLink);

    connect(hostCombo, &QComboBox::editTextChanged,
            this, &Client::enableConnectToServerButton);
    connect(portLineEdit, &QLineEdit::textChanged,
            this, &Client::enableConnectToServerButton);

    connect(nextButton, &QAbstractButton::clicked,
            this, &Client::connectToServer);

    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);

    connect(tcpSocket, &QTcpSocket::connected, this, &Client::enableLogin);

    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &Client::displayError);

    QGridLayout *mainLayout = nullptr;
    if (QGuiApplication::styleHints()->showIsFullScreen() || QGuiApplication::styleHints()->showIsMaximized()) {
        auto outerVerticalLayout = new QVBoxLayout(this);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
        auto outerHorizontalLayout = new QHBoxLayout;
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        auto groupBox = new QGroupBox(QGuiApplication::applicationDisplayName());
        mainLayout = new QGridLayout(groupBox);
        outerHorizontalLayout->addWidget(groupBox);
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        outerVerticalLayout->addLayout(outerHorizontalLayout);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
    } else {
        mainLayout = new QGridLayout(this);
    }
    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostCombo, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);

    mainLayout->addWidget(loginCombo, 2, 0, 1, 2);

    mainLayout->addWidget(userLabel, 3, 0);
    mainLayout->addWidget(userLineEdit, 3, 1);
    mainLayout->addWidget(pwdLabel, 4, 0);
    mainLayout->addWidget(pwdLineEdit, 4, 1);
    mainLayout->addWidget(nickLabel, 5, 0);
    mainLayout->addWidget(nickLineEdit, 5, 1);

    mainLayout->addWidget(openCombo, 6, 0, 1, 2);

    mainLayout->addWidget(fileLabel, 7, 0);
    mainLayout->addWidget(fileCombo, 7, 1);

    mainLayout->addWidget(statusLabel, 8, 0, 1, 2);

    mainLayout->addWidget(linkLabel, 9, 0);
    mainLayout->addWidget(linkLineEdit, 9, 1);

    mainLayout->addWidget(buttonBox, 10, 0, 1, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    portLineEdit->setFocus();

}

void Client::connectToServer()
{
    nextButton->setEnabled(false);
    tcpSocket->abort();
    tcpSocket->connectToHost(hostCombo->currentText(),
                             portLineEdit->text().toInt());
}

void Client::enableLogin()
{

    loginCombo->setEnabled(true);
    userLineEdit->setEnabled(true);
    pwdLineEdit->setEnabled(true);

    connect(userLineEdit, &QLineEdit::textChanged,
            this, &Client::enableLoginButton);
    connect(pwdLineEdit, &QLineEdit::textChanged,
            this, &Client::enableLoginButton);

    statusLabel->setText("connected");
    disconnect(nextButton, &QAbstractButton::clicked,
            this, &Client::connectToServer);

    nextButton->setEnabled(false);
    connect(nextButton, &QAbstractButton::clicked,
            this, &Client::loginTry);

    // TODO: rivedere combo e magari rimuovere signform
    // TODO: rimuovere nick????
    connect(loginCombo, &QComboBox::currentTextChanged, this, &Client::signForm);
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }

    nextButton->setEnabled(true);
}

void Client::enableConnectToServerButton()
{
    nextButton->setEnabled(!hostCombo->currentText().isEmpty() &&
                                 !portLineEdit->text().isEmpty());
}

void Client::enableLoginButton()
{
    nextButton->setEnabled(!userLineEdit->text().isEmpty() &&
                                 !pwdLineEdit->text().isEmpty());
}


void Client::enableOpenButton()
{
    nextButton->setEnabled(!fileCombo->currentText().isEmpty());
}

void Client::sessionOpened()
{

    statusLabel->setText(tr("This examples requires that you run the "
                            "TextEdit Server as well."));

    enableConnectToServerButton();
}

void Client::loginResponse()
{
    int op;
    uid = 0;

    in.startTransaction();
    in >> op;
    if(!(in.commitTransaction()))
        return;
    if(!(op == 'l'|| op == 's'))
        return;
    in.startTransaction();
    in >> uid;
    if(in.commitTransaction()){
        if(uid != 0 ){
            // SUCCESS
            if(in.atEnd())
                return;
            in.startTransaction();
            in >> _files;
            if(!in.commitTransaction())
                return;
            openCombo->setEnabled(true);


            fileCombo->addItems(_files);
            fileCombo->setEnabled(true);
            fileCombo->setEditable(true);

            nextButton->setEnabled(!_files.empty());

            connect(fileCombo, &QComboBox::editTextChanged,
                    this, &Client::enableOpenButton);

            disconnect(nextButton, &QAbstractButton::clicked,
                    this, &Client::loginTry);

            connect(nextButton, &QAbstractButton::clicked,
                    this, &Client::fileTry);

        }
        else {
            qDebug() << "loginread failed: " << " op was" << (char) op;
            QMessageBox::information(this, tr("TextEdit") ,
                                     tr("Utente Non trovato"));
            //FAIL
        }
    }
}

void Client::fileTry()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    int op = 'z'; // UNinitialized value

    bool present = _files.contains(fileCombo->currentText());
    if(openCombo->currentText()=="open"){
        if(!present){
            QMessageBox::information(this, tr("TextEdit"),
                                     tr("No file found"));
            return;
        }
        op = 'o';
    }
    else if(openCombo->currentText()=="new"){
        if(present){
            QMessageBox::information(this, tr("TextEdit"),
                                     tr("Name already used"));
            return;
        }
        op = 'n';
    }
    disconnect(tcpSocket, &QIODevice::readyRead, this, &Client::loginResponse);

    out << op;

    out << fileCombo->currentText();

    loginInfo->file = fileCombo->currentText();
    loginInfo->host = hostCombo->currentText();
    loginInfo->port = portLineEdit->text().toInt();

    tcpSocket->flush();
    tcpSocket->write(block);
    this->done(uid);

}

void Client::openLink()
{
    QUrl link = QUrl(linkLineEdit->text());
    if(!link.isValid()) {
        QMessageBox msgBox;
        msgBox.setText("The link inserted is not a valid one.");
        msgBox.exec();
    }
    hostCombo->setEditText(link.host());
    portLineEdit->setText(QString::number(link.port()));
    fileCombo->setEditable(true);
    fileCombo->setEditText(link.path().remove(0, 1));
    fileCombo->setEditable(true);
}

void Client::loginTry()
{

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    int op = 'z'; // UNinitialized value

    if(loginCombo->currentText()=="login")
        op = 'l';
    else if(loginCombo->currentText()=="signup")
        op = 's';
    out << op;

    out << userLineEdit->text();
    out << pwdLineEdit->text();
    //nick da rimuovere?
    if(loginCombo->currentText()=="signup")
            out << nickLineEdit->text();

    connect(tcpSocket, &QIODevice::readyRead, this, &Client::loginResponse);

    tcpSocket->write(block);


}

void Client::signForm()
{
    if (loginCombo->currentText()=="login"){
        nickLineEdit->setEnabled(false);
    }

    if (loginCombo->currentText()=="signup"){
        nickLineEdit->setEnabled(true);
    }
}
