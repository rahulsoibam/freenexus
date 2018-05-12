#ifndef SETTINGSSTORAGE_H
#define SETTINGSSTORAGE_H

#include <QObject>
#include <QVariantHash>
#include <QTimer>
#include <QReadWriteLock>

class SettingsStorage: public QObject
{
    Q_OBJECT
    SettingsStorage();
    ~SettingsStorage();

public:
    static void initInstance();
    static void freeInstance();
    static SettingsStorage* instance();

    QVariant loadValue(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void storeValue(const QString &key, const QVariant &value);
    void removeValue(const QString &key);

public slots:
    bool save();

private:
    static SettingsStorage *m_instance;

    QVariantHash m_data;
    bool m_dirty;
    QTimer m_timer;
    mutable QReadWriteLock m_lock;
};

#endif // SETTINGSSTORAGE_H
