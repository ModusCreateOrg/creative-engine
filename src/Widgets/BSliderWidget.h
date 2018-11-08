#ifndef GENUS_BSLIDERWIDGET_H
#define GENUS_BSLIDERWIDGET_H

#include <BFont.h>
#include <Widgets/BWidget.h>

/**
 * Widget implementing a slider UI.
 * Presents a range slider.
 * Allows the user to pick a value between start and end in increments of step.
 */

struct TRange {
  const TInt start;
  const TInt end;
  const TInt step;
  const TInt precision;
};

class BSliderWidget : public BWidget {
public:
    BSliderWidget(char *aTitle, const TRange *aRange, TInt aForeground, TInt aBackground = -1);

    BSliderWidget(const char *aTitle, const TRange *aRange, TInt aForeground, TInt aBackground = -1);

    ~BSliderWidget();

public:
    TInt Render(TInt aX, TInt aY);

    void Run();

public:
    virtual void Select(TFloat aValue) = 0;

protected:
    const TRange *mRange;
    TFloat mSelectedValue;
};


#endif //GENUS_BSLIDERWIDGET_H
