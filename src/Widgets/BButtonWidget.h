#ifndef GENUS_BBUTTONWIDGET_H
#define GENUS_BBUTTONWIDGET_H

#include <BFont.h>
#include <Widgets/BWidget.h>

/**
 * Widget implementing a select UI.  Presents a list of options and allows the user to pick one.
 */

class BButtonWidget : public BWidget {
public:
    BButtonWidget(char *aText, TInt aForeground, TInt aBackground = -1);

    BButtonWidget(const char *aText, TInt aForeground, TInt aBackground = -1);

    ~BButtonWidget();

public:
    TInt Render(TInt aX, TInt aY);
    TBool Run();

public:
    virtual void Select() = 0;

public:
    char *mText;
};


#endif //GENUS_BBUTTONWIDGET_H
