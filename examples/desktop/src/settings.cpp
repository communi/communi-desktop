/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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

#include "settings.h"
#include <QApplication>

Settings::Settings() : maxBlockCount(-1), timeStamp(true)
{
#if defined(Q_WS_MAC)
    shortcuts[TabUp] = QApplication::translate("Shortcuts", "Ctrl+Alt+Up");
    shortcuts[TabDown] = QApplication::translate("Shortcuts", "Ctrl+Alt+Down");
    shortcuts[TabLeft] = QApplication::translate("Shortcuts", "Ctrl+Alt+Left");
    shortcuts[TabRight] = QApplication::translate("Shortcuts", "Ctrl+Alt+Right");
#else
    shortcuts[TabUp] = QApplication::translate("Shortcuts", "Alt+Up");
    shortcuts[TabDown] = QApplication::translate("Shortcuts", "Alt+Down");
    shortcuts[TabLeft] = QApplication::translate("Shortcuts", "Alt+Left");
    shortcuts[TabRight] = QApplication::translate("Shortcuts", "Alt+Right");
#endif // Q_WS_XXX

    messages[Joins] = true;
    messages[Parts] = true;
    messages[Nicks] = true;
    messages[Modes] = true;
    messages[Kicks] = true;
    messages[Quits] = true;
    messages[Topics] = true;

    highlights[Joins] = false;
    highlights[Parts] = false;
    highlights[Nicks] = false;
    highlights[Modes] = false;
    highlights[Kicks] = false;
    highlights[Quits] = false;
    highlights[Topics] = false;

    // TODO: the default values should respect palette
    colors[Background] = "white";
    colors[Message] = "black";
    colors[Event] = "gray";
    colors[Notice] = "brown";
    colors[Action] = "darkmagenta";
    colors[Highlight] = "red";
    colors[TimeStamp] = "gray";
}

Settings::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

bool Settings::operator==(const Settings& other) const
{
    return messages == other.messages && highlights == other.highlights
        && language == other.language && font == other.font && colors == other.colors
        && shortcuts == other.shortcuts && maxBlockCount == other.maxBlockCount
        && timeStamp == other.timeStamp;
}

bool Settings::operator!=(const Settings& other) const
{
    return !(*this == other);
}
