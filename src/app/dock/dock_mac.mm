/*
  Copyright (C) 2008-2015 The Communi Project

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

#include "dock.h"
#include <Cocoa/Cocoa.h>

@interface InterfaceStyleListener : NSObject
{
    Dock* dock;
}
@property (assign) Dock* dock;
@end

static InterfaceStyleListener* dock_listener = 0;

@implementation InterfaceStyleListener
@synthesize dock;
- (id)init
{
    self = [super init];
    if (self) {
        [[NSDistributedNotificationCenter defaultCenter] addObserver: self
            selector:@selector(receiveThemeNote:)
            name:@"AppleInterfaceThemeChangedNotification" object:nil];
    }
    return self;
}

- (void) receiveThemeNote: (NSNotification*) note
{
    Q_UNUSED(note);
    dock->init();
    QMetaObject::invokeMethod(dock, "updateTray");
}
@end

void Dock::init()
{
    if (!dock_listener) {
        dock_listener = [[InterfaceStyleListener alloc] init];
        [dock_listener setDock: this];
    }

    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    NSString* style = [defaults stringForKey:@"AppleInterfaceStyle"];
    if (style) {
        d.alertIcon.addFile(":/images/tray/white/blue.png");
        d.onlineIcon.addFile(":/images/tray/white/transparent.png");
        d.offlineIcon.addFile(":/images/tray/white/black.png");
    } else {
        d.alertIcon.addFile(":/images/tray/black/blue.png");
        d.onlineIcon.addFile(":/images/tray/black/black.png");
        d.offlineIcon.addFile(":/images/tray/black/transparent.png");
    }
}

void Dock::uninit()
{
    [dock_listener release];
}
