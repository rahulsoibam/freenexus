#ifndef QBT_FSPATHEDIT_H
#define QBT_FSPATHEDIT_H

#include <QScopedPointer>
#include <QWidget>

namespace Private
{
    class FileEditorWithCompletion;
    class FileLineEdit;
    class FileComboEdit;
}

/*!
 * \brief
 * Widget for editing strings which are paths in filesystem
 */
class FileSystemPathEdit: public QWidget
{
    Q_OBJECT
    Q_ENUMS(Mode)
    Q_PROPERTY(Mode mode READ mode WRITE setMode)
    Q_PROPERTY(QString selectedPath READ selectedPath WRITE setSelectedPath NOTIFY selectedPathChanged)
    Q_PROPERTY(QString fileNameFilter READ fileNameFilter WRITE setFileNameFilter)
    Q_PROPERTY(QString dialogCaption READ dialogCaption WRITE setDialogCaption)

public:
    ~FileSystemPathEdit() override;

    enum class Mode
    {
        FileOpen,        //!< opening files, shows open file dialog
        FileSave,        //!< saving files, shows save file dialog
        DirectoryOpen,   //!< selecting existing directories
        DirectorySave    //!< selecting directories for saving
    };

    Mode mode() const;
    void setMode(Mode mode);

    QString currentDirectory() const;
    QString selectedPath() const;
    void setSelectedPath(const QString &val);

    QString fileNameFilter() const;
    void setFileNameFilter(const QString &val);

    /// The browse button caption is "..." if true, and "Browse" otherwise
    bool briefBrowseButtonCaption() const;
    void setBriefBrowseButtonCaption(bool brief);

    QString dialogCaption() const;
    void setDialogCaption(const QString &caption);

    virtual void clear() = 0;

signals:
    void selectedPathChanged(const QString &path);

protected:
    explicit FileSystemPathEdit(Private::FileEditorWithCompletion *editor, QWidget *parent);

    template <class Widget>
    Widget *editWidget() const
    {
        return static_cast<Widget *>(editWidgetImpl());
    }

protected slots:
    void onPathEdited();

private:
    virtual QString editWidgetText() const = 0;
    virtual void setEditWidgetText(const QString &text) = 0;

    QWidget *editWidgetImpl() const;
    Q_DISABLE_COPY(FileSystemPathEdit)
    class FileSystemPathEditPrivate;
    Q_DECLARE_PRIVATE(FileSystemPathEdit)
    QScopedPointer<FileSystemPathEditPrivate> const d_ptr;
};

/// Widget which uses QLineEdit for path editing
class FileSystemPathLineEdit: public FileSystemPathEdit
{
    using base = FileSystemPathEdit;
    using WidgetType = Private::FileLineEdit;

public:
    explicit FileSystemPathLineEdit(QWidget *parent = nullptr);

    void clear() override;

private:
    QString editWidgetText() const override;
    void setEditWidgetText(const QString &text) override;
};

/// Widget which uses QComboBox for path editing
class FileSystemPathComboEdit: public FileSystemPathEdit
{
    using base = FileSystemPathEdit;
    using WidgetType = Private::FileComboEdit;

public:
    explicit FileSystemPathComboEdit(QWidget *parent = nullptr);

    void clear() override;

    int count() const;
    QString item(int index) const;
    void addItem(const QString &text);
    void insertItem(int index, const QString &text);

    int currentIndex() const;
    void setCurrentIndex(int index);

    int maxVisibleItems() const;
    void setMaxVisibleItems(int maxItems);

private:
    QString editWidgetText() const override;
    void setEditWidgetText(const QString &text) override;
};

#endif // QBT_FSPATHEDIT_H
