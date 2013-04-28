#include "systemnotifier.h"

#include <qt_windows.h>
#include <qabstracteventdispatcher.h>

#if QT_VERSION >= 0x050000
#include <qabstractnativeeventfilter.h>
#endif // QT_VERSION

class SystemNotifierPrivate
#if QT_VERSION >= 0x050000
                       : public QAbstractNativeEventFilter
#endif
{
public:
#if QT_VERSION >= 0x050000
    bool nativeEventFilter(const QByteArray&, void* message, long*)
#else
    static bool nativeEventFilter(void* message)
#endif
    {
        MSG* msg = static_cast<MSG*>(message);
        if (msg && msg->message == WM_POWERBROADCAST) {
            switch (msg->wParam) {
            case PBT_APMSUSPEND:
                QMetaObject::invokeMethod(SystemNotifier::instance(), "sleep");
                break;
            case PBT_APMRESUMESUSPEND:
                QMetaObject::invokeMethod(SystemNotifier::instance(), "wake");
                break;
            default:
                break;
            }
        }
        return false;
    }
#if QT_VERSION < 0x050000
    QAbstractEventDispatcher::EventFilter prev;
#endif
};

void SystemNotifier::initialize()
{
    d = new SystemNotifierPrivate;
#if QT_VERSION >= 0x050000
    QAbstractEventDispatcher::instance()->installNativeEventFilter(d);
#else
    d->prev = QAbstractEventDispatcher::instance()->setEventFilter(SystemNotifierPrivate::nativeEventFilter);
#endif // QT_VERSION
}

void SystemNotifier::uninitialize()
{
    if (QAbstractEventDispatcher::instance())
#if QT_VERSION >= 0x050000
        QAbstractEventDispatcher::instance()->removeNativeEventFilter(d);
#else
        QAbstractEventDispatcher::instance()->setEventFilter(d->prev);
#endif // QT_VERSION
    delete d;
}
