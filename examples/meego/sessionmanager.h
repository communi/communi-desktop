#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QDeclarativeContext>
#include <QNetworkSession>
class SessionItem;
class Session;

class SessionManager : public QObject
{
    Q_OBJECT

public:
    SessionManager(QDeclarativeContext* context);

    Q_INVOKABLE void addSession(Session* session);
    Q_INVOKABLE void removeSession(Session* session);

private slots:
    void onNetworkStateChanged(QNetworkSession::State state);

private:
    bool ensureNetwork();
    void updateModel();
    QObjectList m_items;
    QDeclarativeContext* m_context;
    QNetworkSession* m_network;
};

#endif // SESSIONMANAGER_H
