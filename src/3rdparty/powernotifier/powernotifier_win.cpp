#include "powernotifier.h"

#include <qt_windows.h>
#include <qabstracteventdispatcher.h>

#if QT_VERSION >= 0x050000
#include <qabstractnativeeventfilter.h>
#endif // QT_VERSION

class PowerEventFilter
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
                QMetaObject::invokeMethod(PowerNotifier::instance(), "sleep");
                break;
            case PBT_APMRESUMESUSPEND:
                QMetaObject::invokeMethod(PowerNotifier::instance(), "wake");
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

void PowerNotifier::initialize()
{
    PowerEventFilter* filter = new PowerEventFilter;
#if QT_VERSION >= 0x050000
    QAbstractEventDispatcher::instance()->installNativeEventFilter(filter);
#else
    filter->prev = QAbstractEventDispatcher::instance()->setEventFilter(PowerEventFilter::nativeEventFilter);
#endif // QT_VERSION
    d = filter;
}

void PowerNotifier::uninitialize()
{
    PowerEventFilter* filter = static_cast<PowerEventFilter*>(d);
    if (QAbstractEventDispatcher::instance())
#if QT_VERSION >= 0x050000
        QAbstractEventDispatcher::instance()->removeNativeEventFilter(filter);
#else
        QAbstractEventDispatcher::instance()->setEventFilter(filter->prev);
#endif // QT_VERSION
    delete filter;
}
