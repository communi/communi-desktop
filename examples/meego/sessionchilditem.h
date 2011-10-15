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

protected slots:
    void receiveMessage(IrcMessage* message);

private slots:
    void updateSubtitle();

private:
    SessionItem* m_parent;
    int m_unread;
};

#endif // SESSIONCHILDITEM_H
