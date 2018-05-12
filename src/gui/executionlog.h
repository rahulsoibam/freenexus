#ifndef EXECUTIONLOG_H
#define EXECUTIONLOG_H

#include <QWidget>
#include "base/logger.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ExecutionLog;
}
QT_END_NAMESPACE
class LogListWidget;

class ExecutionLog: public QWidget
{
    Q_OBJECT

public:
    ExecutionLog(QWidget *parent, const Log::MsgTypes &types);
    void showMsgTypes(const Log::MsgTypes &types);
    ~ExecutionLog();

private slots:
    void addLogMessage(const Log::Msg &msg);
    void addPeerMessage(const Log::Peer &peer);

private:
    Ui::ExecutionLog *ui;

    LogListWidget *m_msgList;
    LogListWidget *m_peerList;
};

#endif // EXECUTIONLOG_H
