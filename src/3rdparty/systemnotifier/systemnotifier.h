#ifndef SYSTEMNOTIFIER_H
#define SYSTEMNOTIFIER_H

#include <QObject>

class SystemNotifier : public QObject
{
    Q_OBJECT

public:
    static SystemNotifier* instance();

signals:
    void sleep();
    void wake();

protected:
    SystemNotifier(QObject* parent = 0);
    ~SystemNotifier();

    void initialize();
    void uninitialize();

private:
    void* d;
};

#endif // SYSTEMNOTIFIER_H
