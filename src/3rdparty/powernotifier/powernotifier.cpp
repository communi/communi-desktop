#include "powernotifier.h"

PowerNotifier* PowerNotifier::instance()
{
    static PowerNotifier notifier;
    return &notifier;
}

PowerNotifier::PowerNotifier(QObject* parent) : QObject(parent)
{
    initialize();
}

PowerNotifier::~PowerNotifier()
{
    uninitialize();
}
