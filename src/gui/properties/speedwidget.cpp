#include "speedwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QSignalMapper>
#include <QThread>
#include <QTimer>

#include <libtorrent/session_status.hpp>

#include "base/bittorrent/session.h"
#include "base/bittorrent/sessionstatus.h"
#include "base/preferences.h"
#include "propertieswidget.h"

ComboBoxMenuButton::ComboBoxMenuButton(QWidget *parent, QMenu *menu)
    : QComboBox(parent)
    , m_menu(menu)
{
}

void ComboBoxMenuButton::showPopup()
{
    QPoint p = mapToGlobal(QPoint(0, height()));
    m_menu->exec(p);
    QComboBox::hidePopup();
}

SpeedWidget::SpeedWidget(PropertiesWidget *parent)
    : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(3);

    m_hlayout = new QHBoxLayout();
    m_hlayout->setContentsMargins(0, 0, 0, 0);

    m_periodLabel = new QLabel("<b>" + tr("Period:") + "</b>");

    m_periodCombobox = new QComboBox();
    m_periodCombobox->addItem(tr("1 Minute"));
    m_periodCombobox->addItem(tr("5 Minutes"));
    m_periodCombobox->addItem(tr("30 Minutes"));
    m_periodCombobox->addItem(tr("6 Hours"));

    connect(m_periodCombobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
        , this, &SpeedWidget::onPeriodChange);

    m_graphsMenu = new QMenu(this);
    m_graphsMenu->addAction(tr("Total Upload"));
    m_graphsMenu->addAction(tr("Total Download"));
    m_graphsMenu->addAction(tr("Payload Upload"));
    m_graphsMenu->addAction(tr("Payload Download"));
    m_graphsMenu->addAction(tr("Overhead Upload"));
    m_graphsMenu->addAction(tr("Overhead Download"));
    m_graphsMenu->addAction(tr("DHT Upload"));
    m_graphsMenu->addAction(tr("DHT Download"));
    m_graphsMenu->addAction(tr("Tracker Upload"));
    m_graphsMenu->addAction(tr("Tracker Download"));

    m_graphsMenuActions = m_graphsMenu->actions();
    m_graphsSignalMapper = new QSignalMapper(this);

    for (int id = SpeedPlotView::UP; id < SpeedPlotView::NB_GRAPHS; ++id) {
        QAction *action = m_graphsMenuActions.at(id);
        action->setCheckable(true);
        action->setChecked(true);
        connect(action, &QAction::changed, m_graphsSignalMapper
            , static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        m_graphsSignalMapper->setMapping(action, id);
    }
    connect(m_graphsSignalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped)
        , this, &SpeedWidget::onGraphChange);

    m_graphsButton = new ComboBoxMenuButton(this, m_graphsMenu);
    m_graphsButton->addItem(tr("Select Graphs"));

    m_hlayout->addWidget(m_periodLabel);
    m_hlayout->addWidget(m_periodCombobox);
    m_hlayout->addStretch();
    m_hlayout->addWidget(m_graphsButton);

    m_plot = new SpeedPlotView(this);

    m_layout->addLayout(m_hlayout);
    m_layout->addWidget(m_plot);

    loadSettings();

    QTimer *localUpdateTimer = new QTimer(this);
    connect(localUpdateTimer, &QTimer::timeout, this, &SpeedWidget::update);
    localUpdateTimer->start(1000);

    m_plot->show();
}

SpeedWidget::~SpeedWidget()
{
    qDebug("SpeedWidget::~SpeedWidget() ENTER");
    saveSettings();
    qDebug("SpeedWidget::~SpeedWidget() EXIT");
}

void SpeedWidget::update()
{
    const BitTorrent::SessionStatus &btStatus = BitTorrent::Session::instance()->status();

    SpeedPlotView::PointData point;
    point.x = QDateTime::currentMSecsSinceEpoch() / 1000;
    point.y[SpeedPlotView::UP] = btStatus.uploadRate;
    point.y[SpeedPlotView::DOWN] = btStatus.downloadRate;
    point.y[SpeedPlotView::PAYLOAD_UP] = btStatus.payloadUploadRate;
    point.y[SpeedPlotView::PAYLOAD_DOWN] = btStatus.payloadDownloadRate;
    point.y[SpeedPlotView::OVERHEAD_UP] = btStatus.ipOverheadUploadRate;
    point.y[SpeedPlotView::OVERHEAD_DOWN] = btStatus.ipOverheadDownloadRate;
    point.y[SpeedPlotView::DHT_UP] = btStatus.dhtUploadRate;
    point.y[SpeedPlotView::DHT_DOWN] = btStatus.dhtDownloadRate;
    point.y[SpeedPlotView::TRACKER_UP] = btStatus.trackerUploadRate;
    point.y[SpeedPlotView::TRACKER_DOWN] = btStatus.trackerDownloadRate;

    m_plot->pushPoint(point);
    m_plot->replot();
}

void SpeedWidget::onPeriodChange(int period)
{
    m_plot->setViewableLastPoints(static_cast<SpeedPlotView::TimePeriod>(period));
}

void SpeedWidget::onGraphChange(int id)
{
    QAction *action = m_graphsMenuActions.at(id);
    m_plot->setGraphEnable(static_cast<SpeedPlotView::GraphID>(id), action->isChecked());
}

void SpeedWidget::loadSettings()
{
    Preferences *preferences = Preferences::instance();

    int periodIndex = preferences->getSpeedWidgetPeriod();
    m_periodCombobox->setCurrentIndex(periodIndex);
    onPeriodChange(static_cast<SpeedPlotView::TimePeriod>(periodIndex));

    for (int id = SpeedPlotView::UP; id < SpeedPlotView::NB_GRAPHS; ++id) {
        QAction *action = m_graphsMenuActions.at(id);
        bool enable = preferences->getSpeedWidgetGraphEnable(id);

        action->setChecked(enable);
        m_plot->setGraphEnable(static_cast<SpeedPlotView::GraphID>(id), enable);
    }
}

void SpeedWidget::saveSettings() const
{
    Preferences *preferences = Preferences::instance();

    preferences->setSpeedWidgetPeriod(m_periodCombobox->currentIndex());

    for (int id = SpeedPlotView::UP; id < SpeedPlotView::NB_GRAPHS; ++id) {
        QAction *action = m_graphsMenuActions.at(id);
        preferences->setSpeedWidgetGraphEnable(id, action->isChecked());
    }
}