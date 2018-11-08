#ifndef GENUS_BNUMBERWIDGET_H
#define GENUS_BNUMBERWIDGET_H

#include <BFont.h>
#include <Widgets/BWidget.h>
#include <Widgets/BSliderWidget.h>
#include <Types/TNumber.h>

struct TNumberOpts {
  TRange range;
  TPad   pad;
};

/**
 * Widget implementing a number UI.
 * Presents a number.
 * Allows the user to pick a value between start and end in increments of step.
 * Allows the number to be floating point and padded.
 */

class BNumberWidget : public BWidget {
public:
    BNumberWidget(char *aTitle, const TNumberOpts *aOpts, TInt aForeground, TInt aBackground = -1);

    BNumberWidget(const char *aTitle, const TNumberOpts *aOpts, TInt aForeground, TInt aBackground = -1);

    ~BNumberWidget();

public:
    TInt Render(TInt aX, TInt aY);

    void Run();

public:
    virtual void Select(TFloat aValue) = 0;

protected:
    const TNumberOpts *mOpts;
    TNumber           mSelectedValue;
};


#endif //GENUS_BNUMBERWIDGET_H
