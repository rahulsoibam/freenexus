#include "proptabbar.h"

#include <QButtonGroup>
#include <QKeySequence>
#include <QPushButton>
#include <QSpacerItem>

#include "guiiconprovider.h"

PropTabBar::PropTabBar(QWidget *parent)
    : QHBoxLayout(parent)
    , m_currentIndex(-1)
{
    setAlignment(Qt::AlignLeft | Qt::AlignCenter);
    setSpacing(3);
    m_btnGroup = new QButtonGroup(this);
    // General tab
    QPushButton *mainInfosButton = new QPushButton(
#ifndef Q_OS_MAC
            GuiIconProvider::instance()->getIcon("document-properties"),
#endif
            tr("General"), parent);
    mainInfosButton->setShortcut(Qt::ALT + Qt::Key_G);
    addWidget(mainInfosButton);
    m_btnGroup->addButton(mainInfosButton, MainTab);
    // Trackers tab
    QPushButton *trackersButton = new QPushButton(
#ifndef Q_OS_MAC
            GuiIconProvider::instance()->getIcon("network-server"),
#endif
            tr("Trackers"), parent);
    trackersButton->setShortcut(Qt::ALT + Qt::Key_C);
    addWidget(trackersButton);
    m_btnGroup->addButton(trackersButton, TrackersTab);
    // Peers tab
    QPushButton *peersButton = new QPushButton(
#ifndef Q_OS_MAC
            GuiIconProvider::instance()->getIcon("edit-find-user"),
#endif
            tr("Peers"), parent);
    peersButton->setShortcut(Qt::ALT + Qt::Key_R);
    addWidget(peersButton);
    m_btnGroup->addButton(peersButton, PeersTab);
    // URL seeds tab
    QPushButton *URLSeedsButton = new QPushButton(
#ifndef Q_OS_MAC
            GuiIconProvider::instance()->getIcon("network-server"),
#endif
            tr("HTTP Sources"), parent);
    URLSeedsButton->setShortcut(Qt::ALT + Qt::Key_B);
    addWidget(URLSeedsButton);
    m_btnGroup->addButton(URLSeedsButton, URLSeedsTab);
    // Files tab
    QPushButton *filesButton = new QPushButton(
#ifndef Q_OS_MAC
            GuiIconProvider::instance()->getIcon("inode-directory"),
#endif
            tr("Content"), parent);
    filesButton->setShortcut(Qt::ALT + Qt::Key_Z);
    addWidget(filesButton);
    m_btnGroup->addButton(filesButton, FilesTab);
    // Spacer
    addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
    // Speed tab
    QPushButton *speedButton = new QPushButton(
#ifndef Q_OS_MAC
            GuiIconProvider::instance()->getIcon("office-chart-line"),
#endif
            tr("Speed"), parent);
    speedButton->setShortcut(Qt::ALT + Qt::Key_D);
    addWidget(speedButton);
    m_btnGroup->addButton(speedButton, SpeedTab);
    // SIGNAL/SLOT
    connect(m_btnGroup, SIGNAL(buttonClicked(int)), SLOT(setCurrentIndex(int)));
    // Disable buttons focus
    foreach (QAbstractButton *btn, m_btnGroup->buttons())
        btn->setFocusPolicy(Qt::NoFocus);
}

PropTabBar::~PropTabBar()
{
    delete m_btnGroup;
}

int PropTabBar::currentIndex() const
{
    return m_currentIndex;
}

void PropTabBar::setCurrentIndex(int index)
{
    if (index >= m_btnGroup->buttons().size())
    index = 0;
    // If asked to hide or if the currently selected tab is clicked
    if (index < 0 || m_currentIndex == index) {
        if (m_currentIndex >= 0) {
          m_btnGroup->button(m_currentIndex)->setDown(false);
          m_currentIndex = -1;
          emit visibilityToggled(false);
        }
        return;
    }
    // Unselect previous tab
    if (m_currentIndex >= 0) {
        m_btnGroup->button(m_currentIndex)->setDown(false);
    }
    else {
        // Nothing was selected, show!
        emit visibilityToggled(true);
    }
    // Select the new button
    m_btnGroup->button(index)->setDown(true);
    m_currentIndex = index;
    // Emit the signal
    emit tabChanged(index);
}
