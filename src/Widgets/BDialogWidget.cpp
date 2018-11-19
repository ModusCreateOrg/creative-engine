#include "BDialogWidget.h"
#include "Display.h"

BDialogWidget::BDialogWidget(char *aTitle, TInt aX, TInt aY) : BContainerWidget(aX, aY) {
  mTitle = aTitle;
}

BDialogWidget::BDialogWidget(const char *aTitle, TInt aX, TInt aY) : BContainerWidget(aX, aY) {
  mTitle = (char *) aTitle;
}

BDialogWidget::~BDialogWidget() {

}

TInt BDialogWidget::Render(TInt aX, TInt aY) {
  BBitmap *bm = gDisplay.renderBitmap;
  TInt t = TUint8(bm->TransparentColor());

  // calculate window dimensions
  TInt h = 0,
      width = SCREEN_WIDTH - (aX+mX)*2;

  for (BWidget *w = First(); !End(w); w = Next(w)) {
    h += w->mHeight;
  }
  h += 20;
  if (h > SCREEN_HEIGHT) {
    h = SCREEN_HEIGHT;
  }

  // render dialog
  TUint8 bg = TUint8(gWidgetTheme.GetInt(WIDGET_WINDOW_BG)),
         fg = TUint8(gWidgetTheme.GetInt(WIDGET_WINDOW_FG));

  // draw Dialog background
  TRect rect(mX+aX, mY+aY, (mX + aX) + width - 1, (mY+aY) + h - 1);

  if (bg != t) {
    bm->FillRect(ENull, (mX + aX), (mY+aY), rect.x2, rect.y2, bg);
  }
  // draw Dialog border
  if (fg != t) {
    bm->DrawFastHLine(ENull, (mX + aX), (mY+aY), (TUint)rect.Width(), fg);
    bm->DrawFastHLine(ENull, (mX + aX), rect.y2, (TUint)rect.Width(), fg);

    bm->DrawFastVLine(ENull, (mX + aX), (mY + aY), (TUint)rect.Height(), fg);
    bm->DrawFastVLine(ENull, (mX + aX + rect.Width() -1), (mY + aY), (TUint)rect.Height(), fg);
  }

  // render content area
//  TInt save = gWidgetTheme.GetInt(WIDGET_TITLE_FG);
//  gWidgetTheme.Configure(WIDGET_TITLE_FG, fg, WIDGET_END_TAG);
//  aY += BWidget::RenderTitle(mX + aX + 2, mY + aY + 2);
//  gWidgetTheme.Configure(WIDGET_TITLE_FG, save, WIDGET_END_TAG);
  BContainerWidget::Render(aX + 2, aY+2);
  return h;
}

void BDialogWidget::Run() {
  BContainerWidget::Run();
}
