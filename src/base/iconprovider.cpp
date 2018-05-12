#include <QString>
#include "iconprovider.h"

IconProvider::IconProvider(QObject *parent)
    : QObject(parent)
{
}

IconProvider::~IconProvider() {}

void IconProvider::initInstance()
{
    if (!m_instance)
        m_instance = new IconProvider;
}

void IconProvider::freeInstance()
{
    if (m_instance) {
        delete m_instance;
        m_instance = 0;
    }
}

IconProvider *IconProvider::instance()
{
    return m_instance;
}

QString IconProvider::getIconPath(const QString &iconId)
{
    return ":/icons/qbt-theme/" + iconId + ".png";
}

IconProvider *IconProvider::m_instance = 0;
