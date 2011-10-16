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

    Q_INVOKABLE void addSession(IrcSession* session, const QString& password);
    Q_INVOKABLE void removeSession(IrcSession* session);

private slots:
    void onPassword(QString* password);

private:
    void updateModel();
    QObjectList m_items;
    QDeclarativeContext* m_context;
    QHash<IrcSession*, QString> m_passwords;
};

#endif // SESSIONMANAGER_H
