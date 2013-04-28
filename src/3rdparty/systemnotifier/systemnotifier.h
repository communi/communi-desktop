#ifndef SYSTEMNOTIFIER_H
#define SYSTEMNOTIFIER_H

#include <QObject>

class SystemNotifierPrivate;

class SystemNotifier : public QObject
{
    Q_OBJECT

public:
    static SystemNotifier* instance();

signals:
    void sleep();
    void wake();
    void online();
    void offline();

protected:
    SystemNotifier(QObject* parent = 0);
    ~SystemNotifier();

    void initialize();
    void uninitialize();

private:
    SystemNotifierPrivate* d;
};

#endif // SYSTEMNOTIFIER_H
