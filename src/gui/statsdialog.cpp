#include "statsdialog.h"

#include "base/bittorrent/cachestatus.h"
#include "base/bittorrent/session.h"
#include "base/bittorrent/sessionstatus.h"
#include "base/bittorrent/torrenthandle.h"
#include "base/utils/misc.h"
#include "base/utils/string.h"
#include "ui_statsdialog.h"
#include "utils.h"

StatsDialog::StatsDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::StatsDialog)
{
    m_ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &StatsDialog::close);

    update();
    connect(BitTorrent::Session::instance(), &BitTorrent::Session::statsUpdated
            , this, &StatsDialog::update);

    Utils::Gui::resize(this);
    show();
}

StatsDialog::~StatsDialog()
{
    delete m_ui;
}

void StatsDialog::update()
{
    const BitTorrent::SessionStatus &ss = BitTorrent::Session::instance()->status();
    const BitTorrent::CacheStatus &cs = BitTorrent::Session::instance()->cacheStatus();

    // All-time DL/UL
    quint64 atd = BitTorrent::Session::instance()->getAlltimeDL();
    quint64 atu = BitTorrent::Session::instance()->getAlltimeUL();
    m_ui->labelAlltimeDL->setText(Utils::Misc::friendlyUnit(atd));
    m_ui->labelAlltimeUL->setText(Utils::Misc::friendlyUnit(atu));
    // Total waste (this session)
    m_ui->labelWaste->setText(Utils::Misc::friendlyUnit(ss.totalWasted));
    // Global ratio
    m_ui->labelGlobalRatio->setText(
                ((atd > 0) && (atu > 0))
                ? Utils::String::fromDouble(static_cast<qreal>(atu) / atd, 2)
                : "-");
    // Cache hits
    qreal readRatio = cs.readRatio;
    m_ui->labelCacheHits->setText(QString("%1%").arg(
        readRatio > 0
        ? Utils::String::fromDouble(100 * readRatio, 2)
        : "0"));
    // Buffers size
    m_ui->labelTotalBuf->setText(Utils::Misc::friendlyUnit(cs.totalUsedBuffers * 16 * 1024));
    // Disk overload (100%) equivalent
    // From lt manual: disk_write_queue and disk_read_queue are the number of peers currently waiting on a disk write or disk read
    // to complete before it receives or sends any more data on the socket. It's a metric of how disk bound you are.

    // num_peers is not reliable (adds up peers, which didn't even overcome tcp handshake)
    quint32 peers = 0;
    foreach (BitTorrent::TorrentHandle *const torrent, BitTorrent::Session::instance()->torrents())
        peers += torrent->peersCount();

    m_ui->labelWriteStarve->setText(QString("%1%")
                                    .arg(((ss.diskWriteQueue > 0) && (peers > 0))
                                         ? Utils::String::fromDouble((100. * ss.diskWriteQueue) / peers, 2)
                                         : "0"));
    m_ui->labelReadStarve->setText(QString("%1%")
                                   .arg(((ss.diskReadQueue > 0) && (peers > 0))
                                        ? Utils::String::fromDouble((100. * ss.diskReadQueue) / peers, 2)
                                        : "0"));
    // Disk queues
    m_ui->labelQueuedJobs->setText(QString::number(cs.jobQueueLength));
    m_ui->labelJobsTime->setText(tr("%1 ms", "18 milliseconds").arg(cs.averageJobTime));
    m_ui->labelQueuedBytes->setText(Utils::Misc::friendlyUnit(cs.queuedBytes));

    // Total connected peers
    m_ui->labelPeers->setText(QString::number(ss.peersCount));
}
