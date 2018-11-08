#include "BNumberWidget.h"
#include "Display.h"
#include "Controls.h"

BNumberWidget::BNumberWidget(char *aTitle, const TNumberOpts *aOpts, TInt aForeground, TInt aBackground) : BWidget(aTitle), mOpts(aOpts) {
  mSelectedValue = TNumber(mOpts->range.start, mOpts->range.precision);
}

BNumberWidget::BNumberWidget(const char *aTitle, const TNumberOpts *aOpts, TInt aForeground, TInt aBackground) : BWidget(), mOpts(aOpts) {
  mSelectedValue = TNumber(mOpts->range.start, mOpts->range.precision);
  mTitle = (char *) aTitle;
  mHeight = 40;
}

BNumberWidget::~BNumberWidget() {}

TInt BNumberWidget::Render(TInt aX, TInt aY) {
  const BFont *font = gWidgetTheme.GetFont(WIDGET_TEXT_FONT);
  const TInt  fg    = gWidgetTheme.GetInt(WIDGET_TEXT_FG),
              bg    = gWidgetTheme.GetInt(WIDGET_TEXT_BG);
  char        c[20];

  // Number to string
  mSelectedValue.ToString(c, &mOpts->pad);

  // Draw string
  gDisplay.renderBitmap->DrawString(ENull, c, font, aX * 2, aY, bg, fg);

  return font->mHeight;
}

void BNumberWidget::Run() {
  if (!IsActive()) {
    return;
  }

  // Decrement, check min value
  if (gControls.WasPressed(JOYLEFT)) {
    mSelectedValue.mVal = MAX(mOpts->range.start, mSelectedValue.mVal - mOpts->range.step);
    return;
  }

  // Increment, check max value
  if (gControls.WasPressed(JOYRIGHT)) {
    mSelectedValue.mVal = MIN(mOpts->range.end, mSelectedValue.mVal + mOpts->range.step);
    return;
  }

  // Store selected value
  if (gControls.WasPressed(BUTTON_SELECT)) {
    Select(mSelectedValue.ToFloat());
  }
}
