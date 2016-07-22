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

#ifndef ABSTRACTFINDER_H
#define ABSTRACTFINDER_H

#include <QLineEdit>
#include <QToolButton>

class AbstractFinder : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool error READ hasError)
    Q_PROPERTY(int offset READ offset WRITE setOffset)
    Q_PROPERTY(QString text READ text WRITE setText)

public:
    explicit AbstractFinder(QWidget* parent);
    ~AbstractFinder();

    QString text() const;
    void setText(const QString& text);

    int offset() const;
    void setOffset(int offset);

    bool hasError() const;
    void setError(bool error);

public slots:
    void doFind();
    void reFind();
    void findNext();
    void findPrevious();

    void animateShow();
    void animateHide();

    bool eventFilter(QObject* object, QEvent* event);

signals:
    void returnPressed();
    void destroyed(AbstractFinder* input);

protected slots:
    virtual void find(const QString& text, bool forward = false, bool backward = false, bool typed = true) = 0;

protected slots:
    virtual void relocate() = 0;

private:
    struct Private {
        int offset;
        bool error;
        QLineEdit* lineEdit;
        QToolButton* prevButton;
        QToolButton* nextButton;
    } d;
};

#endif // ABSTRACTFINDER_H
