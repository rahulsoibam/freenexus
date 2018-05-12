#include "rss_item.h"

#include <QDebug>
#include <QRegularExpression>
#include <QStringList>

using namespace RSS;

const QString Item::PathSeparator("\\");

Item::Item(const QString &path)
    : m_path(path)
{
}

Item::~Item() {}

void Item::setPath(const QString &path)
{
    if (path != m_path) {
        m_path = path;
        emit pathChanged(this);
    }
}

QString Item::path() const
{
    return m_path;
}

QString Item::name() const
{
    return relativeName(path());
}

bool Item::isValidPath(const QString &path)
{
    static const QRegularExpression re(
                QString(R"(\A[^\%1]+(\%1[^\%1]+)*\z)").arg(Item::PathSeparator)
                , QRegularExpression::DontCaptureOption | QRegularExpression::OptimizeOnFirstUsageOption);

    if (path.isEmpty() || !re.match(path).hasMatch()) {
        qDebug() << "Incorrect RSS Item path:" << path;
        return false;
    }

    return true;
}

QString Item::joinPath(const QString &path1, const QString &path2)
{
    if (path1.isEmpty())
        return path2;
    else
        return path1 + Item::PathSeparator + path2;
}

QStringList Item::expandPath(const QString &path)
{
    QStringList result;
    if (path.isEmpty()) return result;
    //    if (!isValidRSSFolderName(folder))
    //        return result;

    int index = 0;
    while ((index = path.indexOf(Item::PathSeparator, index)) >= 0) {
        result << path.left(index);
        ++index;
    }
    result << path;

    return result;
}

QString Item::parentPath(const QString &path)
{
    int pos;
    return ((pos = path.lastIndexOf(Item::PathSeparator)) >= 0 ? path.left(pos) : "");
}

QString Item::relativeName(const QString &path)
{
    int pos;
    return ((pos = path.lastIndexOf(Item::PathSeparator)) >= 0 ? path.right(path.size() - (pos + 1)) : path);
}
