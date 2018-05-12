#ifndef QBT_GUI_FSPATHEDIT_P_H
#define QBT_GUI_FSPATHEDIT_P_H

#include <QAction>
#include <QComboBox>
#include <QCompleter>
#include <QContextMenuEvent>
#include <QDir>
#include <QFileIconProvider>
#include <QFileSystemModel>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QStringRef>
#include <QValidator>
#include <QVector>

class QStringList;

namespace Private
{
    class FileSystemPathValidator: public QValidator
    {
        Q_OBJECT

    public:
        FileSystemPathValidator(QObject *parent = nullptr);

        bool strictMode() const;
        void setStrictMode(bool v);

        bool existingOnly() const;
        void setExistingOnly(bool v);

        bool directoriesOnly() const;
        void setDirectoriesOnly(bool v);

        bool checkReadPermission() const;
        void setCheckReadPermission(bool v);

        bool checkWritePermission() const;
        void setCheckWritePermission(bool v);

        QValidator::State validate(QString &input, int &pos) const override;

        enum class TestResult
        {
            OK,
            DoesNotExist,
            NotADir,
            NotAFile,
            CantRead,
            CantWrite
        };

        TestResult lastTestResult() const;
        QValidator::State lastValidationState() const;
        QString lastTestedPath() const;

    private:
        QValidator::State validate(const QString &path, const QVector<QStringRef> &pathComponents, bool strict,
                                   int firstComponentToTest, int lastComponentToTest) const;

        TestResult testPath(const QStringRef &path, bool pathIsComplete) const;

        bool m_strictMode;
        bool m_existingOnly;
        bool m_directoriesOnly;
        bool m_checkReadPermission;
        bool m_checkWritePermission;

        mutable TestResult m_lastTestResult;
        mutable QValidator::State m_lastValidationState;
        mutable QString m_lastTestedPath;
    };

    class FileEditorWithCompletion
    {
    public:
        virtual ~FileEditorWithCompletion() = default;
        virtual void completeDirectoriesOnly(bool completeDirsOnly) = 0;
        virtual void setFilenameFilters(const QStringList &filters) = 0;
        virtual void setBrowseAction(QAction *action) = 0;
        virtual void setValidator(QValidator *validator) = 0;
        virtual QWidget *widget() = 0;
    };

    class FileLineEdit: public QLineEdit, public FileEditorWithCompletion
    {
        Q_OBJECT
        Q_DISABLE_COPY(FileLineEdit)

    public:
        FileLineEdit(QWidget *parent = nullptr);
        ~FileLineEdit();

        void completeDirectoriesOnly(bool completeDirsOnly) override;
        void setFilenameFilters(const QStringList &filters) override;
        void setBrowseAction(QAction *action) override;
        void setValidator(QValidator *validator) override;
        QWidget *widget() override;

    protected:
        void keyPressEvent(QKeyEvent *event) override;
        void contextMenuEvent(QContextMenuEvent *event) override;

    private:
        static QString warningText(FileSystemPathValidator::TestResult r);
        void showCompletionPopup();

        QFileSystemModel *m_completerModel;
        QCompleter *m_completer;
        QAction *m_browseAction;
        QFileIconProvider m_iconProvider;
        QAction *m_warningAction;
    };

    class FileComboEdit: public QComboBox, public FileEditorWithCompletion
    {
        Q_OBJECT

    public:
        FileComboEdit(QWidget *parent = nullptr);

        void completeDirectoriesOnly(bool completeDirsOnly) override;
        void setFilenameFilters(const QStringList &filters) override;
        void setBrowseAction(QAction *action) override;
        void setValidator(QValidator *validator) override;
        QWidget *widget() override;

    protected:
        QString text() const;
    };
}

#endif // QBT_GUI_FSPATHEDIT_P_H
