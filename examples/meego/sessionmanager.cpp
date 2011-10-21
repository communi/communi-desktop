#include "sessionmanager.h"
#include "sessionitem.h"
#include "session.h"
#include <QNetworkConfigurationManager>

SessionManager::SessionManager(QDeclarativeContext* context) :
    m_context(context), m_network(0)
{
    updateModel();
}

bool SessionManager::isOnline() const
{
    return m_network && m_network->state() == QNetworkSession::Connected;
}

bool SessionManager::isOffline() const
{
    return !m_network || m_network->state() != QNetworkSession::Connected;
}

void SessionManager::addSession(Session* session)
{
    SessionItem* item = new SessionItem(session);
    connect(item, SIGNAL(alert(QString)), SIGNAL(alert(QString)));
    m_items.append(item);
    updateModel();

    if (ensureNetwork())
        session->open();
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

bool SessionManager::ensureNetwork()
{
    if (!m_network || !m_network->isOpen())
    {
        QNetworkConfigurationManager manager;
        if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
        {
            QNetworkConfiguration config = manager.defaultConfiguration();
            if (!m_network)
            {
                m_network = new QNetworkSession(config, this);
                connect(m_network, SIGNAL(stateChanged(QNetworkSession::State)), this, SLOT(onNetworkStateChanged(QNetworkSession::State)));
                connect(m_network, SIGNAL(stateChanged(QNetworkSession::State)), this, SIGNAL(onlineStateChanged()));
                connect(m_network, SIGNAL(stateChanged(QNetworkSession::State)), this, SIGNAL(offlineStateChanged()));
            }
            m_network->open();
        }
    }
    return m_network->isOpen();
}

void SessionManager::onNetworkStateChanged(QNetworkSession::State state)
{
    if (state == QNetworkSession::Connected)
    {
        foreach (QObject* item, m_items)
        {
            IrcSession* session = static_cast<SessionItem*>(item)->session();
            if (!session->isActive())
                session->open();
        }
    }
    else if (state == QNetworkSession::Closing)
    {
        foreach (QObject* item, m_items)
        {
            IrcSession* session = static_cast<SessionItem*>(item)->session();
            if (session->isActive())
                session->close();
        }
    }
}

void SessionManager::updateModel()
{
    m_context->setContextProperty("SessionModel", QVariant::fromValue(m_items));
}
