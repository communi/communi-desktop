#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QDeclarativeContext>
class SessionItem;
class Session;

class SessionManager : public QObject
{
    Q_OBJECT

public:
    SessionManager(QDeclarativeContext* context);

    Q_INVOKABLE void addSession(Session* session);
    Q_INVOKABLE void removeSession(Session* session);

private:
    void updateModel();
    QObjectList m_items;
    QDeclarativeContext* m_context;
};

#endif // SESSIONMANAGER_H
