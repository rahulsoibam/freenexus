#ifndef LOGLISTWIDGET_H
#define LOGLISTWIDGET_H

#include <QListWidget>
#include "base/logger.h"

QT_BEGIN_NAMESPACE
class QKeyEvent;
QT_END_NAMESPACE

class LogListWidget: public QListWidget
{
    Q_OBJECT

public:
    // -1 is the portable way to have all the bits set
    explicit LogListWidget(int maxLines, const Log::MsgTypes &types = Log::ALL, QWidget *parent = 0);
    void showMsgTypes(const Log::MsgTypes &types);

public slots:
    void appendLine(const QString &line, const Log::MsgType &type);

protected slots:
    void copySelection();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    int m_maxLines;
    Log::MsgTypes m_types;
};

#endif // LOGLISTWIDGET_H
