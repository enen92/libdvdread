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

#include <dvdread/dvd_filesystem.h>

#ifdef __ANDROID__
# undef  lseek
# define lseek lseek64
# undef  off_t
# define off_t off64_t
#endif


static int64_t _file_close(dvd_file_h *file)
{
    if (file) {
        int64_t ret = close((int)(intptr_t)file->internal);
        free(file);
        return ret;
    }
    return 0;
}

static int64_t _file_seek(dvd_file_h *file, int64_t offset, int32_t origin)
{
    off_t result = lseek((int)(intptr_t)file->internal, offset, origin);
    if (result == (off_t)-1) {
        return -1;
    }
    return (int64_t)result;
}

static int64_t _file_read(dvd_file_h *file, char *buf, int64_t size)
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


static dvd_file_h* _file_open(const char* filename, const char *cmode)
{
    dvd_file_h *file;
    int fd    = -1;
    int flags = 0;
    int mode  = 0;

    if (strchr(cmode, 'w')) {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
        mode  = S_IRUSR | S_IWUSR;
    } else {
        #if defined(__OS2__)
        flags = O_RDONLY | O_BINARY;
        #else
        flags = O_RDONLY;
        #endif
    }

#ifdef O_CLOEXEC
    flags |= O_CLOEXEC;
#endif
#ifdef O_BINARY
    flags |= O_BINARY;
#endif

    if ((fd = open(filename, flags, mode)) < 0) {
        return NULL;
    }

    file = calloc(1, sizeof(dvd_file_h));
    if (!file) {
        close(fd);
        return NULL;
    }

    file->close = _file_close;
    file->read = _file_read;
    file->seek  = _file_seek;
    file->internal = (void*)(intptr_t)fd;

    return file;
}

dvd_file_h* (*file_open)(const char* filename, const char *mode) = _file_open;

dvd_reader_file_open file_open_default(void)
{
    return _file_open;
}
