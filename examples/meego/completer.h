#ifndef COMPLETER_H
#define COMPLETER_H

#include <QObject>
#include <QStringList>
class AbstractSessionItem;

class Completer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* modelItem READ modelItem WRITE setModelItem)

public:
    explicit Completer(QObject *parent = 0);

    QObject* modelItem() const;
    void setModelItem(QObject* item);

public slots:
    void complete(const QString& text, int selStart, int selEnd);

signals:
    void completed(const QString& text, int selStart, int selEnd);

private:
    static QString findWord(const QString& text, int selStart, int selEnd, int* wordStart, int* wordEnd);
    static QStringList filterList(const QStringList& list, const QString& prefix);

    AbstractSessionItem* m_item;
    QStringList m_candidates;
    int m_current;
};

#endif // COMPLETER_H
