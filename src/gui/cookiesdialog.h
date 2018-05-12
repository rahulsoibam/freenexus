#ifndef COOKIESDIALOG_H
#define COOKIESDIALOG_H

#include <QDialog>

namespace Ui
{
    class CookiesDialog;
}

class CookiesModel;

class CookiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CookiesDialog(QWidget *parent = nullptr);
    ~CookiesDialog();

public slots:
    void accept() override;

private slots:
    void onButtonAddClicked();
    void onButtonDeleteClicked();

private:
    Ui::CookiesDialog *m_ui;
    CookiesModel *m_cookiesModel;
};

#endif // COOKIESDIALOG_H
