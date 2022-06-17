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

#ifndef DVDREAD_FILESYSTEM_H_
#define DVDREAD_FILESYSTEM_H_

#include <stdint.h>

/*
 * directory access
 */

// short version of dirent struct - only contains the parts we care about.
typedef struct
{
    char    d_name[256];
} dvd_dirent_t;

typedef struct dvd_filesystem_dir_s dvd_dir_h;
struct dvd_filesystem_dir_s
{
    void* internal;
    void (*close)(dvd_dir_h *dir);
    int (*read)(dvd_dir_h *dir, dvd_dirent_t *entry);
};
typedef dvd_dir_h* (*dvd_reader_dir_open) (const char* dirname);

/*
 * Stat access
 */

typedef struct
{
    int size;
    int is_blk;
    int is_chr;
    int is_reg;
    int is_dir;
} dvdstat_t;

typedef int (*dvd_reader_stat) (const char *path, dvdstat_t* statbuf);


/*
 * file access
 */

typedef struct dvd_filesystem_file_s dvd_file_h;
struct dvd_filesystem_file_s
{
    void* internal;
    int64_t (*close) (dvd_file_h *file);
    int64_t (*seek)  (dvd_file_h *file, int64_t offset, int32_t origin);
    int64_t (*read)  (dvd_file_h *file, char *buf, int64_t size);
};

typedef dvd_file_h* (*dvd_reader_file_open)(const char* filename, const char *mode);


/*
 * Filesystem implementation
 */

typedef struct
{
    void (*close) (void *);
    int (*stat) (const char *path, dvdstat_t* statbuf);
    dvd_dir_h* (*dir_open) (const char* dirname);
    dvd_file_h* (*file_open) (const char* filename, const char *mode);
} dvd_reader_filesystem;

#endif /* DVDREAD_FILESYSTEM_H_ */
