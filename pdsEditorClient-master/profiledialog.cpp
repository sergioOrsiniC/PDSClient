#include "profiledialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFileDialog>

ProfileDialog::ProfileDialog(QWidget *parent, User* user, QString *uname, QString *pw) : QDialog(parent)
{
    auto layout = new QGridLayout(this);
    this->user = user;
    this->pw = pw;
    this->uname = uname;

    nickEdit = new QLineEdit(user->nick);
    auto nickLabel = new QLabel(tr("User nick:"));
    nickLabel->setBuddy(nickEdit);
    userEdit = new QLineEdit();
    auto userLabel = new QLabel(tr("Username:"));
    userLabel->setBuddy(userEdit);
    pwEdit = new QLineEdit();
    auto pwLabel = new QLabel(tr("Password:"));
    pwLabel->setBuddy(pwEdit);

    auto picButton = new QPushButton(tr("Select propic from file..."));

    layout->addWidget(userLabel);
    layout->addWidget(userEdit);
    layout->addWidget(pwLabel);
    layout->addWidget(pwEdit);
    layout->addWidget(nickLabel);
    layout->addWidget(nickEdit);
    layout->addWidget(picButton);

    auto quitButton = new QPushButton(tr("Cancel"));

    acceptButton = new QPushButton(tr("Ok"));

    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(acceptButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    layout->addWidget(buttonBox);

    connect(picButton, &QAbstractButton::clicked,
            this, &ProfileDialog::openImageFromFile);
    connect(acceptButton, &QAbstractButton::clicked,
            this, &ProfileDialog::changesAccepted);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);

}

void ProfileDialog::changesAccepted()
{
    *uname = userEdit->text();
    *pw = pwEdit->text();
    user->nick = nickEdit->text();
    user->icon = QImage(fileName).scaled(32, 32, Qt::IgnoreAspectRatio);
    this->done(Accepted);
}

void ProfileDialog::openImageFromFile()
{
    fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/home/", tr("Image Files (*.png *.jpg *.bmp)"));
}
