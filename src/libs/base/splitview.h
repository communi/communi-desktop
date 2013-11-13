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
#include <QPointer>

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

    void reset();
    void split(Qt::Orientation orientation);
    void split(BufferView* view, Qt::Orientation orientation = Qt::Vertical);
    QSplitter* wrap(BufferView* view, Qt::Orientation orientation = Qt::Vertical);

signals:
    void viewAdded(BufferView* view);
    void viewRemoved(BufferView* view);
    void currentViewChanged(BufferView* view);
    void currentBufferChanged(IrcBuffer* buffer);

protected:
    BufferView* createBufferView(QSplitter* splitter, int index = -1);

private slots:
    void onViewRemoved(BufferView* view);
    void onBufferRemoved(IrcBuffer* buffer);
    void onFocusChanged(QWidget* old, QWidget* now);

private:
    struct Private {
        QList<BufferView*> views;
        QPointer<BufferView> current;
    } d;
};

#endif // SPLITVIEW_H
