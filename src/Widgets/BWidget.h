#ifndef GENUS_BWIDGET_H
#define GENUS_BWIDGET_H

#include <BList.h>
#include <BFont.h>
#include "BWidgetTheme.h"

class BWidget : public BNode {
public:
    BWidget(TInt aX, TInt aY, char *aTitle);

    BWidget(char *aTitle);

    BWidget();

    virtual ~BWidget();

public:
    virtual TInt Render(TInt aX, TInt aY) = 0;

    virtual void Run() = 0;

public:
    // A widget can be acvtive or inactive.
    // if active, a widget may process controls
    // if inactive, it might not
    // certain appearance features of a widget may also depend on active state
    TBool IsActive() { return mActive; }

    void Activate() { mActive = ETrue; }

    void Deactivate() { mActive = EFalse; }

public:
    TBool mActive;
    TInt mX, mY;
    char *mTitle;
    TInt mHeight;

#if 0
private:
    static TInt mDialogBG, mDialogFG;
private:
    static BFont *mTextFont;
    static TInt mTextColor;
    static TInt mTextBackgroundColor;

public:
    static void SetTextFont(BFont *aFont) {
      mTextFont = aFont;
    }
    static BFont *GetTextFont() { return mTextFont; }
    static void SetTextColors(TInt aForeground, TInt aBackground = -1) {
      mTextColor = aForeground;
      mTextBackgroundColor = aBackground;
    }
    static TInt GetTextForeground() { return mTextColor; }
    static TInt GetTextBackground() { return mTextBackgroundColor; }

private:
    static BFont *mTitleFont;
    static TInt mTitleColor;
    static TInt mTitleBackgroundColor;

public:
    static void SetTitleFont(BFont *aFont) {
      mTitleFont = aFont;
    }
    static void SetTitleColors(TInt aForeground, TInt aBackground = -1) {
      mTitleColor = aForeground;
      mTitleBackgroundColor = aBackground;
    }
    static TInt GetTitleForeground() { return mTitleColor; }
    static TInt GetTitleBackground() { return mTitleBackgroundColor; }

    static BFont *GetTitleFont() { return mTitleFont; }

#endif
public:
    TInt RenderTitle(TInt aX, TInt aY, TBool aActive = EFalse);
};

class BWidgetList : public BList{
public:
    BWidgetList();

    ~BWidgetList();

public:
    BWidget *RemHead() { return (BWidget *)BList::RemHead(); }
    BWidget *First() { return (BWidget *)BList::First(); }
    BWidget *Last() { return (BWidget *)BList::Last(); }
    BWidget *Next(BWidget *aWidget) { return (BWidget *)BList::Next(aWidget); }
    BWidget *Prev(BWidget *aWidget) { return (BWidget *)BList::Prev(aWidget); }
    TBool End(BWidget *aWidget) { return (BWidget *)BList::End(aWidget); }
public:
    void Run();

};

#endif //GENUS_BWIDGET_H
