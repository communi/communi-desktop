#include "session.h"
#include <QSslSocket>

Session::Session(QObject *parent) : IrcSession(parent)
{
    connect(this, SIGNAL(password(QString*)), this, SLOT(onPassword(QString*)));
}

bool Session::isSecure() const
{
    return qobject_cast<QSslSocket*>(socket());
}

void Session::setSecure(bool secure)
{
    QSslSocket* sslSocket = qobject_cast<QSslSocket*>(socket());
    if (secure && !sslSocket)
    {
        sslSocket = new QSslSocket(this);
        sslSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
        setSocket(sslSocket);
    }
    else if (!secure && sslSocket)
    {
        setSocket(new QTcpSocket(this));
    }
}

QString Session::getPassword() const
{
    return m_password;
}

void Session::setPassword(const QString& password)
{
    m_password = password;
}

void Session::onPassword(QString* password)
{
    *password = m_password;
}
