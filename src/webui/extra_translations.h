/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2014  Vladimir Golovnev <glassez@yandex.ru>
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

#ifndef EXTRA_TRANSLATIONS_H
#define EXTRA_TRANSLATIONS_H

#include <QObject>

// Additional translations for Web UI
const char *QBT_WEBUI_TRANSLATIONS[] = {
    QT_TRANSLATE_NOOP("HttpServer", "Logout"),
    QT_TRANSLATE_NOOP("HttpServer", "Exit qBittorrent"),
    QT_TRANSLATE_NOOP("HttpServer", "Download Torrents from their URLs or Magnet links"),
    QT_TRANSLATE_NOOP("HttpServer", "Only one link per line"),
    QT_TRANSLATE_NOOP("HttpServer", "Upload local torrent"),
    QT_TRANSLATE_NOOP("HttpServer", "Are you sure you want to delete the selected torrents from the transfer list?"),
    QT_TRANSLATE_NOOP("HttpServer", "Global upload rate limit must be greater than 0 or disabled."),
    QT_TRANSLATE_NOOP("HttpServer", "Global download rate limit must be greater than 0 or disabled."),
    QT_TRANSLATE_NOOP("HttpServer", "Alternative upload rate limit must be greater than 0 or disabled."),
    QT_TRANSLATE_NOOP("HttpServer", "Alternative download rate limit must be greater than 0 or disabled."),
    QT_TRANSLATE_NOOP("HttpServer", "Maximum active downloads must be greater than -1."),
    QT_TRANSLATE_NOOP("HttpServer", "Maximum active uploads must be greater than -1."),
    QT_TRANSLATE_NOOP("HttpServer", "Maximum active torrents must be greater than -1."),
    QT_TRANSLATE_NOOP("HttpServer", "Maximum number of connections limit must be greater than 0 or disabled."),
    QT_TRANSLATE_NOOP("HttpServer", "Maximum number of connections per torrent limit must be greater than 0 or disabled."),
    QT_TRANSLATE_NOOP("HttpServer", "Maximum number of upload slots per torrent limit must be greater than 0 or disabled."),
    QT_TRANSLATE_NOOP("HttpServer", "Unable to save program preferences, qBittorrent is probably unreachable."),
    QT_TRANSLATE_NOOP("HttpServer", "The port used for incoming connections must be between 1 and 65535."),
    QT_TRANSLATE_NOOP("HttpServer", "The port used for the Web UI must be between 1 and 65535."),
    QT_TRANSLATE_NOOP("HttpServer", "Save"),
    QT_TRANSLATE_NOOP("HttpServer", "qBittorrent client is not reachable"),
    QT_TRANSLATE_NOOP("HttpServer", "qBittorrent has been shutdown."),
    QT_TRANSLATE_NOOP("HttpServer", "Unable to log in, qBittorrent is probably unreachable."),
    QT_TRANSLATE_NOOP("HttpServer", "Invalid Username or Password."),
    QT_TRANSLATE_NOOP("HttpServer", "Username"),
    QT_TRANSLATE_NOOP("HttpServer", "Password"),
    QT_TRANSLATE_NOOP("HttpServer", "Login"),
    QT_TRANSLATE_NOOP("HttpServer", "Original authors"),
    QT_TRANSLATE_NOOP("HttpServer", "Apply"),
    QT_TRANSLATE_NOOP("HttpServer", "Add"),
    QT_TRANSLATE_NOOP("HttpServer", "Save files to location:"),
    QT_TRANSLATE_NOOP("HttpServer", "Cookie:"),
    QT_TRANSLATE_NOOP("HttpServer", "Type folder here"),
    QT_TRANSLATE_NOOP("HttpServer", "More information"),
    QT_TRANSLATE_NOOP("HttpServer", "Information about certificates"),
    QT_TRANSLATE_NOOP("HttpServer", "Save Files to"),
    QT_TRANSLATE_NOOP("HttpServer", "IRC: #qbittorrent on Freenode"),
    QT_TRANSLATE_NOOP("HttpServer", "Invalid category name:\nPlease do not use any special characters in the category name."),
    QT_TRANSLATE_NOOP("HttpServer", "Unknown"),
    QT_TRANSLATE_NOOP("HttpServer", "Hard Disk"),
    QT_TRANSLATE_NOOP("HttpServer", "Share ratio limit must be between 0 and 9998."),
    QT_TRANSLATE_NOOP("HttpServer", "Seeding time limit must be between 0 and 525600 minutes."),
    QT_TRANSLATE_NOOP("HttpServer", "Set location"),
    QT_TRANSLATE_NOOP("HttpServer", "Limit upload rate"),
    QT_TRANSLATE_NOOP("HttpServer", "Limit download rate"),
    QT_TRANSLATE_NOOP("HttpServer", "Rename torrent")
};

const struct { const char *source; const char *comment; } QBT_WEBUI_COMMENTED_TRANSLATIONS[] = {
    QT_TRANSLATE_NOOP3("HttpServer", "Other...", "Save Files to: Watch Folder / Default Folder / Other..."),
    QT_TRANSLATE_NOOP3("HttpServer", "Monday", "Schedule the use of alternative rate limits on ..."),
    QT_TRANSLATE_NOOP3("HttpServer", "Tuesday", "Schedule the use of alternative rate limits on ..."),
    QT_TRANSLATE_NOOP3("HttpServer", "Wednesday", "Schedule the use of alternative rate limits on ..."),
    QT_TRANSLATE_NOOP3("HttpServer", "Thursday", "Schedule the use of alternative rate limits on ..."),
    QT_TRANSLATE_NOOP3("HttpServer", "Friday", "Schedule the use of alternative rate limits on ..."),
    QT_TRANSLATE_NOOP3("HttpServer", "Saturday", "Schedule the use of alternative rate limits on ..."),
    QT_TRANSLATE_NOOP3("HttpServer", "Sunday", "Schedule the use of alternative rate limits on ..."),
    QT_TRANSLATE_NOOP3("HttpServer", "Upload Torrents", "Upload torrent files to qBittorent using WebUI")
};

#endif // EXTRA_TRANSLATIONS_H
