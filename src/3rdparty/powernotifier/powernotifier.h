#ifndef POWERNOTIFIER_H
#define POWERNOTIFIER_H

#include <QObject>

class PowerNotifier : public QObject
{
    Q_OBJECT

public:
    static PowerNotifier* instance();

signals:
    void sleep();
    void wake();

protected:
    PowerNotifier(QObject* parent = 0);
    ~PowerNotifier();

    void initialize();
    void uninitialize();

private:
    void* d;
};

#endif // POWERNOTIFIER_H
