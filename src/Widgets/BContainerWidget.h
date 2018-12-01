/**
 * Widget that is a container of other widgets.
 *
 * Container is a BNode linked list.
 *
 * Run() runs all the widgets in the list.
 *
 * Joy Up/Down advances to the next widget, and activates it.
 */
#ifndef GENUS_BCONTAINERWIDGET_H
#define GENUS_BCONTAINERWIDGET_H

#include "BWidget.h"
#include "BList.h"

class BContainerWidget : public BWidget {
public:
    BContainerWidget(TInt aX, TInt aY);

    virtual ~BContainerWidget();

public:
    void AddWidget(BWidget &aWidget);

    BWidget *RemHead() { return (BWidget *) mList.RemHead(); }

    BWidget *First() { return (BWidget *) mList.First(); }

    BWidget *Last() { return (BWidget *) mList.Last(); }

    BWidget *Next(BWidget *aWidget) { return (BWidget *) mList.Next(aWidget); }

    BWidget *Prev(BWidget *aWidget) { return (BWidget *) mList.Next(aWidget); }

    TBool End(BWidget *aWidget) { return (BWidget *) mList.End(aWidget); }

public:
    TInt Render(TInt aX, TInt aY);

    TBool Run();

protected:
    BWidgetList mList;
    TInt mNextY;
    BWidget *mCurrentWidget;
};

#endif //GENUS_BCONTAINERWIDGET_H
