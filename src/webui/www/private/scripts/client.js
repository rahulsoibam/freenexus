/*
 * MIT License
 * Copyright (c) 2008 Ishan Arora <ishan@freenexus.org>,
 * Christophe Dumez <chris@freenexus.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

torrentsTable = new TorrentsTable();
torrentPeersTable = new TorrentPeersTable();

var updatePropertiesPanel = function() {};

var updateTorrentData = function() {};
var updateTrackersData = function() {};
var updateTorrentPeersData = function() {};
var updateWebSeedsData = function() {};
var updateTorrentFilesData = function() {};

var updateMainData = function() {};
var alternativeSpeedLimits = false;
var queueing_enabled = true;
var syncMainDataTimerPeriod = 1500;

var clipboardEvent;

var CATEGORIES_ALL = 1;
var CATEGORIES_UNCATEGORIZED = 2;

var category_list = {};

var selected_category = CATEGORIES_ALL;
var setCategoryFilter = function() {};

var selected_filter = getLocalStorageItem('selected_filter', 'all');
var setFilter = function() {};

var loadSelectedCategory = function() {
    selected_category = getLocalStorageItem('selected_category', CATEGORIES_ALL);
};
loadSelectedCategory();

function genHash(string) {
    var hash = 0;
    for (var i = 0; i < string.length; i++) {
        var c = string.charCodeAt(i);
        hash = (c + hash * 31) | 0;
    }
    return hash;
}

window.addEvent('load', function() {

    var saveColumnSizes = function() {
        var filters_width = $('Filters').getSize().x;
        var properties_height_rel = $('propertiesPanel').getSize().y / Window.getSize().y;
        localStorage.setItem('filters_width', filters_width);
        localStorage.setItem('properties_height_rel', properties_height_rel);
    };

    window.addEvent('resize', function() {
        // Resizing might takes some time.
        saveColumnSizes.delay(200);
    });

    /*MochaUI.Desktop = new MochaUI.Desktop();
    MochaUI.Desktop.desktop.setStyles({
        'background': '#fff',
        'visibility': 'visible'
    });*/
    MochaUI.Desktop.initialize();

    var filt_w = localStorage.getItem('filters_width');
    if ($defined(filt_w))
        filt_w = filt_w.toInt();
    else
        filt_w = 120;
    new MochaUI.Column({
        id: 'filtersColumn',
        placement: 'left',
        onResize: saveColumnSizes,
        width: filt_w,
        resizeLimit: [100, 300]
    });
    new MochaUI.Column({
        id: 'mainColumn',
        placement: 'main',
        width: null,
        resizeLimit: [100, 300]
    });

    setCategoryFilter = function(hash) {
        selected_category = hash;
        localStorage.setItem('selected_category', selected_category);
        highlightSelectedCategory();
        if (typeof torrentsTable.tableBody != 'undefined')
            updateMainData();
    };

    setFilter = function(f) {
        // Visually Select the right filter
        $("all_filter").removeClass("selectedFilter");
        $("downloading_filter").removeClass("selectedFilter");
        $("seeding_filter").removeClass("selectedFilter");
        $("completed_filter").removeClass("selectedFilter");
        $("paused_filter").removeClass("selectedFilter");
        $("resumed_filter").removeClass("selectedFilter");
        $("active_filter").removeClass("selectedFilter");
        $("inactive_filter").removeClass("selectedFilter");
        $("errored_filter").removeClass("selectedFilter");
        $(f + "_filter").addClass("selectedFilter");
        selected_filter = f;
        localStorage.setItem('selected_filter', f);
        // Reload torrents
        if (typeof torrentsTable.tableBody != 'undefined')
            updateMainData();
    };

    new MochaUI.Panel({
        id: 'Filters',
        title: 'Panel',
        header: false,
        padding: {
            top: 0,
            right: 0,
            bottom: 0,
            left: 0
        },
        loadMethod: 'xhr',
        contentURL: 'filters.html',
        onContentLoaded: function() {
            setFilter(selected_filter);
        },
        column: 'filtersColumn',
        height: 300
    });
    initializeWindows();

    // Show Top Toolbar is enabled by default
    var showTopToolbar = true;
    if (localStorage.getItem('show_top_toolbar') !== null)
        showTopToolbar = localStorage.getItem('show_top_toolbar') == "true";
    if (!showTopToolbar) {
        $('showTopToolbarLink').firstChild.style.opacity = '0';
        $('mochaToolbar').addClass('invisible');
    }

    // Show Status Bar is enabled by default
    var showStatusBar = true;
    if (localStorage.getItem('show_status_bar') !== null)
        showStatusBar = localStorage.getItem('show_status_bar') === "true";
    if (!showStatusBar) {
        $('showStatusBarLink').firstChild.style.opacity = '0';
        $('desktopFooterWrapper').addClass('invisible');
    }

    var speedInTitle = localStorage.getItem('speed_in_browser_title_bar') == "true";
    if (!speedInTitle)
        $('speedInBrowserTitleBarLink').firstChild.style.opacity = '0';

    // After showing/hiding the toolbar + status bar
    MochaUI.Desktop.setDesktopSize();

    var syncMainDataLastResponseId = 0;
    var serverState = {};

    var removeTorrentFromCategoryList = function(hash) {
        if (hash === null || hash === "")
            return false;
        var removed = false;
        Object.each(category_list, function(category) {
            if (Object.contains(category.torrents, hash)) {
                removed = true;
                category.torrents.splice(category.torrents.indexOf(hash), 1);
            }
        });
        return removed;
    };

    var addTorrentToCategoryList = function(torrent) {
        var category = torrent['category'];
        if (typeof category === 'undefined')
            return false;
        if (category.length === 0) { // Empty category
            removeTorrentFromCategoryList(torrent['hash']);
            return true;
        }
        var categoryHash = genHash(category);
        if (category_list[categoryHash] === null) // This should not happen
            category_list[categoryHash] = {
                name: category,
                torrents: []
            };
        if (!Object.contains(category_list[categoryHash].torrents, torrent['hash'])) {
            removeTorrentFromCategoryList(torrent['hash']);
            category_list[categoryHash].torrents = category_list[categoryHash].torrents.combine([torrent['hash']]);
            return true;
        }
        return false;
    };

    var updateFilter = function(filter, filterTitle) {
        $(filter + '_filter').firstChild.childNodes[1].nodeValue = filterTitle.replace('%1', torrentsTable.getFilteredTorrentsNumber(filter, CATEGORIES_ALL));
    };

    var updateFiltersList = function() {
        updateFilter('all', 'QBT_TR(All (%1))QBT_TR[CONTEXT=StatusFiltersWidget]');
        updateFilter('downloading', 'QBT_TR(Downloading (%1))QBT_TR[CONTEXT=StatusFiltersWidget]');
        updateFilter('seeding', 'QBT_TR(Seeding (%1))QBT_TR[CONTEXT=StatusFiltersWidget]');
        updateFilter('completed', 'QBT_TR(Completed (%1))QBT_TR[CONTEXT=StatusFiltersWidget]');
        updateFilter('resumed', 'QBT_TR(Resumed (%1))QBT_TR[CONTEXT=StatusFiltersWidget]');
        updateFilter('paused', 'QBT_TR(Paused (%1))QBT_TR[CONTEXT=StatusFiltersWidget]');
        updateFilter('active', 'QBT_TR(Active (%1))QBT_TR[CONTEXT=StatusFiltersWidget]');
        updateFilter('inactive', 'QBT_TR(Inactive (%1))QBT_TR[CONTEXT=StatusFiltersWidget]');
        updateFilter('errored', 'QBT_TR(Errored (%1))QBT_TR[CONTEXT=StatusFiltersWidget]');
    };

    var updateCategoryList = function() {
        var categoryList = $('filterCategoryList');
        if (!categoryList)
            return;
        categoryList.empty();

        var create_link = function(hash, text, count) {
            var html = '<a href="#" onclick="setCategoryFilter(' + hash + ');return false;">'
                + '<img src="theme/inode-directory"/>'
                + escapeHtml(text) + ' (' + count + ')' + '</a>';
            var el = new Element('li', {
                id: hash,
                html: html
            });
            categoriesFilterContextMenu.addTarget(el);
            return el;
        };

        var all = torrentsTable.getRowIds().length;
        var uncategorized = 0;
        Object.each(torrentsTable.rows, function(row) {
            if (row['full_data'].category.length === 0)
                uncategorized += 1;
        });
        categoryList.appendChild(create_link(CATEGORIES_ALL, 'QBT_TR(All)QBT_TR[CONTEXT=CategoryFilterModel]', all));
        categoryList.appendChild(create_link(CATEGORIES_UNCATEGORIZED, 'QBT_TR(Uncategorized)QBT_TR[CONTEXT=CategoryFilterModel]', uncategorized));

        var sortedCategories = [];
        Object.each(category_list, function(category) {
            sortedCategories.push(category.name);
        });
        sortedCategories.sort();

        Object.each(sortedCategories, function(categoryName) {
            var categoryHash = genHash(categoryName);
            var categoryCount = category_list[categoryHash].torrents.length;
            categoryList.appendChild(create_link(categoryHash, categoryName, categoryCount));
        });

        highlightSelectedCategory();
    };

    var highlightSelectedCategory = function() {
        var categoryList = $('filterCategoryList');
        if (!categoryList)
            return;
        var childrens = categoryList.childNodes;
        for (var i in childrens) {
            if (childrens[i].id == selected_category)
                childrens[i].className = "selectedFilter";
            else
                childrens[i].className = "";
        }
    };

    var syncMainDataTimer;
    var syncMainData = function() {
        var url = new URI('api/v2/sync/maindata');
        url.setData('rid', syncMainDataLastResponseId);
        var request = new Request.JSON({
            url: url,
            noCache: true,
            method: 'get',
            onFailure: function() {
                var errorDiv = $('error_div');
                if (errorDiv)
                    errorDiv.set('html', 'QBT_TR(freeNexus client is not reachable)QBT_TR[CONTEXT=HttpServer]');
                clearTimeout(syncMainDataTimer);
                syncMainDataTimer = syncMainData.delay(2000);
            },
            onSuccess: function(response) {
                $('error_div').set('html', '');
                if (response) {
                    var torrentsTableSelectedRows;
                    var update_categories = false;
                    var full_update = (response['full_update'] === true);
                    if (full_update) {
                        torrentsTableSelectedRows = torrentsTable.selectedRowsIds();
                        torrentsTable.clear();
                        category_list = {};
                    }
                    if (response['rid']) {
                        syncMainDataLastResponseId = response['rid'];
                    }
                    if (response['categories']) {
                        response['categories'].each(function(category) {
                            var categoryHash = genHash(category);
                            category_list[categoryHash] = {
                                name: category,
                                torrents: []
                            };
                        });
                        update_categories = true;
                    }
                    if (response['categories_removed']) {
                        response['categories_removed'].each(function(category) {
                            var categoryHash = genHash(category);
                            delete category_list[categoryHash];
                        });
                        update_categories = true;
                    }
                    if (response['torrents']) {
                        var updateTorrentList = false;
                        for (var key in response['torrents']) {
                            response['torrents'][key]['hash'] = key;
                            response['torrents'][key]['rowId'] = key;
                            if (response['torrents'][key]['state'])
                                response['torrents'][key]['status'] = response['torrents'][key]['state'];
                            torrentsTable.updateRowData(response['torrents'][key]);
                            if (addTorrentToCategoryList(response['torrents'][key]))
                                update_categories = true;
                            if (response['torrents'][key]['name'])
                                updateTorrentList = true;
                        }

                        if (updateTorrentList)
                            setupCopyEventHandler();
                    }
                    if (response['torrents_removed'])
                        response['torrents_removed'].each(function(hash) {
                            torrentsTable.removeRow(hash);
                            removeTorrentFromCategoryList(hash);
                            update_categories = true; // Always to update All category
                        });
                    torrentsTable.updateTable(full_update);
                    torrentsTable.altRow();
                    if (response['server_state']) {
                        var tmp = response['server_state'];
                        for (var k in tmp)
                            serverState[k] = tmp[k];
                        processServerState();
                    }
                    updateFiltersList();
                    if (update_categories) {
                        updateCategoryList();
                        torrentsTableContextMenu.updateCategoriesSubMenu(category_list);
                    }

                    if (full_update)
                        // re-select previously selected rows
                        torrentsTable.reselectRows(torrentsTableSelectedRows);
                }
                clearTimeout(syncMainDataTimer);
                syncMainDataTimer = syncMainData.delay(syncMainDataTimerPeriod);
            }
        }).send();
    };

    updateMainData = function() {
        torrentsTable.updateTable();
        clearTimeout(syncMainDataTimer);
        syncMainDataTimer = syncMainData.delay(100);
    };

    var processServerState = function() {
        var transfer_info = friendlyUnit(serverState.dl_info_speed, true);
        if (serverState.dl_rate_limit > 0)
            transfer_info += " [" + friendlyUnit(serverState.dl_rate_limit, true) + "]";
        transfer_info += " (" + friendlyUnit(serverState.dl_info_data, false) + ")";
        $("DlInfos").set('html', transfer_info);
        transfer_info = friendlyUnit(serverState.up_info_speed, true);
        if (serverState.up_rate_limit > 0)
            transfer_info += " [" + friendlyUnit(serverState.up_rate_limit, true) + "]";
        transfer_info += " (" + friendlyUnit(serverState.up_info_data, false) + ")";
        $("UpInfos").set('html', transfer_info);
        if (speedInTitle) {
            document.title = "QBT_TR([D: %1, U: %2] freeNexus %3)QBT_TR[CONTEXT=MainWindow]".replace("%1", friendlyUnit(serverState.dl_info_speed, true)).replace("%2", friendlyUnit(serverState.up_info_speed, true)).replace("%3", "${VERSION}");
            document.title += " QBT_TR(Web UI)QBT_TR[CONTEXT=OptionsDialog]";
        }
        else
            document.title = "freeNexus ${VERSION} QBT_TR(Web UI)QBT_TR[CONTEXT=OptionsDialog]";
        $('DHTNodes').set('html', 'QBT_TR(DHT: %1 nodes)QBT_TR[CONTEXT=StatusBar]'.replace("%1", serverState.dht_nodes));

        // Statistics dialog
        if (document.getElementById("statisticspage")) {
            $('AlltimeDL').set('html', friendlyUnit(serverState.alltime_dl, false));
            $('AlltimeUL').set('html', friendlyUnit(serverState.alltime_ul, false));
            $('TotalWastedSession').set('html', friendlyUnit(serverState.total_wasted_session, false));
            $('GlobalRatio').set('html', serverState.global_ratio);
            $('TotalPeerConnections').set('html', serverState.total_peer_connections);
            $('ReadCacheHits').set('html', serverState.read_cache_hits + "%");
            $('TotalBuffersSize').set('html', friendlyUnit(serverState.total_buffers_size, false));
            $('WriteCacheOverload').set('html', serverState.write_cache_overload + "%");
            $('ReadCacheOverload').set('html', serverState.read_cache_overload + "%");
            $('QueuedIOJobs').set('html', serverState.queued_io_jobs);
            $('AverageTimeInQueue').set('html', serverState.average_time_queue + " ms");
            $('TotalQueuedSize').set('html', friendlyUnit(serverState.total_queued_size, false));
        }

        if (serverState.connection_status == "connected")
            $('connectionStatus').src = 'images/skin/connected.png';
        else if (serverState.connection_status == "firewalled")
            $('connectionStatus').src = 'images/skin/firewalled.png';
        else
            $('connectionStatus').src = 'images/skin/disconnected.png';

        if (queueing_enabled != serverState.queueing) {
            queueing_enabled = serverState.queueing;
            torrentsTable.columns['priority'].force_hide = !queueing_enabled;
            torrentsTable.updateColumn('priority');
            if (queueing_enabled) {
                $('topPrioItem').removeClass('invisible');
                $('increasePrioItem').removeClass('invisible');
                $('decreasePrioItem').removeClass('invisible');
                $('bottomPrioItem').removeClass('invisible');
                $('queueingButtons').removeClass('invisible');
                $('queueingMenuItems').removeClass('invisible');
            }
            else {
                $('topPrioItem').addClass('invisible');
                $('increasePrioItem').addClass('invisible');
                $('decreasePrioItem').addClass('invisible');
                $('bottomPrioItem').addClass('invisible');
                $('queueingButtons').addClass('invisible');
                $('queueingMenuItems').addClass('invisible');
            }
        }

        if (alternativeSpeedLimits != serverState.use_alt_speed_limits) {
            alternativeSpeedLimits = serverState.use_alt_speed_limits;
            updateAltSpeedIcon(alternativeSpeedLimits);
        }

        syncMainDataTimerPeriod = serverState.refresh_interval;
        if (syncMainDataTimerPeriod < 500)
            syncMainDataTimerPeriod = 500;
    };

    var updateAltSpeedIcon = function(enabled) {
        if (enabled)
            $('alternativeSpeedLimits').src = "images/slow.png";
        else
            $('alternativeSpeedLimits').src = "images/slow_off.png";
    };

    $('alternativeSpeedLimits').addEvent('click', function() {
        // Change icon immediately to give some feedback
        updateAltSpeedIcon(!alternativeSpeedLimits);

        new Request({
            url: 'api/v2/transfer/toggleSpeedLimitsMode',
            method: 'post',
            onComplete: function() {
                alternativeSpeedLimits = !alternativeSpeedLimits;
                updateMainData();
            },
            onFailure: function() {
                // Restore icon in case of failure
                updateAltSpeedIcon(alternativeSpeedLimits);
            }
        }).send();
    });

    $('DlInfos').addEvent('click', globalDownloadLimitFN);
    $('UpInfos').addEvent('click', globalUploadLimitFN);

    $('showTopToolbarLink').addEvent('click', function(e) {
        showTopToolbar = !showTopToolbar;
        localStorage.setItem('show_top_toolbar', showTopToolbar.toString());
        if (showTopToolbar) {
            $('showTopToolbarLink').firstChild.style.opacity = '1';
            $('mochaToolbar').removeClass('invisible');
        }
        else {
            $('showTopToolbarLink').firstChild.style.opacity = '0';
            $('mochaToolbar').addClass('invisible');
        }
        MochaUI.Desktop.setDesktopSize();
    });

    $('showStatusBarLink').addEvent('click', function(e) {
        showStatusBar = !showStatusBar;
        localStorage.setItem('show_status_bar', showStatusBar.toString());
        if (showStatusBar) {
            $('showStatusBarLink').firstChild.style.opacity = '1';
            $('desktopFooterWrapper').removeClass('invisible');
        }
        else {
            $('showStatusBarLink').firstChild.style.opacity = '0';
            $('desktopFooterWrapper').addClass('invisible');
        }
        MochaUI.Desktop.setDesktopSize();
    });

    $('speedInBrowserTitleBarLink').addEvent('click', function(e) {
        speedInTitle = !speedInTitle;
        localStorage.setItem('speed_in_browser_title_bar', speedInTitle.toString());
        if (speedInTitle)
            $('speedInBrowserTitleBarLink').firstChild.style.opacity = '1';
        else
            $('speedInBrowserTitleBarLink').firstChild.style.opacity = '0';
        processServerState();
    });

    $('StatisticsLink').addEvent('click', StatisticsLinkFN);

    new MochaUI.Panel({
        id: 'transferList',
        title: 'Panel',
        header: false,
        padding: {
            top: 0,
            right: 0,
            bottom: 0,
            left: 0
        },
        loadMethod: 'xhr',
        contentURL: 'transferlist.html',
        onContentLoaded: function() {
            updateMainData();
        },
        column: 'mainColumn',
        onResize: saveColumnSizes,
        height: null
    });
    var prop_h = localStorage.getItem('properties_height_rel');
    if ($defined(prop_h))
        prop_h = prop_h.toFloat() * Window.getSize().y;
    else
        prop_h = Window.getSize().y / 2.0;
    new MochaUI.Panel({
        id: 'propertiesPanel',
        title: 'Panel',
        header: true,
        padding: {
            top: 0,
            right: 0,
            bottom: 0,
            left: 0
        },
        contentURL: 'properties_content.html',
        require: {
            css: ['css/Tabs.css', 'css/dynamicTable.css'],
            js: ['scripts/prop-general.js', 'scripts/prop-trackers.js', 'scripts/prop-webseeds.js', 'scripts/prop-files.js'],
        },
        tabsURL: 'properties.html',
        tabsOnload: function() {
            MochaUI.initializeTabs('propertiesTabs');

            updatePropertiesPanel = function() {
                if (!$('prop_general').hasClass('invisible'))
                    updateTorrentData();
                else if (!$('prop_trackers').hasClass('invisible'))
                    updateTrackersData();
                else if (!$('prop_peers').hasClass('invisible'))
                    updateTorrentPeersData();
                else if (!$('prop_webseeds').hasClass('invisible'))
                    updateWebSeedsData();
                else if (!$('prop_files').hasClass('invisible'))
                    updateTorrentFilesData();
            };

            $('PropGeneralLink').addEvent('click', function(e) {
                $('prop_general').removeClass("invisible");
                $('prop_trackers').addClass("invisible");
                $('prop_webseeds').addClass("invisible");
                $('prop_files').addClass("invisible");
                $('prop_peers').addClass("invisible");
                updatePropertiesPanel();
                localStorage.setItem('selected_tab', this.id);
            });

            $('PropTrackersLink').addEvent('click', function(e) {
                $('prop_trackers').removeClass("invisible");
                $('prop_general').addClass("invisible");
                $('prop_webseeds').addClass("invisible");
                $('prop_files').addClass("invisible");
                $('prop_peers').addClass("invisible");
                updatePropertiesPanel();
                localStorage.setItem('selected_tab', this.id);
            });

            $('PropPeersLink').addEvent('click', function(e) {
                $('prop_peers').removeClass("invisible");
                $('prop_trackers').addClass("invisible");
                $('prop_general').addClass("invisible");
                $('prop_webseeds').addClass("invisible");
                $('prop_files').addClass("invisible");
                updatePropertiesPanel();
                localStorage.setItem('selected_tab', this.id);
            });

            $('PropWebSeedsLink').addEvent('click', function(e) {
                $('prop_webseeds').removeClass("invisible");
                $('prop_general').addClass("invisible");
                $('prop_trackers').addClass("invisible");
                $('prop_files').addClass("invisible");
                $('prop_peers').addClass("invisible");
                updatePropertiesPanel();
                localStorage.setItem('selected_tab', this.id);
            });

            $('PropFilesLink').addEvent('click', function(e) {
                $('prop_files').removeClass("invisible");
                $('prop_general').addClass("invisible");
                $('prop_trackers').addClass("invisible");
                $('prop_webseeds').addClass("invisible");
                $('prop_peers').addClass("invisible");
                updatePropertiesPanel();
                localStorage.setItem('selected_tab', this.id);
            });

            $('propertiesPanel_collapseToggle').addEvent('click', function(e) {
                updatePropertiesPanel();
            });
        },
        column: 'mainColumn',
        height: prop_h
    });
});

function closeWindows() {
    MochaUI.closeAll();
}

function setupCopyEventHandler() {
    if (clipboardEvent)
        clipboardEvent.destroy();

    clipboardEvent = new ClipboardJS('.copyToClipboard', {
        text: function(trigger) {
            var textToCopy;

            switch (trigger.id) {
                case "CopyName":
                    textToCopy = copyNameFN();
                    break;
                case "CopyMagnetLink":
                    textToCopy = copyMagnetLinkFN();
                    break;
                case "CopyHash":
                    textToCopy = copyHashFN();
                    break;
            }

            return textToCopy;
        }
    });
}

var keyboardEvents = new Keyboard({
    defaultEventType: 'keydown',
    events: {
        'ctrl+a': function(event) {
            torrentsTable.selectAll();
            event.preventDefault();
        },
        'delete': function(event) {
            deleteFN();
            event.preventDefault();
        }
    }
});

keyboardEvents.activate();

var loadTorrentPeersTimer;
var syncTorrentPeersLastResponseId = 0;
var show_flags = true;
var loadTorrentPeersData = function() {
    if ($('prop_peers').hasClass('invisible')
        || $('propertiesPanel_collapseToggle').hasClass('panel-expand')) {
        syncTorrentPeersLastResponseId = 0;
        torrentPeersTable.clear();
        return;
    }
    var current_hash = torrentsTable.getCurrentTorrentHash();
    if (current_hash === "") {
        syncTorrentPeersLastResponseId = 0;
        torrentPeersTable.clear();
        clearTimeout(loadTorrentPeersTimer);
        loadTorrentPeersTimer = loadTorrentPeersData.delay(syncMainDataTimerPeriod);
        return;
    }
    var url = new URI('api/v2/sync/torrentPeers');
    url.setData('rid', syncTorrentPeersLastResponseId);
    url.setData('hash', current_hash);
    var request = new Request.JSON({
        url: url,
        noCache: true,
        method: 'get',
        onFailure: function() {
            $('error_div').set('html', 'QBT_TR(freeNexus client is not reachable)QBT_TR[CONTEXT=HttpServer]');
            clearTimeout(loadTorrentPeersTimer);
            loadTorrentPeersTimer = loadTorrentPeersData.delay(5000);
        },
        onSuccess: function(response) {
            $('error_div').set('html', '');
            if (response) {
                var full_update = (response['full_update'] === true);
                if (full_update) {
                    torrentPeersTable.clear();
                }
                if (response['rid']) {
                    syncTorrentPeersLastResponseId = response['rid'];
                }
                if (response['peers']) {
                    for (var key in response['peers']) {
                        response['peers'][key]['rowId'] = key;

                        if (response['peers'][key]['client'])
                            response['peers'][key]['client'] = escapeHtml(response['peers'][key]['client']);

                        torrentPeersTable.updateRowData(response['peers'][key]);
                    }
                }
                if (response['peers_removed'])
                    response['peers_removed'].each(function(hash) {
                        torrentPeersTable.removeRow(hash);
                    });
                torrentPeersTable.updateTable(full_update);
                torrentPeersTable.altRow();

                if (response['show_flags']) {
                    if (show_flags != response['show_flags']) {
                        show_flags = response['show_flags'];
                        torrentPeersTable.columns['country'].force_hide = !show_flags;
                        torrentPeersTable.updateColumn('country');
                    }
                }
            }
            else {
                torrentPeersTable.clear();
            }
            clearTimeout(loadTorrentPeersTimer);
            loadTorrentPeersTimer = loadTorrentPeersData.delay(syncMainDataTimerPeriod);
        }
    }).send();
};

updateTorrentPeersData = function() {
    clearTimeout(loadTorrentPeersTimer);
    loadTorrentPeersData();
};
