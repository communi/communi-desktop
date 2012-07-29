#include "pluginloader_p.h"
#include <QCoreApplication>
#include <QDir>
#include <QSet>
#include <QDebug>

/*!
	\class PluginLoader
	\internal
 */



PluginLoader::PluginLoader(const QString &category, const char *interfaceId) :
	m_interfaceId(interfaceId),
	m_location(QLatin1Char('/') + category + QLatin1Char('/'))
{
	load();
}

PluginLoader::~PluginLoader()
{
	for (int i = 0; i < m_plugins.count(); i++) {
		m_plugins.at(i)->unload();
		delete m_plugins.at(i);
	}
}

QObjectList PluginLoader::instances()
{
    QMutexLocker locker(&m_mutex);
    Q_UNUSED(locker);

    QObjectList list;
    for (int i = 0; i < m_plugins.count(); i++) {
        QObject *obj = m_plugins.at(i)->instance();
        if (obj)
            list << obj;
    }
    return list;
}

void PluginLoader::load()
{
#if !defined QT_NO_DEBUG
	const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
#endif

	if (!m_plugins.isEmpty())
		return;

	QStringList plugins = pluginList();

	for (int i=0; i < plugins.count(); i++) {
		QPluginLoader* loader = new QPluginLoader(plugins.at(i));
		QObject *o = loader->instance();
		if (o && (m_interfaceId.isEmpty() || o->qt_metacast(m_interfaceId))) {
            m_plugins.append(loader);
		} else {
#if !defined QT_NO_DEBUG
			if (showDebug)
				qWarning() << "QAudioPluginLoader: Failed to load plugin: "
						   << plugins.at(i) << loader->errorString();
#endif
			delete o;
			delete loader;
		}
	}
}

QStringList PluginLoader::pluginList() const
{
#if !defined QT_NO_DEBUG
	const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
#endif

	QStringList paths = QCoreApplication::libraryPaths();
#ifdef QTM_PLUGIN_PATH
	paths << QLatin1String(QTM_PLUGIN_PATH);
#endif
#if !defined QT_NO_DEBUG
	if (showDebug)
		qDebug() << "Plugin paths:" << paths;
#endif

	//temp variable to avoid multiple identic path
	QSet<QString> processed;

	/* Discover a bunch o plugins */
	QStringList pluginList;

	/* Enumerate our plugin paths */
	for (int i=0; i < paths.count(); i++) {
		if (processed.contains(paths.at(i)))
			continue;
		processed.insert(paths.at(i));
		QDir pluginsDir(paths.at(i)+m_location);

		if (!pluginsDir.exists())
			continue;

		QStringList files = pluginsDir.entryList(QDir::Files);
#if !defined QT_NO_DEBUG
		if (showDebug)
			qDebug() << "Looking for plugins in " << pluginsDir.path() << files;
#endif
		for (int j=0; j < files.count(); j++) {
			const QString &file = files.at(j);
			pluginList <<  pluginsDir.absoluteFilePath(file);
		}
	}
	return  pluginList;
}

