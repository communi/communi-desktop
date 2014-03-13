/*
* Copyright (C) 2008-2014 The Communi Project
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
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

public:
    explicit AbstractFinder(QWidget* parent);
    ~AbstractFinder();

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
