#include "autoexpandabledialog.h"

#include <QDesktopWidget>

#include "mainwindow.h"
#include "ui_autoexpandabledialog.h"
#include "utils.h"

AutoExpandableDialog::AutoExpandableDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::AutoExpandableDialog)
{
    m_ui->setupUi(this);
}

AutoExpandableDialog::~AutoExpandableDialog()
{
    delete m_ui;
}

QString AutoExpandableDialog::getText(QWidget *parent, const QString &title, const QString &label,
                                      QLineEdit::EchoMode mode, const QString &text,
                                      bool *ok, Qt::InputMethodHints inputMethodHints)
{
    AutoExpandableDialog d(parent);
    d.setWindowTitle(title);
    d.m_ui->textLabel->setText(label);
    d.m_ui->textEdit->setText(text);
    d.m_ui->textEdit->setEchoMode(mode);
    d.m_ui->textEdit->setInputMethodHints(inputMethodHints);

    bool res = d.exec();
    if (ok)
        *ok = res;

    if (!res) return QString();

    return d.m_ui->textEdit->text();
}

void AutoExpandableDialog::showEvent(QShowEvent *e)
{
    // Overriding showEvent is required for consistent UI with fixed size under custom DPI
    QDialog::showEvent(e);

    // Show dialog and resize textbox to fit the text
    // NOTE: For unknown reason QFontMetrics gets more accurate when called from showEvent.
    int wd = m_ui->textEdit->fontMetrics().width(m_ui->textEdit->text()) + 4;

    if (!windowTitle().isEmpty()) {
        // not really the font metrics in window title, so we enlarge it a bit,
        // including the small icon and close button width
        int w = fontMetrics().width(windowTitle()) * 1.8;
        wd = std::max(wd, w);
    }

    if (!m_ui->textLabel->text().isEmpty()) {
        int w = m_ui->textLabel->fontMetrics().width(m_ui->textLabel->text());
        wd = std::max(wd, w);
    }

    // Now resize the dialog to fit the contents
    // max width of text from either of: label, title, textedit
    // If the value is less than dialog default size, default size is used
    if (wd > width()) {
        QSize size = {width() - m_ui->verticalLayout->sizeHint().width() + wd, height()};
        Utils::Gui::resize(this, size);
    }
}
