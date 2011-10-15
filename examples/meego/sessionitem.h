#ifndef SESSIONITEM_H
#define SESSIONITEM_H

#include "abstractsessionitem.h"
#include "messagehandler.h"
class IrcSession;

class SessionItem : public AbstractSessionItem
{
    Q_OBJECT
    Q_PROPERTY(QObjectList childItems READ childItems NOTIFY childItemsChanged)

public:
    explicit SessionItem(IrcSession* session);

    QObjectList childItems() const;

public slots:
    void addChild(const QString& name);
    void renameChild(const QString& from, const QString& to);
    void removeChild(const QString& name);

signals:
    void childItemsChanged();

private:
    QObjectList m_children;
    MessageHandler m_handler;
};

#endif // SESSIONITEM_H
