#ifndef GENUS_BDIALOGWIDGET_H
#define GENUS_BDIALOGWIDGET_H

#include "BWidgetTheme.h"
#include "BContainerWidget.h"

class BDialogWidget : public BContainerWidget {
public:
    BDialogWidget(char *aTitle, TInt aX, TInt aY);
    BDialogWidget(const char *aTitle, TInt aX, TInt aY);
    virtual ~BDialogWidget();
public:
    TInt Render(TInt aX, TInt aY);
};


#endif //GENUS_BDIALOGWIDGET_H
