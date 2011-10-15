#include "sessionchilditem.h"
#include "sessionitem.h"
#include <IrcSession>

SessionChildItem::SessionChildItem(SessionItem* parent) :
    AbstractSessionItem(parent), m_parent(parent), m_unread(0)
{
    setSession(parent->session());
    connect(this, SIGNAL(currentChanged()), this, SLOT(updateSubtitle()));
    updateSubtitle();
}

void SessionChildItem::receiveMessage(IrcMessage* message)
{
    AbstractSessionItem::receiveMessage(message);
    if (message->type() == IrcMessage::Private)
    {
        IrcPrivateMessage* privMsg = static_cast<IrcPrivateMessage*>(message);
        if (privMsg->message().contains(m_parent->session()->nickName()))
            setHighlighted(true);

        if (!isCurrent())
        {
            ++m_unread;
            updateSubtitle();
        }
    }
}

void SessionChildItem::updateSubtitle()
{
    if (isCurrent())
        m_unread = 0;

    if (m_unread == 0)
        setSubtitle(" ");
    else
        setSubtitle(tr("%1 unread message(s)").arg(m_unread));
}
