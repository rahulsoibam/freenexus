#include <QListWidgetItem>
#include <QLabel>
#include <QDateTime>
#include <QColor>
#include <QPalette>
#include "executionlog.h"
#include "ui_executionlog.h"
#include "guiiconprovider.h"
#include "loglistwidget.h"

ExecutionLog::ExecutionLog(QWidget *parent, const Log::MsgTypes &types)
    : QWidget(parent)
    , ui(new Ui::ExecutionLog)
    , m_peerList(new LogListWidget(MAX_LOG_MESSAGES))
{
    ui->setupUi(this);

    m_msgList = new LogListWidget(MAX_LOG_MESSAGES, Log::MsgTypes(types));

#ifndef Q_OS_MAC
    ui->tabConsole->setTabIcon(0, GuiIconProvider::instance()->getIcon("view-calendar-journal"));
    ui->tabConsole->setTabIcon(1, GuiIconProvider::instance()->getIcon("view-filter"));
#endif
    ui->tabGeneral->layout()->addWidget(m_msgList);
    ui->tabBan->layout()->addWidget(m_peerList);

    const Logger* const logger = Logger::instance();
    foreach (const Log::Msg& msg, logger->getMessages())
        addLogMessage(msg);
    foreach (const Log::Peer& peer, logger->getPeers())
        addPeerMessage(peer);
    connect(logger, SIGNAL(newLogMessage(const Log::Msg &)), SLOT(addLogMessage(const Log::Msg &)));
    connect(logger, SIGNAL(newLogPeer(const Log::Peer &)), SLOT(addPeerMessage(const Log::Peer &)));
}

ExecutionLog::~ExecutionLog()
{
    delete m_msgList;
    delete m_peerList;
    delete ui;
}

void ExecutionLog::showMsgTypes(const Log::MsgTypes &types)
{
    m_msgList->showMsgTypes(types);
}

void ExecutionLog::addLogMessage(const Log::Msg &msg)
{
    QString text;
    QDateTime time = QDateTime::fromMSecsSinceEpoch(msg.timestamp);
    QColor color;

    switch (msg.type) {
    case Log::INFO:
        color.setNamedColor("blue");
        break;
    case Log::WARNING:
        color.setNamedColor("orange");
        break;
    case Log::CRITICAL:
        color.setNamedColor("red");
        break;
    default:
        color = QApplication::palette().color(QPalette::WindowText);
    }

    text = "<font color='grey'>" + time.toString(Qt::SystemLocaleShortDate) + "</font> - <font color='" + color.name() + "'>" + msg.message + "</font>";
    m_msgList->appendLine(text, msg.type);
}

void ExecutionLog::addPeerMessage(const Log::Peer& peer)
{
    QString text;
    QDateTime time = QDateTime::fromMSecsSinceEpoch(peer.timestamp);

    if (peer.blocked)
        text = "<font color='grey'>" + time.toString(Qt::SystemLocaleShortDate) + "</font> - "
            + tr("<font color='red'>%1</font> was blocked %2", "x.y.z.w was blocked").arg(peer.ip, peer.reason);
    else
        text = "<font color='grey'>" + time.toString(Qt::SystemLocaleShortDate) + "</font> - " + tr("<font color='red'>%1</font> was banned", "x.y.z.w was banned").arg(peer.ip);

    m_peerList->appendLine(text, Log::NORMAL);
}
