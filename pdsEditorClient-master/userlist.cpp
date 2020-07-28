#include "userlist.h"
#include "userscrolllist.h"
#include <algorithm>

UserList::UserList(ListModifiedCallbackFunction modifCallback, bool stretch, QBoxLayout::Direction dir, QWidget* parent)
   : QFrame(parent), _modifCallback(modifCallback)
   {
      setBackgroundRole(QPalette::ColorRole::Base);
      setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
      setMinimumSize(QSize(20, 20));
      if (!stretch)
         setFrameStyle(QFrame::Box);

      _layout = new QBoxLayout(dir);
      _layout->setSizeConstraint(QLayout::SetMinimumSize);
      _layout->setMargin(2);
      _layout->setSpacing(0);
      setLayout(_layout);

      if (stretch)
         _layout->addStretch(0);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Minimum size propagation.

   void UserList::propagateMinimumDimension()
   {
      int minSoFar = 0;
      QWidget* widget = this;
      while (widget)
      {
         if (auto scroll = dynamic_cast<UserScrollList*>(widget))
         {
            if (auto list = dynamic_cast<UserList*>(scroll->widget()))
            {
               auto items = list->getItems();
                  auto maxMinWidthPos = std::max_element(items.begin(), items.end(), [](const UserListItem* lhs, const UserListItem* rhs)
                  {
                     return lhs->sizeHint().width() < rhs->sizeHint().width();
                  });

                  if (maxMinWidthPos != items.end())
                  {
                     const int newWidth = (*maxMinWidthPos)->sizeHint().width() + scroll->contentsMargins().left() + scroll->contentsMargins().right();
                     minSoFar = std::max(minSoFar, newWidth);
                     scroll->setMinimumWidth(minSoFar);
                  }


            }
         }
         widget = widget->parentWidget();
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Items management.


   UserListItem* UserList::addItem(UserListItem* item, int index)
   {
      if (!item)
         return nullptr;

      if (!_layout)
         return nullptr;

      if (index < 0 || index >= _layout->count())
         index = _layout->count() - 1;

      _layout->insertWidget(index, item);

      setMinimumWidth(std::max(minimumWidth(), item->sizeHint().width() + contentsMargins().left() + contentsMargins().right()));

      propagateMinimumDimension();

      if (_modifCallback)
         _modifCallback(this);

      return item;
   }

   void UserList::removeItem(UserListItem* item)
   {
      item->setParent(nullptr);
      _layout->removeWidget(item);
      _layout->update();

      if (_modifCallback)
         _modifCallback(this);
   }

   QVector<UserListItem*> UserList::getItems() const
   {
      QVector<UserListItem*> widgets;

      const int c = _layout->count();
      for (int i = 0; i < c; ++i)
         if (auto w = dynamic_cast<UserListItem*>(_layout->itemAt(i)->widget()))
               widgets.push_back(w);
      return widgets;
   }
