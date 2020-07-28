#ifndef USERLIST_H
#define USERLIST_H

#include <QObject>
#include <QFrame>
#include "userlistitem.h"
#include <QBoxLayout>

#include <functional>

class UserList : public QFrame
{
    Q_OBJECT
public:

      // Callback signature when the list was modified: added or remove an item.
      using ListModifiedCallbackFunction = std::function<void(UserList * list)>;

      // Create a widget list widget.
      UserList(ListModifiedCallbackFunction modifCallback = {}, bool stretch = true, QBoxLayout::Direction dir = QBoxLayout::Direction::TopToBottom, QWidget * parent = nullptr);

      // Add a widget item.
      UserListItem* addItem(UserListItem* item, int index = -1);

      // Remove a widget item.
      void removeItem(UserListItem* item);

      QVector<UserListItem *> getItems() const;

   protected:

      // Ensure the widget has enough width to show its items.
      void propagateMinimumDimension();

      ListModifiedCallbackFunction _modifCallback;
      QBoxLayout* _layout = nullptr;
      QLabel* _dropHere = nullptr;

};

#endif // USERLIST_H
