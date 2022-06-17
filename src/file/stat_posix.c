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
#include <sys/stat.h>

#include <dvdread/dvd_filesystem.h>

static int _stat_posix(const char *path, dvdstat_t* statbuf)
{
    struct stat posixstatbuf;
    int ret = stat(path, &posixstatbuf);
    statbuf->size = posixstatbuf.st_size;
    statbuf->is_blk = S_ISBLK(posixstatbuf.st_mode);
    statbuf->is_chr = S_ISCHR(posixstatbuf.st_mode);
    statbuf->is_reg = S_ISREG(posixstatbuf.st_mode);
    statbuf->is_dir = S_ISDIR(posixstatbuf.st_mode);
    return ret;
}

dvd_reader_stat stat_default(void)
{
    return _stat_posix;
}
