/*
 * This file is part of libdvdread
 * Copyright (C) 2022 VideoLAN
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <windows.h>
#include "../msvc/contrib/win32_cs.h"

#include <dvdread/dvd_filesystem.h>


static int64_t _file_close_win32(dvd_file_h *file)
{
    if (file) {
        int64_t ret = close((int)(intptr_t)file->internal);
        free(file);
        return ret;
    }
    return 0;
}

static int64_t _file_seek_win32(dvd_file_h *file, int64_t offset, int32_t origin)
{
    off_t result = _lseeki64((int)(intptr_t)file->internal, offset, origin);
    if (result == (off_t)-1) {
        return -1;
    }
    return (int64_t)result;
}

static int64_t _file_read_win32(dvd_file_h *file, char *buf, int64_t size)
{
    ssize_t got, result;

    if (size <= 0) {
        return 0;
    }

    for (got = 0; got < (ssize_t)size; got += result) {
        result = read((int)(intptr_t)file->internal, buf + got, size - got);
        if (result < 0) {
            if (errno != EINTR) {
                break;
            }
            result = 0;
        } else if (result == 0) {
            // hit EOF.
            break;
        }
    }
    return (int64_t)got;
}


static dvd_file_h* _file_open_win32(const char* filename, const char *cmode)
{
    dvd_file_h *file;
    int fd    = -1;
    wchar_t *wpath;

    wpath = _utf8_to_wchar(filename);
    if (!wpath) {
        return NULL;
    }
    if (strchr(cmode, 'w')) {
        // write is not supported/needed
        free(wpath);
        return NULL;
    } else {
        fd = _wopen(wpath, O_RDONLY | O_BINARY);
        free(wpath);
    }

    file = calloc(1, sizeof(dvd_file_h));
    if (!file) {
        close(fd);
        return NULL;
    }

    file->close = _file_close_win32;
    file->read = _file_read_win32;
    file->seek  = _file_seek_win32;
    file->internal = (void*)(intptr_t)fd;

    return file;
}

dvd_file_h* (*file_open)(const char* filename, const char *mode) = _file_open_win32;

dvd_reader_file_open file_open_default(void)
{
    return _file_open_win32;
}
