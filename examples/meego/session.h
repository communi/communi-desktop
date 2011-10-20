#ifndef SESSION_H
#define SESSION_H

#include <IrcSession>

class Session : public IrcSession
{
    Q_OBJECT
    Q_PROPERTY(bool secure READ isSecure WRITE setSecure)
    Q_PROPERTY(QString password READ getPassword WRITE setPassword)

public:
    explicit Session(QObject *parent = 0);

    bool isSecure() const;
    void setSecure(bool secure);

    QString getPassword() const;
    void setPassword(const QString& password);

private slots:
    void onPassword(QString* password);

private:
    QString m_password;
};

#endif // SESSION_H
