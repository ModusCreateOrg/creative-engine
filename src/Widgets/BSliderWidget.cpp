#include "BSliderWidget.h"
#include "Display.h"
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
  const TInt  fg       = gWidgetTheme.GetInt(WIDGET_TEXT_FG),
              bg       = gWidgetTheme.GetInt(WIDGET_TEXT_BG),
              maxWidth = (SCREEN_WIDTH - aX * 2);

  gDisplay.renderBitmap->DrawRect(ENull, aX, aY, aX + maxWidth - 1, aY + 4, fg);

  if (mSelectedValue == mRange->end) {
    gDisplay.renderBitmap->FillRect(ENull, aX + 2, aY + 2, aX + maxWidth - 3, aY + 2, fg);
  } else if (mSelectedValue > mRange->start) {
    TInt width = maxWidth * (mSelectedValue / mRange->end);
    gDisplay.renderBitmap->FillRect(ENull, aX + 2, aY + 2, aX + width - 3, aY + 2, fg);
  }

  return 16;
}

void BSliderWidget::Run() {
  if (!IsActive()) {
    return;
  }

  // Decrement, check min value and convert to TFloat if needed
  if (gControls.WasPressed(JOYLEFT)) {
    if (mRange->precision > 0) {
      mSelectedValue *= mRange->precision;
      mSelectedValue = MAX(mRange->start, mSelectedValue - mRange->step);
      mSelectedValue /= mRange->precision;
    } else {
      mSelectedValue = MAX(mRange->start, mSelectedValue - mRange->step);
    }
    return;
  }

  // Increment, check max value and convert to TFloat if needed
  if (gControls.WasPressed(JOYRIGHT)) {
    if (mRange->precision > 0) {
      mSelectedValue *= mRange->precision;
      mSelectedValue = MIN(mRange->end, mSelectedValue + mRange->step);
      mSelectedValue /= mRange->precision;
    } else {
      mSelectedValue = MIN(mRange->end, mSelectedValue + mRange->step);
    }
    return;
  }

  // Store selected value
  if (gControls.WasPressed(BUTTON_SELECT)) {
    Select(mSelectedValue);
  }
}
