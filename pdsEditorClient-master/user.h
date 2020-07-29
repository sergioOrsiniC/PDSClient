#ifndef USER_H
#define USER_H

#include <QColor>
#include <QString>
#include <QImage>

class User{
public:
    User(quint32 u, QString n, QColor col, int tc): uid(u), nick(n), color(col), startCursor(tc){}
    User(){}
    quint32 uid = 0;
    QString nick = "";
    QColor color = QColor();
    int startCursor = 0;
    QImage icon =  QImage();
};

#endif // USER_H
