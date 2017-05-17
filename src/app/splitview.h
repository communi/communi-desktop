/*
  Copyright (C) 2008-2016 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SPLITVIEW_H
#define SPLITVIEW_H

#include <QSplitter>
#include <QPointer>
#include <QVariant>
#include <QMenu>

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
    void clearCurrentView();
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

    void zoomIn();
    void zoomOut();
    void resetZoom();
    void closeView();
    void joinChannel();
    void openQuery();

    void showContextMenu(const QPoint& pos);

private:
    BufferView* targetView() const;
    void addViewActions(QMenu* menu, BufferView* view);
    void addSplitActions(QMenu* menu, BufferView* view);
    void addZoomActions(QMenu* menu, BufferView* view);
    void addGlobalActions(QMenu* menu);

    QVariantMap saveSplittedViews(const QSplitter* splitter) const;
    void restoreSplittedViews(QSplitter* splitter, const QVariantMap& state);

    struct Private {
        QList<BufferView*> views;
        QPointer<BufferView> current;
        QList<QPointer<QAction> > unsplitters;
    } d;
};

#endif // SPLITVIEW_H
