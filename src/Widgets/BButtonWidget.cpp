#include "BButtonWidget.h"
#include "Display/Display.h"
#include "Controls.h"
#include <strings.h>

BButtonWidget::BButtonWidget(char *aText, TInt aForeground, TInt aBackground) : BWidget() {
  mText = aText;
}

BButtonWidget::BButtonWidget(const char *aText, TInt aForeground, TInt aBackground) : BWidget() {
  mText = (char *) aText;
  mHeight = 40;
}

BButtonWidget::~BButtonWidget() {}

TInt BButtonWidget::Render(TInt aX, TInt aY) {
  const BFont *font = gWidgetTheme.GetFont(WIDGET_TEXT_FONT);
  const TInt  fg    = gWidgetTheme.GetInt(WIDGET_TEXT_FG),
              bg    = gWidgetTheme.GetInt(WIDGET_TEXT_BG);

  if (IsActive()) {
    gDisplay.renderBitmap->DrawString(ENull, mText, font, aX, aY, bg, fg);
  } else {
    gDisplay.renderBitmap->DrawString(ENull, mText, font, aX, aY, fg, bg);
  }

  return font->mHeight;
}

void BButtonWidget::Run() {
  if (IsActive() && gControls.WasPressed(BUTTONA | BUTTONB | BUTTON_START)) {
    Select();
  }
}
