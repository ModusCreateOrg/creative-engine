#include "BSliderWidget.h"
#include "Display/Display.h"
#include "Controls.h"

BSliderWidget::BSliderWidget(char *aTitle, const TRange *aRange, TInt aForeground, TInt aBackground)
    : BWidget(aTitle), mRange(aRange) {
  mSelectedValue = mRange->start;
}

BSliderWidget::BSliderWidget(const char *aTitle, const TRange *aRange, TInt aForeground, TInt aBackground)
    : BWidget(), mRange(aRange) {
  mSelectedValue = mRange->start;
  mTitle = (char *) aTitle;
  mHeight = 40;
}

BSliderWidget::~BSliderWidget() {}

TInt BSliderWidget::Render(TInt aX, TInt aY) {
  const TUint8 fg       = gWidgetTheme.GetInt(WIDGET_TEXT_FG),
             maxWidth = (SCREEN_WIDTH - aX * 2);

  gDisplay.renderBitmap->DrawRect(ENull, aX, aY, aX + maxWidth - 1, aY + 4, fg);

  if (mSelectedValue == mRange->end) {
    gDisplay.renderBitmap->FillRect(ENull, aX + 2, aY + 2, aX + maxWidth - 3, aY + 2, fg);
  } else if (mSelectedValue > mRange->start) {
    TUint base = mRange->precision ? mRange->precision : 100;
    TInt width = maxWidth * (mSelectedValue / base);
    gDisplay.renderBitmap->FillRect(ENull, aX + 2, aY + 2, aX + 2 + width, aY + 2, fg);
  }

  return 16;
}

void BSliderWidget::Run() {
  if (!IsActive()) {
    return;
  }

  // Decrement, check min value and convert to TFloat if needed
  if (gControls.WasPressed(JOYLEFT)) {
    mSelectedValue = MAX(mRange->start, mSelectedValue - mRange->step);
    Select(mSelectedValue);
    return;
  }

  // Increment, check max value and convert to TFloat if needed
  if (gControls.WasPressed(JOYRIGHT)) {
    mSelectedValue = MIN(mRange->end, mSelectedValue + mRange->step);
    Select(mSelectedValue);
  }
}
