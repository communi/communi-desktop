/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "bufferviewex.h"
#include "listview.h"
#include "textinput.h"
#include "topiclabel.h"
#include "textbrowser.h"
#include "commandparser.h"

// TODO:
#include <QPluginLoader>
#include "browserplugin.h"
#include "inputplugin.h"
#include "listplugin.h"
#include "topicplugin.h"

// TODO:
Q_IMPORT_PLUGIN(CompleterPlugin)
Q_IMPORT_PLUGIN(HistoryPlugin)
Q_IMPORT_PLUGIN(MenuPlugin)
Q_IMPORT_PLUGIN(SubjectPlugin)

BufferViewEx::BufferViewEx(QWidget* parent) : BufferView(parent)
{
    CommandParser* parser = new CommandParser(this);
    parser->setTriggers(QStringList("/"));
    parser->setTolerant(true);
    textInput()->setParser(parser);

    // TODO: connect(d.textBrowser, SIGNAL(split(Qt::Orientation)), this, SIGNAL(split(Qt::Orientation)));

    // TODO: move outta here...?
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        BrowserPlugin* browserPlugin = qobject_cast<BrowserPlugin*>(instance);
        if (browserPlugin)
            browserPlugin->initialize(textBrowser());
        InputPlugin* inputPlugin = qobject_cast<InputPlugin*>(instance);
        if (inputPlugin)
            inputPlugin->initialize(textInput());
        ListPlugin* listPlugin = qobject_cast<ListPlugin*>(instance);
        if (listPlugin)
            listPlugin->initialize(listView());
        TopicPlugin* topicPlugin = qobject_cast<TopicPlugin*>(instance);
        if (topicPlugin)
            topicPlugin->initialize(topicLabel());
    }
}
