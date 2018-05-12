#ifndef TAGFILTERPROXYMODEL_H
#define TAGFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QString>

class TagFilterProxyModel: public QSortFilterProxyModel
{
public:
    explicit TagFilterProxyModel(QObject *parent = nullptr);

    // TagFilterModel methods which we need to relay
    QModelIndex index(const QString &tag) const;
    QString tag(const QModelIndex &index) const;

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    // we added another overload of index(), hence this using directive:
    using QSortFilterProxyModel::index;
};

#endif // TAGFILTERPROXYMODEL_H
