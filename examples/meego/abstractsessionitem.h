#ifndef ABSTRACTSESSIONITEM_H
#define ABSTRACTSESSIONITEM_H

#include <QObject>
#include <QStringListModel>
#include "messageformatter.h"

class IrcMessage;
class IrcSession;

class AbstractSessionItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(IrcSession* session READ session)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString subtitle READ subtitle WRITE setSubtitle NOTIFY subtitleChanged)
    Q_PROPERTY(bool current READ isCurrent WRITE setCurrent NOTIFY currentChanged)
    Q_PROPERTY(bool highlighted READ isHighlighted WRITE setHighlighted NOTIFY highlightedChanged)
    Q_PROPERTY(QStringList users READ users NOTIFY usersChanged)
    Q_PROPERTY(QObject* messages READ messages)

public:
    explicit AbstractSessionItem(QObject *parent = 0);

    IrcSession* session() const;

    QString title() const;
    void setTitle(const QString& title);

    QString subtitle() const;
    void setSubtitle(const QString& subtitle);

    bool isCurrent() const;
    void setCurrent(bool current);

    bool isHighlighted() const;
    void setHighlighted(bool highlighted);

    QStringList users() const;
    QObject* messages() const;

signals:
    void titleChanged();
    void subtitleChanged();
    void currentChanged();
    void highlightedChanged();
    void usersChanged();

protected slots:
    virtual void addUser(const QString& user);
    virtual void removeUser(const QString& user);
    virtual void receiveMessage(IrcMessage* message);

protected:
    void setSession(IrcSession* session);

private:
    IrcSession* m_session;
    QString m_title;
    QString m_subtitle;
    bool m_current;
    bool m_highlighted;
    QStringList m_users;
    QStringListModel* m_messages;
    MessageFormatter m_formatter;
};

#endif // ABSTRACTSESSIONITEM_H
