#ifndef GENUS_BSELECTWIDGET_H
#define GENUS_BSELECTWIDGET_H

#include <BFont.h>
#include <Widgets/BWidget.h>

/**
 * Widget implementing a select UI.  Presents a list of options and allows the user to pick one.
 */

struct TSelectOption {
    const char *text;
    const TInt value;
};

#define TSELECT_END_OPTIONS { ENull, 0 }

class BSelectWidget : public BWidget {
public:
    BSelectWidget(char *aTitle, const TSelectOption *aOptions, TInt aForeground, TInt aBackground = -1);

    BSelectWidget(const char *aTitle, const TSelectOption *aOptions, TInt aForeground, TInt aBackground = -1);

    ~BSelectWidget();

public:
    TInt Render(TInt aX, TInt aY);

    TBool Run();

public:
    virtual void Select(TInt aValue) = 0;

protected:
    const TSelectOption *mOptions;
    TInt mSelectedIndex;
};


#endif //GENUS_BSELECTWIDGET_H
