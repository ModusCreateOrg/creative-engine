#ifndef GENUS_BMENUWIDGET_H
#define GENUS_BMENUWIDGET_H

#include "BWidget.h"
#include "BFont.h"

class BMenuWidget : public BWidget {
public:
  BMenuWidget(TInt aX, TInt aY, BFont *aFont, TUint8 aForeground, TUint8 aBackground);
  ~BMenuWidget();
public:
  // add child widgets for the menu
  void AddWidget(BWidget& aWidget) {
    mList.AddTail(aWidget);
  }
protected:
  BWidgetList mList;
};


#endif //GENUS_BMENUWIDGET_H
