/*
  Copyright (C) 2008-2016 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
