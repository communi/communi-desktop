/*
 * Copyright (C) 2008-2014 The Communi Project
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
#include <QVariant>

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

    QByteArray saveState() const;
    void restoreState(const QByteArray& state);

public slots:
    void setCurrentView(BufferView* view);
    void setCurrentBuffer(IrcBuffer* buffer);

    void reset();
    void split(Qt::Orientation orientation);
    void split(BufferView* view, Qt::Orientation orientation = Qt::Vertical);
    QSplitter* wrap(BufferView* view, Qt::Orientation orientation = Qt::Vertical);

    void addBuffer(IrcBuffer* buffer);
    void removeBuffer(IrcBuffer* buffer);

signals:
    void viewAdded(BufferView* view);
    void viewRemoved(BufferView* view);
    void currentBufferChanged(IrcBuffer* buffer);
    void currentViewChanged(BufferView* current, BufferView* previous = 0);

protected:
    BufferView* createBufferView(QSplitter* splitter, int index = -1);

private slots:
    void activateNextView();
    void activatePreviousView();

    void onViewRemoved(BufferView* view);
    void onFocusChanged(QWidget* old, QWidget* now);

    void updateActions();
    void splitVertical();
    void splitHorizontal();
    void unsplit();

    void closeView();
    void joinChannel();
    void openQuery();

    void prepareViewMenu(BufferView* view);
    void showContextMenu(const QPoint& pos);

private:
    QVariantMap saveSplittedViews(const QSplitter* splitter) const;
    void restoreSplittedViews(QSplitter* splitter, const QVariantMap& state);

    struct Private {
        QAction* unsplitAction;
        QList<BufferView*> views;
        QPointer<BufferView> current;
    } d;
};

#endif // SPLITVIEW_H
