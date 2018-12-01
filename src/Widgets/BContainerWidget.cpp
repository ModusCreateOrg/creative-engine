#include "BContainerWidget.h"
#include "Controls.h"

BContainerWidget::BContainerWidget(TInt aX, TInt aY) : BWidget(aX, aY, ENull) {
  mCurrentWidget = ENull;
  mNextY         = 0;
}

BContainerWidget::~BContainerWidget() {
  BWidget *w = (BWidget *) mList.First();
  while (!(BWidget *) mList.End(w)) {
    BWidget *n = (BWidget *) mList.Next(w);
    delete w;
    w = n;
  }
}

void BContainerWidget::AddWidget(BWidget &aWidget) {
  aWidget.mX          = mX;
  aWidget.mY          = mNextY;
  mNextY += aWidget.mHeight;
  mList.AddTail(aWidget);
  if (!mCurrentWidget) {
    mCurrentWidget = &aWidget;
    mCurrentWidget->Activate();
  }
}

TInt BContainerWidget::Render(TInt aX, TInt aY) {
  TInt h = 0;
  for (BWidget *w = mList.First(); !mList.End(w); w = mList.Next(w)) {

    TInt dy = w->RenderTitle(aX + mX, aY + mY, w == mCurrentWidget);
    aY += dy;
    h += dy;

    dy =w->Render(aX + mX, aY + mY);  // render relative to mY
    aY += dy;
    h += dy;

  }
  return h;
}

void BContainerWidget::Run() {
  for (BWidget *w = (BWidget *) mList.First(); !mList.End(w); w = (BWidget *) mList.Next(w)) {
    w->Run();
  }

  if (gControls.WasPressed(JOYUP) && OnNavigate(JOYUP) && mCurrentWidget->OnNavigate(JOYUP)) {
    mCurrentWidget->Deactivate();
    if (mCurrentWidget == mList.First()) {
      mCurrentWidget = (BWidget *) mList.Last();
    } else {
      mCurrentWidget = (BWidget *) mList.Prev(mCurrentWidget);
    }
    mCurrentWidget->Activate();

    // reset dKeys so next state doesn't react to any keys already pressed
    gControls.dKeys = 0;
  }

  if (gControls.WasPressed(JOYDOWN | BUTTON_SELECT) && OnNavigate(JOYDOWN) && mCurrentWidget->OnNavigate(JOYDOWN)) {
    mCurrentWidget->Deactivate();
    if (mCurrentWidget == mList.Last()) {
      mCurrentWidget = (BWidget *) mList.First();
    } else {
      mCurrentWidget = (BWidget *) mList.Next(mCurrentWidget);
    }
    mCurrentWidget->Activate();

    // reset dKeys so next state doesn't react to any keys already pressed
    gControls.dKeys = 0;
  }
}
