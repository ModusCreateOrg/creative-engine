#include "BWidgetTheme.h"

#include <cstdarg>
#include <Panic.h>

BWidgetTheme gWidgetTheme;

BWidgetTheme::BWidgetTheme() {}
BWidgetTheme::~BWidgetTheme() {}

void BWidgetTheme::Configure(TInt aTag, ...) {
  TInt tag = aTag;
  va_list args;
  va_start(args, aTag);

  while (tag != WIDGET_END_TAG) {
    switch (tag) {
      case WIDGET_TEXT_FONT:
        mTextFont = (BFont *)va_arg(args, void *);
        break;
      case WIDGET_TEXT_FG:
        mTextFG = va_arg(args, TInt);
        break;
      case WIDGET_TEXT_BG:
        mTextBG = va_arg(args, TInt);
        break;
      case WIDGET_TITLE_FONT:
        mTitleFont = (BFont *)va_arg(args, void *);
        break;
      case WIDGET_TITLE_FG:
        mTitleFG = va_arg(args, TInt);
        break;
      case WIDGET_TITLE_BG:
        mTitleBG = va_arg(args, TInt);
        break;
      case WIDGET_WINDOW_FG:
        mWindowFG = va_arg(args, TInt);
        break;
      case WIDGET_WINDOW_BG:
        mWindowBG = va_arg(args, TInt);
        break;
      case WIDGET_SLIDER_FG:
        mSliderFG = va_arg(args, TInt);
        break;
      case WIDGET_SLIDER_BG:
        mSliderBG = va_arg(args, TInt);
        break;
      default:
        Panic("Invalid BWidgetTheme configure tag: %d\n", tag);
    }
    tag = va_arg(args, TInt);
  }
}

TInt BWidgetTheme::GetInt(TInt aTag){
  switch (aTag) {
    case WIDGET_TEXT_FG:
      return mTextFG;
    case WIDGET_TEXT_BG:
      return mTextBG;
    case WIDGET_TITLE_FG:
      return mTitleFG;
    case WIDGET_TITLE_BG:
      return mTitleBG;
    case WIDGET_WINDOW_FG:
      return mWindowFG;
    case WIDGET_WINDOW_BG:
      return mWindowBG;
    case WIDGET_SLIDER_FG:
      return mSliderFG;
    case WIDGET_SLIDER_BG:
      return mSliderBG;
    default:
      Panic("Invalid BWidgetTheme GertInt tag: %d\n", aTag);
  }
  return 0;
}

BFont *BWidgetTheme::GetFont(TInt aTag) {
  switch (aTag) {
    case WIDGET_TEXT_FONT:
      return mTextFont;
    case WIDGET_TITLE_FONT:
      return mTitleFont;
    default:
      Panic("Invalid BWidgetTheme GertFont tag: %d\n", aTag);
  }
  return ENull;
}
