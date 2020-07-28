#ifndef USERSCROLLLIST_H
#define USERSCROLLLIST_H

#include <QWidget>
#include <QScrollArea>

class UserScrollList : public QScrollArea
{
public:
   // Create a scrollable widget around another widget.
   UserScrollList(QWidget * widget, QWidget* parent = nullptr);
};

#endif // USERSCROLLLIST_H
