#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>
#include <IrcSession>
#include <QDeclarativeContext>
class SessionItem;

class SessionManager : public QObject
{
    Q_OBJECT

public:
    SessionManager(QDeclarativeContext* context);

    Q_INVOKABLE void addSession(IrcSession* session);
    Q_INVOKABLE void removeSession(IrcSession* session);

private:
    void updateModel();
    QObjectList m_items;
    QDeclarativeContext* m_context;
};

#endif // SESSIONMANAGER_H
