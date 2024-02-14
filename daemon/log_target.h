/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2013 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#pragma once

#include <syslog.h>
#include <cstring>

class LogTarget {
public:
    int mMinLogLevel = LOG_INFO;
    bool mUseSyslog = false;

public:
    LogTarget(int minLogLevel = LOG_INFO, bool useSyslog = false);

    void log(int level, const char *format, ...);

    static constexpr auto strLevel(int level)
    {
        switch (level) {
        case LOG_EMERG:   return "Emergency";
        case LOG_ALERT:   return "Alert";
        case LOG_CRIT:    return "Critical";
        case LOG_ERR:     return "Error";
        case LOG_WARNING: return "Warning";
        case LOG_NOTICE:  return "Notice";
        case LOG_INFO:    return "Info";
        case LOG_DEBUG:   return "Debug";
        }

        return ""; // fallthrough
    }

    static constexpr auto levelFromStr(const char* str) {
        if (str == nullptr) {
            // error: null string
            return -2;
        }

        if (std::strcmp(str, "error") == 0) {
            return LOG_ERR;
        }
        if (std::strcmp(str, "warning") == 0) {
            return LOG_WARNING;
        }
        if (std::strcmp(str, "notice") == 0) {
            return LOG_NOTICE;
        }
        if (std::strcmp(str, "info") == 0) {
            return LOG_INFO;
        }
        if (std::strcmp(str, "debug") == 0) {
            return LOG_DEBUG;
        }

        // error: unknown log level string
        return -1;
    }
};
