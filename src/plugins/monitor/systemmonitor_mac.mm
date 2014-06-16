/*
* Copyright (C) 2008-2014 The Communi Project
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "systemmonitor.h"
#include "mac/Reachability.h"
#include <Cocoa/Cocoa.h>

@interface CocoaSystemMonitor : NSObject
{
    SystemMonitor* monitor;
}
@property (assign) SystemMonitor* monitor;
@end

class SystemMonitorPrivate
{
public:
    Reachability* reachability;
    CocoaSystemMonitor* notifier;
};

@implementation CocoaSystemMonitor
@synthesize monitor;
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
        [[NSDistributedNotificationCenter defaultCenter] addObserver: self
            selector: @selector(receiveScreenLockNote:)
            name: @"com.apple.screenIsLocked" object: nil];
        [[NSDistributedNotificationCenter defaultCenter] addObserver: self
            selector: @selector(receiveScreenUnlockNote:)
            name: @"com.apple.screenIsUnlocked" object: nil];
        [[NSDistributedNotificationCenter defaultCenter] addObserver: self
            selector: @selector(receiveScreenSaverStartNote:)
            name: @"com.apple.screensaver.didstart" object: nil];
        [[NSDistributedNotificationCenter defaultCenter] addObserver: self
            selector: @selector(receiveScreenSaverStopNote:)
            name: @"com.apple.screensaver.didstop" object: nil];
    }
    return self;
}

- (void) receiveSleepNote: (NSNotification*) note
{
    Q_UNUSED(note);
    QMetaObject::invokeMethod(monitor, "sleep");
}

- (void) receiveWakeNote: (NSNotification*) note
{
    Q_UNUSED(note);
    QMetaObject::invokeMethod(monitor, "wake");
}

- (void) receiveNetworkNote: (NSNotification*) note
{
    Reachability* reachability = [note object];
    NSParameterAssert([reachability isKindOfClass: [Reachability class]]);
    BOOL offline = [reachability connectionRequired];
    QMetaObject::invokeMethod(monitor, offline ? "offline" : "online");
}

- (void) receiveScreenLockNote: (NSNotification*) note
{
    Q_UNUSED(note);
    QMetaObject::invokeMethod(monitor, "screenLocked");
}

- (void) receiveScreenUnlockNote: (NSNotification*) note
{
    Q_UNUSED(note);
    QMetaObject::invokeMethod(monitor, "screenUnlocked");
}

- (void) receiveScreenSaverStartNote: (NSNotification*) note
{
    Q_UNUSED(note);
    QMetaObject::invokeMethod(monitor, "screenSaverStarted");
}

- (void) receiveScreenSaverStopNote: (NSNotification*) note
{
    Q_UNUSED(note);
    QMetaObject::invokeMethod(monitor, "screenSaverStopped");
}
@end

void SystemMonitor::initialize()
{
    d = new SystemMonitorPrivate;
    d->notifier = [[CocoaSystemMonitor alloc] init];
    [d->notifier setMonitor: this];
    d->reachability = [[Reachability reachabilityForInternetConnection] retain];
    [d->reachability startNotifier];
}

void SystemMonitor::uninitialize()
{
    [d->notifier release];
    [d->reachability release];
    delete d;
}
