#ifndef GENUS_BWIDGETTHEME_H
#define GENUS_BWIDGETTHEME_H

#include "BBase.h"
#include "BFont.h"

enum {
    WIDGET_END_TAG,
    WIDGET_TEXT_FONT,
    WIDGET_TEXT_FG,
    WIDGET_TEXT_BG,
    WIDGET_TITLE_FONT,
    WIDGET_TITLE_FG,
    WIDGET_TITLE_BG,
    WIDGET_WINDOW_BG,   // color of body of window
    WIDGET_WINDOW_FG,   // window decoration color
};

class BWidgetTheme : public BBase {
public:
    BWidgetTheme();

    virtual ~BWidgetTheme();

public:
    void Configure(TInt aTag, ...);

    TInt GetInt(TInt aTag);
    BFont *GetFont(TInt aTag);

protected:
    BFont *mTextFont;
    TInt mTextFG, mTextBG;
    BFont *mTitleFont;
    TInt mTitleFG, mTitleBG;
    TInt mWindowFG, mWindowBG;

};

extern BWidgetTheme gWidgetTheme;


#endif //GENUS_BWIDGETTHEME_H
