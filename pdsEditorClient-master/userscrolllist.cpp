#include "userscrolllist.h"
#include "userlist.h"

UserScrollList::UserScrollList(QWidget* widget, QWidget* parent)
   : QScrollArea(parent)
{
    setWidget(widget);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

    setWidgetResizable(true);
    setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
}
