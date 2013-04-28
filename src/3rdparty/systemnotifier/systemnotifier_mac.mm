#include "systemnotifier.h"
#include "Reachability.h"
#include <Cocoa/Cocoa.h>

@interface CocoaSystemNotifier : NSObject
@end

class SystemNotifierPrivate
{
public:
    Reachability* reachability;
    CocoaSystemNotifier* notifier;
};

@implementation CocoaSystemNotifier
- (id)init
{
    self = [super init];

    if (self) {
        [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver: self
            selector: @selector(receiveSleepNote:)
            name: NSWorkspaceWillSleepNotification object: nil];
        [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver: self
            selector: @selector(receiveWakeNote:)
            name: NSWorkspaceDidWakeNotification object: nil];
        [[NSNotificationCenter defaultCenter] addObserver: self
            selector: @selector(receiveNetworkNote:)
            name: kReachabilityChangedNotification object: nil];
    }
    return self;
}

- (void) receiveSleepNote: (NSNotification*) note
{
    Q_UNUSED(note);
    QMetaObject::invokeMethod(SystemNotifier::instance(), "sleep");
}

- (void) receiveWakeNote: (NSNotification*) note
{
    Q_UNUSED(note);
    QMetaObject::invokeMethod(SystemNotifier::instance(), "wake");
}

- (void) receiveNetworkNote: (NSNotification* )note
{
    Reachability* reachability = [note object];
    NSParameterAssert([reachability isKindOfClass: [Reachability class]]);
    BOOL offline = [reachability connectionRequired];
    QMetaObject::invokeMethod(SystemNotifier::instance(), offline ? "offline" : "online");
}
@end

void SystemNotifier::initialize()
{
    d = new SystemNotifierPrivate;

    d->notifier = [[CocoaSystemNotifier alloc] init];
    CFRetain(d->notifier);
    [d->notifier release];

    d->reachability = [[Reachability reachabilityForInternetConnection] retain];
    [d->reachability startNotifier];
}

void SystemNotifier::uninitialize()
{
    CFRelease(d->notifier);
}
