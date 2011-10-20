#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QDeclarativeContext>
#include <QNetworkSession>
class SessionItem;
class Session;

class SessionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool online READ isOnline NOTIFY onlineStateChanged)
    Q_PROPERTY(bool offline READ isOffline NOTIFY offlineStateChanged)

public:
    SessionManager(QDeclarativeContext* context);

    bool isOnline() const;
    bool isOffline() const;

    Q_INVOKABLE void addSession(Session* session);
    Q_INVOKABLE void removeSession(Session* session);
    Q_INVOKABLE bool ensureNetwork();

signals:
    void onlineStateChanged();
    void offlineStateChanged();

private slots:
    void onNetworkStateChanged(QNetworkSession::State state);

private:
    void updateModel();
    QObjectList m_items;
    QDeclarativeContext* m_context;
    QNetworkSession* m_network;
};

#endif // SESSIONMANAGER_H
