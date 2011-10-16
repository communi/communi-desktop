#include "sessionmanager.h"
#include "sessionitem.h"

SessionManager::SessionManager(QDeclarativeContext* context) : m_context(context)
{
    updateModel();
}

void SessionManager::addSession(IrcSession* session, const QString& password)
{
    SessionItem* item = new SessionItem(session);
    m_items.append(item);
    updateModel();

    m_passwords.insert(session, password);
    connect(session, SIGNAL(password(QString*)), this, SLOT(onPassword(QString*)));
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

    m_passwords.remove(session);
    disconnect(session, SIGNAL(password(QString*)), this, SLOT(onPassword(QString*)));
}

void SessionManager::onPassword(QString* password)
{
    IrcSession* session = qobject_cast<IrcSession*>(sender());
    *password = m_passwords.value(session);
}

void SessionManager::updateModel()
{
    m_context->setContextProperty("SessionModel", QVariant::fromValue(m_items));
}
