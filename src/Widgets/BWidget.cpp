#include "BWidget.h"
#include <Display.h>

// BWidgetList

BWidgetList::BWidgetList() {

}

BWidgetList::~BWidgetList() {

}

// run all the widgets in this list
void BWidgetList::Run() {
  for (BWidget *w = (BWidget *) First(); !End(w); w = (BWidget *) Next(w)) {
    w->Run();
  }
}

// BWidget

BWidget::BWidget(TInt aX, TInt aY, char *aTitle) : BNode() {
  mX = aX;
  mY = aY;
  mActive = EFalse;
  mHeight = 16;
  mTitle = aTitle;
}

BWidget::BWidget(char *aTitle) : BNode(), mTitle(aTitle) {
  mTitle = aTitle;
  mX = mY = 0;
  mActive = EFalse;
  mHeight = 16;
}


BWidget::BWidget() : BNode(), mX(0), mY(0), mTitle(ENull) {
  mActive = EFalse;
  mHeight = 20;
}

BWidget::~BWidget() {}

TInt BWidget::RenderTitle(TInt aX, TInt aY, TBool aActive) {
  if (!mTitle) {
    return 0;
  }
  const BFont *f = gWidgetTheme.GetFont(WIDGET_TITLE_FONT);
  if (aActive) {
    gDisplay.renderBitmap->DrawString(ENull,
                                      mTitle,
                                      f,
                                      aX, aY,
                                      gWidgetTheme.GetInt(WIDGET_TITLE_BG),
                                      gWidgetTheme.GetInt(WIDGET_TITLE_FG));

  } else {
    gDisplay.renderBitmap->DrawString(ENull,
                                      mTitle,
                                      f,
                                      aX, aY,
                                      gWidgetTheme.GetInt(WIDGET_TITLE_FG),
                                      gWidgetTheme.GetInt(WIDGET_TITLE_BG));
  }
  return f->mHeight + 4;
}

TBool BWidget::OnNavigate(TUint16 bits) {
  return ETrue;
}
