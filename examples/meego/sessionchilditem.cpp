#include "sessionchilditem.h"
#include "sessionitem.h"
#include <IrcSession>
#include <Irc>

SessionChildItem::SessionChildItem(SessionItem* parent) :
    AbstractSessionItem(parent), m_parent(parent)
{
    setSession(parent->session());
}

void SessionChildItem::updateCurrent(AbstractSessionItem* item)
{
    m_parent->updateCurrent(item);
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
            setUnread(unread() + 1);
    }
    else if (message->type() == IrcMessage::Numeric)
    {
        IrcNumericMessage* numMsg = static_cast<IrcNumericMessage*>(message);
        if (numMsg->code() == Irc::RPL_TOPIC)
            setSubtitle(numMsg->parameters().value(2));
    }
}
