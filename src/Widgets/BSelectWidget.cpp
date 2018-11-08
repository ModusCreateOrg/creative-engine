#include "BSelectWidget.h"
#include "Display.h"
#include "Controls.h"
#include <strings.h>

BSelectWidget::BSelectWidget(char *aTitle, const TSelectOption *aOptions, TInt aForeground, TInt aBackground)
    : BWidget(aTitle), mOptions(aOptions) {
  mSelectedIndex = 0;
}

BSelectWidget::BSelectWidget(const char *aTitle, const TSelectOption *aOptions, TInt aForeground, TInt aBackground)
    : BWidget(), mOptions(aOptions) {
  mSelectedIndex = 0;
  mTitle = (char *) aTitle;
  mHeight = 40;
}

BSelectWidget::~BSelectWidget() {}

TInt BSelectWidget::Render(TInt aX, TInt aY) {
  const BFont *font = gWidgetTheme.GetFont(WIDGET_TEXT_FONT);
  const TInt fg = gWidgetTheme.GetInt(WIDGET_TEXT_FG),
      bg = gWidgetTheme.GetInt(WIDGET_TEXT_BG);

  // TODO: center and add spaces between options, based upon strlen(); pad left/right, too
  TInt ndx = 0, x = aX;
  while (mOptions[ndx].text) {
    if (ndx != mSelectedIndex) {
      gDisplay.renderBitmap->DrawString(ENull,
                                        mOptions[ndx].text,
                                        font,
                                        x + aX, aY,
                                        fg, bg);
    } else {
      gDisplay.renderBitmap->DrawString(ENull,
                                        mOptions[ndx].text,
                                        font,
                                        x + aX, aY,
                                        bg, fg);
    }

    x += font->mWidth * strlen(mOptions[ndx].text) + 20;
    ndx++;
  }
  return font->mHeight;
}

void BSelectWidget::Run() {
  // move cursor, select, etc.
  if (IsActive()) {
    if (gControls.WasPressed(JOYLEFT)) {
      mSelectedIndex = MAX(mSelectedIndex - 1, 0);
    } else if (gControls.WasPressed(JOYRIGHT)) {
      if (mOptions[mSelectedIndex].text != ENull) {
        mSelectedIndex++;
        if (mOptions[mSelectedIndex].text == ENull) {
          mSelectedIndex = 0;
        }
      }
    } else if (gControls.WasPressed(BUTTON_SELECT)) {
      Select(mOptions[mSelectedIndex].value);
    }
  }
}
