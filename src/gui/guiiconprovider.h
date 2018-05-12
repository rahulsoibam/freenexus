#ifndef GUIICONPROVIDER_H
#define GUIICONPROVIDER_H

#include "base/iconprovider.h"

class QIcon;

class GuiIconProvider : public IconProvider
{
    Q_DISABLE_COPY(GuiIconProvider)
    Q_OBJECT

public:
    static void initInstance();
    static GuiIconProvider *instance();

    QIcon getIcon(const QString &iconId);
    QIcon getIcon(const QString &iconId, const QString &fallback);
    QIcon getFlagIcon(const QString &countryIsoCode);
    QString getIconPath(const QString &iconId);

private slots:
    void configure();

private:
    explicit GuiIconProvider(QObject *parent = 0);
    ~GuiIconProvider();
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MAC))
    QIcon generateDifferentSizes(const QIcon &icon);

    bool m_useSystemTheme;
#endif
};

#endif // GUIICONPROVIDER_H
