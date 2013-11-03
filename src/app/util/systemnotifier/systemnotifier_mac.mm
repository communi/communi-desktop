/*
* Copyright (C) 2008-2013 The Communi Project
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

#include "systemnotifier.h"
#include "mac/Reachability.h"
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
    d->reachability = [[Reachability reachabilityForInternetConnection] retain];
    [d->reachability startNotifier];
}

void SystemNotifier::uninitialize()
{
    [d->notifier release];
    [d->reachability release];
    delete d;
}
