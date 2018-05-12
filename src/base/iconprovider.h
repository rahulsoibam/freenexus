#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include <QObject>

class QString;

class IconProvider : public QObject
{
    Q_DISABLE_COPY(IconProvider)

public:
    static void initInstance();
    static void freeInstance();
    static IconProvider *instance();

    virtual QString getIconPath(const QString &iconId);

protected:
    explicit IconProvider(QObject *parent = 0);
    ~IconProvider();

    static IconProvider *m_instance;
};

#endif // ICONPROVIDER_H
