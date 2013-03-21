#include "powernotifier.h"

#include <Cocoa/Cocoa.h>

@interface CocoaPowerNotifier : NSObject
@end

@implementation CocoaPowerNotifier
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
    QMetaObject::invokeMethod(PowerNotifier::instance(), "sleep");
}

- (void) receiveWakeNote: (NSNotification*) note
{
    Q_UNUSED(note);
    QMetaObject::invokeMethod(PowerNotifier::instance(), "wake");
}
@end

void PowerNotifier::initialize()
{
    d = [[CocoaPowerNotifier alloc] init];
    CFRetain(d);
    [static_cast<CocoaPowerNotifier *>(d) release];
}

void PowerNotifier::uninitialize()
{
    CFRelease(d);
}
