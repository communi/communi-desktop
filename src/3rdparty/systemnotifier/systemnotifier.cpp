#include "systemnotifier.h"

SystemNotifier* SystemNotifier::instance()
{
    static SystemNotifier notifier;
    return &notifier;
}

SystemNotifier::SystemNotifier(QObject* parent) : QObject(parent)
{
    initialize();
}

SystemNotifier::~SystemNotifier()
{
    uninitialize();
}
