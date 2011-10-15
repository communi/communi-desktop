#include "sessionmanager.h"
#include "sessionitem.h"

SessionManager::SessionManager(QDeclarativeContext* context) : m_context(context)
{
    updateModel();
}

void SessionManager::addSession(IrcSession* session)
{
    SessionItem* item = new SessionItem(session);
    m_items.append(item);
    updateModel();
}

void SessionManager::removeSession(IrcSession* session)
{
    for (int i = 0; i < m_items.count(); ++i)
    {
        if (static_cast<SessionItem*>(m_items.at(i))->session() == session)
        {
            m_items.removeAt(i);
            updateModel();
            break;
        }
    }
}

void SessionManager::updateModel()
{
    m_context->setContextProperty("SessionModel", QVariant::fromValue(m_items));
}
