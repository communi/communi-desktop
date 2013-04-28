#include "systemnotifier.h"
#include <QtDBus>

class SystemNotifierPrivate : public QObject
{
    Q_OBJECT

private slots:
    void networkStateChanged(uint state)
    {
        static const uint NM_STATE_DISCONNECTED = 20;
        static const uint NM_STATE_CONNECTED_GLOBAL = 70;

        if (state == NM_STATE_DISCONNECTED)
            QMetaObject::invokeMethod(SystemNotifier::instance(), "offline");
        else if (state == NM_STATE_CONNECTED_GLOBAL)
            QMetaObject::invokeMethod(SystemNotifier::instance(), "online");
    }
};

void SystemNotifier::initialize()
{
    d = new SystemNotifierPrivate;

    QDBusConnection bus = QDBusConnection::systemBus();
    bus.connect("org.freedesktop.NetworkManager", "/org/freedesktop/NetworkManager",
                "org.freedesktop.NetworkManager", "StateChanged", d, SLOT(networkStateChanged(uint)));
}

void SystemNotifier::uninitialize()
{
    delete d;
}

#include "systemnotifier_dbus.moc"
