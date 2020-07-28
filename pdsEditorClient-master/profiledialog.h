#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include "user.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

class ProfileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProfileDialog(QWidget *parent = nullptr, User *user= nullptr, QString *uname=nullptr, QString *pw=nullptr);
private:
    QLineEdit *nickEdit = nullptr;
    QLineEdit *userEdit = nullptr;
    QLineEdit *pwEdit = nullptr;
    QPushButton *acceptButton = nullptr;
    User* user = nullptr;
    QString fileName;
    QString *uname;
    QString *pw;
private slots:
    void changesAccepted();
    void openImageFromFile();
};

#endif // PROFILEDIALOG_H
