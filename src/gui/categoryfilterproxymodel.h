#ifndef CATEGORYFILTERPROXYMODEL_H
#define CATEGORYFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QString>

class CategoryFilterProxyModel: public QSortFilterProxyModel
{
public:
    explicit CategoryFilterProxyModel(QObject *parent = nullptr);

    // CategoryFilterModel methods which we need to relay
    QModelIndex index(const QString &categoryName) const;
    QString categoryName(const QModelIndex &index) const;

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    // we added another overload of index(), hence this using directive:
    using QSortFilterProxyModel::index;
};

#endif // CATEGORYFILTERPROXYMODEL_H
