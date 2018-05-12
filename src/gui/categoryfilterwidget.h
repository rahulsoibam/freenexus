 #include <QTreeView>

class CategoryFilterWidget: public QTreeView
{
    Q_OBJECT
    Q_DISABLE_COPY(CategoryFilterWidget)

public:
    explicit CategoryFilterWidget(QWidget *parent = nullptr);

    QString currentCategory() const;

signals:
    void categoryChanged(const QString &categoryName);
    void actionResumeTorrentsTriggered();
    void actionPauseTorrentsTriggered();
    void actionDeleteTorrentsTriggered();

private slots:
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void showMenu(QPoint);
    void callUpdateGeometry();
    void addCategory();
    void addSubcategory();
    void editCategory();
    void removeCategory();
    void removeUnusedCategories();

private:
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void rowsInserted(const QModelIndex &parent, int start, int end) override;

    int m_defaultIndentation;
};
