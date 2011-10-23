#ifndef SESSIONCHILDITEM_H
#define SESSIONCHILDITEM_H

#include "abstractsessionitem.h"

class SessionItem;
class IrcMessage;

class SessionChildItem : public AbstractSessionItem
{
    Q_OBJECT

public:
    explicit SessionChildItem(SessionItem* parent);

    void updateCurrent(AbstractSessionItem* item);

protected slots:
    void receiveMessage(IrcMessage* message);

private slots:
    void updateIcon();

private:
    SessionItem* m_parent;
};

#endif // SESSIONCHILDITEM_H
