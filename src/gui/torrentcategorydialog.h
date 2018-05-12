#pragma once

#include <QDialog>

namespace Ui
{
    class TorrentCategoryDialog;
}

class TorrentCategoryDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(TorrentCategoryDialog)

public:
    static QString createCategory(QWidget *parent, const QString &parentCategoryName = QString());
    static void editCategory(QWidget *parent, const QString &categoryName);

    explicit TorrentCategoryDialog(QWidget *parent = nullptr);
    ~TorrentCategoryDialog() override;

    void setCategoryNameEditable(bool editable);
    QString categoryName() const;
    void setCategoryName(const QString &categoryName);
    QString savePath() const;
    void setSavePath(const QString &savePath);

private:
    Ui::TorrentCategoryDialog *m_ui;
};
