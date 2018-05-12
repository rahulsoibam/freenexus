#ifndef COOKIESMODEL_H
#define COOKIESMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QNetworkCookie>

class CookiesModel : public QAbstractItemModel
{
     Q_OBJECT

public:
    enum Column
    {
        COL_DOMAIN,
        COL_PATH,
        COL_NAME,
        COL_VALUE,
        COL_EXPDATE,

        NB_COLUMNS
    };

    explicit CookiesModel(const QList<QNetworkCookie> &cookies, QObject *parent = nullptr);

    QList<QNetworkCookie> cookies() const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    mutable QList<QNetworkCookie> m_cookies;
};

#endif // COOKIESMODEL_H
