#include "sessionitem.h"
#include "sessionchilditem.h"
#include <IrcSession>
#include <IrcMessage>

SessionItem::SessionItem(IrcSession* session) : AbstractSessionItem(session)
{
    setTitle(session->host());
    setSubtitle(session->nickName());

    connect(session, SIGNAL(hostChanged(QString)), this, SLOT(setTitle(QString)));
    connect(session, SIGNAL(nickNameChanged(QString)), this, SLOT(setSubtitle(QString)));

    connect(session, SIGNAL(activeChanged(bool)), this, SIGNAL(busyChanged()));
    connect(session, SIGNAL(connectedChanged(bool)), this, SIGNAL(busyChanged()));

    setSession(session);
    m_handler.setSession(session);
    m_handler.setDefaultReceiver(this);
    connect(&m_handler, SIGNAL(receiverToBeAdded(QString)), SLOT(addChild(QString)));
    connect(&m_handler, SIGNAL(receiverToBeRenamed(QString,QString)), SLOT(renameChild(QString,QString)));
    connect(&m_handler, SIGNAL(receiverToBeRemoved(QString)), SLOT(removeChild(QString)));
    updateCurrent(this);
}

bool SessionItem::isBusy() const
{
    const IrcSession* session = m_handler.session();
    return session && session->isActive() && !session->isConnected();
}

QObjectList SessionItem::childItems() const
{
    return m_children;
}

void SessionItem::updateCurrent(AbstractSessionItem* item)
{
    m_handler.setCurrentReceiver(item);
}

void SessionItem::addChild(const QString& name)
{
    SessionChildItem* child = new SessionChildItem(this);
    child->setTitle(name);
    m_handler.addReceiver(name, child);
    m_children.append(child);
    emit childItemsChanged();
}

void SessionItem::renameChild(const QString& from, const QString& to)
{
    for (int i = 0; i < m_children.count(); ++i)
    {
        SessionChildItem* child = static_cast<SessionChildItem*>(m_children.at(i));
        if (child->title() == from)
        {
            child->setTitle(to);
            emit childItemsChanged();
            break;
        }
    }
}

void SessionItem::removeChild(const QString& name)
{
    for (int i = 0; i < m_children.count(); ++i)
    {
        SessionChildItem* child = static_cast<SessionChildItem*>(m_children.at(i));
        if (child->title() == name)
        {
            m_children.removeAt(i);
            emit childItemsChanged();
            break;
        }
    }
}
