#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H
#include <QObject>
#include <QMutex>
#include <QPluginLoader>

class PluginLoader
{
public:
    PluginLoader(const QString &category, const char *interfaceId = 0);
    ~PluginLoader();
    QObjectList instances();
    template <typename T>
    QList<T*> instances();
private:
    void load();
    QStringList pluginList() const;

    QByteArray m_interfaceId;
    QMutex m_mutex;
    QString m_location;
    QList<QPluginLoader*> m_plugins;
};

template <typename T>
Q_INLINE_TEMPLATE QList<T*> PluginLoader::instances()
{
    QMutexLocker locker(&m_mutex);
    Q_UNUSED(locker);

    QList<T*> list;
    for (int i = 0; i < m_plugins.count(); i++) {
        //fast cast and os x workaround
        if (!m_interfaceId.isEmpty())
            list << static_cast<T*>(m_plugins.at(i)->instance());
        T *obj = qobject_cast<T*>(m_plugins.at(i)->instance());
        if (obj)
            list << obj;
    }
    return list;
}

#endif // PLUGINLOADER_H
