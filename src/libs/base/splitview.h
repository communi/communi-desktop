/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef SPLITVIEW_H
#define SPLITVIEW_H

#include <QSplitter>

class IrcBuffer;
class BufferView;

class SplitView : public QSplitter
{
    Q_OBJECT

public:
    SplitView(QWidget* parent = 0);

    IrcBuffer* currentBuffer() const;
    BufferView* currentView() const;
    QList<BufferView*> views() const;

public slots:
    void setCurrentView(BufferView* view);
    void setCurrentBuffer(IrcBuffer* buffer);
    void split(Qt::Orientation orientation);

signals:
    void viewAdded(BufferView* view);
    void viewRemoved(BufferView* view);
    void currentViewChanged(BufferView* view);
    void currentBufferChanged(IrcBuffer* buffer);

protected:
    BufferView* createBufferView(QSplitter* splitter);

private slots:
    void onViewRemoved(BufferView* view);
    void onFocusChanged(QWidget* old, QWidget* now);

private:
    struct Private {
        BufferView* current;
        QList<BufferView*> views;
    } d;
};

#endif // SPLITVIEW_H
