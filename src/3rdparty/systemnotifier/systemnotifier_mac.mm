#include "systemnotifier.h"

#include <Cocoa/Cocoa.h>

@interface CocoaSystemNotifier : NSObject
@end

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
@end

void SystemNotifier::initialize()
{
    d = [[CocoaSystemNotifier alloc] init];
    CFRetain(d);
    [static_cast<CocoaSystemNotifier *>(d) release];
}

void SystemNotifier::uninitialize()
{
    CFRelease(d);
}
