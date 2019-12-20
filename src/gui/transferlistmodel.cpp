/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2015  Vladimir Golovnev <glassez@yandex.ru>
 * Copyright (C) 2010  Christophe Dumez <chris@qbittorrent.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If you
 * modify file(s), you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version.
 */

#include "transferlistmodel.h"

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QIcon>
#include <QPalette>

#include "base/bittorrent/session.h"
#include "base/bittorrent/torrenthandle.h"
#include "base/global.h"
#include "base/utils/fs.h"


static QColor getColorByState(BitTorrent::TorrentState state, qreal ratio);

static QIcon getPausedIcon();
static QIcon getQueuedIcon();
static QIcon getDownloadingIcon();
static QIcon getStalledDownloadingIcon();
static QIcon getUploadingIcon();
static QIcon getStalledUploadingIcon();
static QIcon getCompletedIcon();
static QIcon getCheckingIcon();
static QIcon getErrorIcon();

static bool isDarkTheme();

// TransferListModel

TransferListModel::TransferListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_stateForegroundColors {
        {BitTorrent::TorrentState::Unknown, getColorByState(BitTorrent::TorrentState::Unknown)},
        {BitTorrent::TorrentState::ForcedDownloading, getColorByState(BitTorrent::TorrentState::ForcedDownloading)},
        {BitTorrent::TorrentState::Downloading, getColorByState(BitTorrent::TorrentState::Downloading)},
        {BitTorrent::TorrentState::DownloadingMetadata, getColorByState(BitTorrent::TorrentState::DownloadingMetadata)},
        {BitTorrent::TorrentState::Allocating, getColorByState(BitTorrent::TorrentState::Allocating)},
        {BitTorrent::TorrentState::StalledDownloading, getColorByState(BitTorrent::TorrentState::StalledDownloading)},
        {BitTorrent::TorrentState::ForcedUploading, getColorByState(BitTorrent::TorrentState::ForcedUploading)},
        {BitTorrent::TorrentState::Uploading, getColorByState(BitTorrent::TorrentState::Uploading)},
        {BitTorrent::TorrentState::StalledUploading, getColorByState(BitTorrent::TorrentState::StalledUploading)},
        {BitTorrent::TorrentState::CheckingResumeData, getColorByState(BitTorrent::TorrentState::CheckingResumeData)},
        {BitTorrent::TorrentState::QueuedDownloading, getColorByState(BitTorrent::TorrentState::QueuedDownloading)},
        {BitTorrent::TorrentState::QueuedUploading, getColorByState(BitTorrent::TorrentState::QueuedUploading)},
        {BitTorrent::TorrentState::CheckingUploading, getColorByState(BitTorrent::TorrentState::CheckingUploading)},
        {BitTorrent::TorrentState::CheckingDownloading, getColorByState(BitTorrent::TorrentState::CheckingDownloading)},
        {BitTorrent::TorrentState::PausedDownloading, getColorByState(BitTorrent::TorrentState::PausedDownloading)},
        {BitTorrent::TorrentState::PausedUploading, getColorByState(BitTorrent::TorrentState::PausedUploading)},
        {BitTorrent::TorrentState::Moving, getColorByState(BitTorrent::TorrentState::Moving)},
        {BitTorrent::TorrentState::MissingFiles, getColorByState(BitTorrent::TorrentState::MissingFiles)},
        {BitTorrent::TorrentState::Error, getColorByState(BitTorrent::TorrentState::Error)}
    }
{
    // Load the torrents
    using namespace BitTorrent;
    for (TorrentHandle *const torrent : asConst(Session::instance()->torrents()))
        addTorrent(torrent);

    // Listen for torrent changes
    connect(Session::instance(), &Session::torrentAdded, this, &TransferListModel::addTorrent);
    connect(Session::instance(), &Session::torrentAboutToBeRemoved, this, &TransferListModel::handleTorrentAboutToBeRemoved);
    connect(Session::instance(), &Session::torrentsUpdated, this, &TransferListModel::handleTorrentsUpdated);

    connect(Session::instance(), &Session::torrentFinished, this, &TransferListModel::handleTorrentStatusUpdated);
    connect(Session::instance(), &Session::torrentMetadataLoaded, this, &TransferListModel::handleTorrentStatusUpdated);
    connect(Session::instance(), &Session::torrentResumed, this, &TransferListModel::handleTorrentStatusUpdated);
    connect(Session::instance(), &Session::torrentPaused, this, &TransferListModel::handleTorrentStatusUpdated);
    connect(Session::instance(), &Session::torrentFinishedChecking, this, &TransferListModel::handleTorrentStatusUpdated);
}

int TransferListModel::rowCount(const QModelIndex &) const
{
    return m_torrentList.size();
}

int TransferListModel::columnCount(const QModelIndex &) const
{
    return NB_COLUMNS;
}

QVariant TransferListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
            case TR_QUEUE_POSITION: return QChar('#');
            case TR_NAME: return tr("Name", "i.e: torrent name");
            case TR_SIZE: return tr("Size", "i.e: torrent size");
            case TR_PROGRESS: return tr("Done", "% Done");
            case TR_STATUS: return tr("Status", "Torrent status (e.g. downloading, seeding, paused)");
            case TR_SEEDS: return tr("Seeds", "i.e. full sources (often untranslated)");
            case TR_PEERS: return tr("Peers", "i.e. partial sources (often untranslated)");
            case TR_DLSPEED: return tr("Down Speed", "i.e: Download speed");
            case TR_UPSPEED: return tr("Up Speed", "i.e: Upload speed");
            case TR_RATIO: return tr("Ratio", "Share ratio");
            case TR_ETA: return tr("ETA", "i.e: Estimated Time of Arrival / Time left");
            case TR_CATEGORY: return tr("Category");
            case TR_TAGS: return tr("Tags");
            case TR_ADD_DATE: return tr("Added On", "Torrent was added to transfer list on 01/01/2010 08:00");
            case TR_SEED_DATE: return tr("Completed On", "Torrent was completed on 01/01/2010 08:00");
            case TR_TRACKER: return tr("Tracker");
            case TR_DLLIMIT: return tr("Down Limit", "i.e: Download limit");
            case TR_UPLIMIT: return tr("Up Limit", "i.e: Upload limit");
            case TR_AMOUNT_DOWNLOADED: return tr("Downloaded", "Amount of data downloaded (e.g. in MB)");
            case TR_AMOUNT_UPLOADED: return tr("Uploaded", "Amount of data uploaded (e.g. in MB)");
            case TR_AMOUNT_DOWNLOADED_SESSION: return tr("Session Download", "Amount of data downloaded since program open (e.g. in MB)");
            case TR_AMOUNT_UPLOADED_SESSION: return tr("Session Upload", "Amount of data uploaded since program open (e.g. in MB)");
            case TR_AMOUNT_LEFT: return tr("Remaining", "Amount of data left to download (e.g. in MB)");
            case TR_TIME_ELAPSED: return tr("Time Active", "Time (duration) the torrent is active (not paused)");
            case TR_SAVE_PATH: return tr("Save path", "Torrent save path");
            case TR_COMPLETED: return tr("Completed", "Amount of data completed (e.g. in MB)");
            case TR_RATIO_LIMIT: return tr("Ratio Limit", "Upload share ratio limit");
            case TR_SEEN_COMPLETE_DATE: return tr("Last Seen Complete", "Indicates the time when the torrent was last seen complete/whole");
            case TR_LAST_ACTIVITY: return tr("Last Activity", "Time passed since a chunk was downloaded/uploaded");
            case TR_TOTAL_SIZE: return tr("Total Size", "i.e. Size including unwanted data");
            case TR_AVAILABILITY: return tr("Availability", "The number of distributed copies of the torrent");
            default: return {};
            }
        }
        else if (role == Qt::TextAlignmentRole) {
            switch (section) {
            case TR_AMOUNT_DOWNLOADED:
            case TR_AMOUNT_UPLOADED:
            case TR_AMOUNT_DOWNLOADED_SESSION:
            case TR_AMOUNT_UPLOADED_SESSION:
            case TR_AMOUNT_LEFT:
            case TR_COMPLETED:
            case TR_SIZE:
            case TR_TOTAL_SIZE:
            case TR_ETA:
            case TR_SEEDS:
            case TR_PEERS:
            case TR_UPSPEED:
            case TR_DLSPEED:
            case TR_UPLIMIT:
            case TR_DLLIMIT:
            case TR_RATIO_LIMIT:
            case TR_RATIO:
            case TR_QUEUE_POSITION:
            case TR_LAST_ACTIVITY:
            case TR_AVAILABILITY:
                return QVariant(Qt::AlignRight | Qt::AlignVCenter);
            default:
                return QAbstractListModel::headerData(section, orientation, role);
            }
        }
    }

    return {};
}

QVariant TransferListModel::data(const QModelIndex &index, const int role) const
{
    if (!index.isValid()) return {};

    const BitTorrent::TorrentHandle *torrent = m_torrentList.value(index.row());
    if (!torrent) return {};

    if ((role == Qt::DecorationRole) && (index.column() == TR_NAME))
        return getIconByState(torrent->state());

    if (role == Qt::ForegroundRole)
        return stateForeground(torrent->state());

    if ((role != Qt::DisplayRole) && (role != Qt::UserRole))
        return {};

    switch (index.column()) {
    case TR_NAME:
        return torrent->name();
    case TR_QUEUE_POSITION:
        return torrent->queuePosition();
    case TR_SIZE:
        return torrent->wantedSize();
    case TR_PROGRESS:
        return torrent->progress();
    case TR_STATUS:
        return (role == Qt::DisplayRole) ? QVariant::fromValue(torrent->state()) : torrent->error();
    case TR_SEEDS:
        return (role == Qt::DisplayRole) ? torrent->seedsCount() : torrent->totalSeedsCount();
    case TR_PEERS:
        return (role == Qt::DisplayRole) ? torrent->leechsCount() : torrent->totalLeechersCount();
    case TR_DLSPEED:
        return torrent->downloadPayloadRate();
    case TR_UPSPEED:
        return torrent->uploadPayloadRate();
    case TR_ETA:
        return torrent->eta();
    case TR_RATIO:
        return torrent->realRatio();
    case TR_CATEGORY:
        return torrent->category();
    case TR_TAGS: {
            QStringList tagsList = torrent->tags().values();
            tagsList.sort();
            return tagsList.join(", ");
        }
    case TR_ADD_DATE:
        return torrent->addedTime();
    case TR_SEED_DATE:
        return torrent->completedTime();
    case TR_TRACKER:
        return torrent->currentTracker();
    case TR_DLLIMIT:
        return torrent->downloadLimit();
    case TR_UPLIMIT:
        return torrent->uploadLimit();
    case TR_AMOUNT_DOWNLOADED:
        return torrent->totalDownload();
    case TR_AMOUNT_UPLOADED:
        return torrent->totalUpload();
    case TR_AMOUNT_DOWNLOADED_SESSION:
        return torrent->totalPayloadDownload();
    case TR_AMOUNT_UPLOADED_SESSION:
        return torrent->totalPayloadUpload();
    case TR_AMOUNT_LEFT:
        return torrent->incompletedSize();
    case TR_TIME_ELAPSED:
        return (role == Qt::DisplayRole) ? torrent->activeTime() : torrent->seedingTime();
    case TR_SAVE_PATH:
        return Utils::Fs::toNativePath(torrent->savePath());
    case TR_COMPLETED:
        return torrent->completedSize();
    case TR_RATIO_LIMIT:
        return torrent->maxRatio();
    case TR_SEEN_COMPLETE_DATE:
        return torrent->lastSeenComplete();
    case TR_LAST_ACTIVITY:
        if (torrent->isPaused() || torrent->isChecking())
            return -1;
        return torrent->timeSinceActivity();
    case TR_AVAILABILITY:
        return torrent->distributedCopies();
    case TR_TOTAL_SIZE:
        return torrent->totalSize();
    }

    return {};
}

bool TransferListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || (role != Qt::DisplayRole)) return false;

    BitTorrent::TorrentHandle *const torrent = m_torrentList.value(index.row());
    if (!torrent) return false;

    // Category and Name columns can be edited
    switch (index.column()) {
    case TR_NAME:
        torrent->setName(value.toString());
        break;
    case TR_CATEGORY:
        torrent->setCategory(value.toString());
        break;
    default:
        return false;
    }

    return true;
}

void TransferListModel::addTorrent(BitTorrent::TorrentHandle *const torrent)
{
    Q_ASSERT(!m_torrentMap.contains(torrent));

    const int row = m_torrentList.size();

    beginInsertRows({}, row, row);
    m_torrentList << torrent;
    m_torrentMap[torrent] = row;
    endInsertRows();
}

Qt::ItemFlags TransferListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;

    // Explicitly mark as editable
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

BitTorrent::TorrentHandle *TransferListModel::torrentHandle(const QModelIndex &index) const
{
    if (!index.isValid()) return nullptr;

    return m_torrentList.value(index.row());
}

void TransferListModel::handleTorrentAboutToBeRemoved(BitTorrent::TorrentHandle *const torrent)
{
    const int row = m_torrentMap.value(torrent, -1);
    Q_ASSERT(row >= 0);

    beginRemoveRows({}, row, row);
    m_torrentList.removeAt(row);
    m_torrentMap.remove(torrent);
    for (int &value : m_torrentMap) {
        if (value > row)
            --value;
    }
    endRemoveRows();
}

void TransferListModel::handleTorrentStatusUpdated(BitTorrent::TorrentHandle *const torrent)
{
    const int row = m_torrentMap.value(torrent, -1);
    Q_ASSERT(row >= 0);

    emit dataChanged(index(row, 0), index(row, columnCount() - 1));
}

void TransferListModel::handleTorrentsUpdated(const QVector<BitTorrent::TorrentHandle *> &torrents)
{
    const int columns = (columnCount() - 1);

    if (torrents.size() <= (m_torrentList.size() * 0.5)) {
        for (BitTorrent::TorrentHandle *const torrent : torrents) {
            const int row = m_torrentMap.value(torrent, -1);
            Q_ASSERT(row >= 0);

            emit dataChanged(index(row, 0), index(row, columns));
        }
    }
    else {
        // save the overhead when more than half of the torrent list needs update
        emit dataChanged(index(0, 0), index((rowCount() - 1), columns));
    }
}

void TransferListModel::setStateForeground(const BitTorrent::TorrentState state, const QColor &color)
{
    m_stateForegroundColors[state] = color;
}

QColor TransferListModel::stateForeground(const BitTorrent::TorrentState state) const
{
    return m_stateForegroundColors[state];
}

// Static functions

QIcon TransferListModel::getIconByState(const BitTorrent::TorrentState state)
{
    switch (state) {
    case BitTorrent::TorrentState::Downloading:
    case BitTorrent::TorrentState::ForcedDownloading:
    case BitTorrent::TorrentState::DownloadingMetadata:
        return getDownloadingIcon();
    case BitTorrent::TorrentState::Allocating:
    case BitTorrent::TorrentState::StalledDownloading:
        return getStalledDownloadingIcon();
    case BitTorrent::TorrentState::StalledUploading:
        return getStalledUploadingIcon();
    case BitTorrent::TorrentState::Uploading:
    case BitTorrent::TorrentState::UploadingGoodRatio:
    case BitTorrent::TorrentState::ForcedUploading:
        return getUploadingIcon();
    case BitTorrent::TorrentState::PausedDownloading:
        return getPausedIcon();
    case BitTorrent::TorrentState::PausedUploading:
        return getCompletedIcon();
    case BitTorrent::TorrentState::QueuedDownloading:
    case BitTorrent::TorrentState::QueuedUploading:
        return getQueuedIcon();
    case BitTorrent::TorrentState::CheckingDownloading:
    case BitTorrent::TorrentState::CheckingUploading:
    case BitTorrent::TorrentState::CheckingResumeData:
    case BitTorrent::TorrentState::Moving:
        return getCheckingIcon();
    case BitTorrent::TorrentState::Unknown:
    case BitTorrent::TorrentState::MissingFiles:
    case BitTorrent::TorrentState::Error:
        return getErrorIcon();
    default:
        Q_ASSERT(false);
        return getErrorIcon();
    }
}

QColor getColorByState(const BitTorrent::TorrentState state, qreal ratio)
{
    // Color names taken from http://cloford.com/resources/colours/500col.htm
    bool dark = isDarkTheme();

    switch (state) {
    case BitTorrent::TorrentState::Downloading:
    case BitTorrent::TorrentState::ForcedDownloading:
    case BitTorrent::TorrentState::DownloadingMetadata:
        if (!dark)
            return QColor(20, 55, 209);
        else
            return {50, 205, 50}; // Lime Green
    case BitTorrent::TorrentState::Allocating:
    case BitTorrent::TorrentState::StalledDownloading:
            return QColor(250, 128, 114); // Salmon
    case BitTorrent::TorrentState::StalledUploading:
        if (!dark)
            return QColor(0, 100, 0);
        else
            return {204, 204, 204}; // Gray 80
    case BitTorrent::TorrentState::UploadingGoodRatio:
        return QColor(153, 0, 153);
    case BitTorrent::TorrentState::Uploading:
    case BitTorrent::TorrentState::ForcedUploading:
        if (!dark)
            return QColor(0, 160, 0);
        else
            return {99, 184, 255}; // Steel Blue 1
    case BitTorrent::TorrentState::PausedDownloading:
        return {250, 128, 114}; // Salmon
    case BitTorrent::TorrentState::PausedUploading:
        if (!dark)
            return {0, 0, 139}; // Dark Blue
        else
            return {79, 148, 205}; // Steel Blue 3
    case BitTorrent::TorrentState::Error:
    case BitTorrent::TorrentState::MissingFiles:
        return {255, 0, 0}; // red
    case BitTorrent::TorrentState::QueuedDownloading:
    case BitTorrent::TorrentState::QueuedUploading:
    case BitTorrent::TorrentState::CheckingDownloading:
    case BitTorrent::TorrentState::CheckingUploading:
    case BitTorrent::TorrentState::CheckingResumeData:
    case BitTorrent::TorrentState::Moving:
        if (!dark)
            return {0, 128, 128}; // Teal
        else
            return {0, 205, 205}; // Cyan 3
    case BitTorrent::TorrentState::Unknown:
        return {255, 0, 0}; // red
    default:
        Q_ASSERT(false);
        return {255, 0, 0}; // red
    }
}

QIcon getPausedIcon()
{
    static QIcon cached = QIcon(":/icons/skin/paused.svg");
    return cached;
}

QIcon getQueuedIcon()
{
    static QIcon cached = QIcon(":/icons/skin/queued.svg");
    return cached;
}

QIcon getDownloadingIcon()
{
    static QIcon cached = QIcon(":/icons/skin/downloading.svg");
    return cached;
}

QIcon getStalledDownloadingIcon()
{
    static QIcon cached = QIcon(":/icons/skin/stalledDL.svg");
    return cached;
}

QIcon getUploadingIcon()
{
    static QIcon cached = QIcon(":/icons/skin/uploading.svg");
    return cached;
}

QIcon getStalledUploadingIcon()
{
    static QIcon cached = QIcon(":/icons/skin/stalledUP.svg");
    return cached;
}

QIcon getCompletedIcon()
{
    static QIcon cached = QIcon(":/icons/skin/completed.svg");
    return cached;
}

QIcon getCheckingIcon()
{
    static QIcon cached = QIcon(":/icons/skin/checking.svg");
    return cached;
}

QIcon getErrorIcon()
{
    static QIcon cached = QIcon(":/icons/skin/error.svg");
    return cached;
}

bool isDarkTheme()
{
    const QPalette pal = QApplication::palette();
    // QPalette::Base is used for the background of the Treeview
    const QColor &color = pal.color(QPalette::Active, QPalette::Base);
    return (color.lightness() < 127);
}
