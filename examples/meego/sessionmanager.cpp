#include "sessionmanager.h"
#include "sessionitem.h"
#include "session.h"

SessionManager::SessionManager(QDeclarativeContext* context) : m_context(context)
{
    updateModel();
}

void SessionManager::addSession(Session* session)
{
    SessionItem* item = new SessionItem(session);
    m_items.append(item);
    updateModel();
}

void SessionManager::removeSession(Session* session)
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
