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

/**
 * The dvd_dirent_t struct abstracts dirent usage from the respective platform implementation.
 * For this reason, it only includes the parts the lib cares about (in this case d_name - the directory name)
 */
typedef struct
{
    char    d_name[256];
} dvd_dirent_t;

typedef struct dvd_filesystem_dir_s dvd_dir_h;
/**
 * Abstraction for directory access
 * internal - opaque handler internal to the specific implementation used to store the data type of directory stream objects.
 * This is tipically a dir handler (e.g. a DIR* in posix)
 * close(dvd_dir_h *dir) - called to close the directory and cleanup any alloc'd dir structures
 * read(dvd_dir_h *dir, dvd_dirent_t *entry) - provided the dvd_dirent_t, reads the respective directory and returns -1 if the
 * directory could not be read, 0 if successfully read.
 */
struct dvd_filesystem_dir_s
{
    void* internal;
    void (*close)(dvd_dir_h *dir);
    int (*read)(dvd_dir_h *dir, dvd_dirent_t *entry);
};
/**
 * Abstraction for dir open
 * dirname - path to the directory
 */
typedef dvd_dir_h* (*dvd_reader_dir_open) (const char* dirname);

/*
 * Stat access
 */

/**
 * Abstraction for stat buffer structure
 * size - size of the stat'd file
 * is_blk - 1 if the file is a block device, 0 otherwise
 * is_chr - 1 if the file is a chr device, 0 otherwise
 * is_reg - 1 if the file is a regular file, 0 otherwise
 * is_dir - 1 if the file is a directory, 0 otherwise
 */
typedef struct
{
    int size;
    int is_blk;
    int is_chr;
    int is_reg;
    int is_dir;
} dvdstat_t;

/**
 * Abstraction for stat access
 * path - the path for the file/dir
 * statbuf - the dvdstat_t struct to be filled
 *
 * returns the result of the stat operation. -1 means stat failure.
 */
typedef int (*dvd_reader_stat) (const char *path, dvdstat_t* statbuf);


/*
 * file access
 */

/**
 * Abstraction for file access
 * internal - opaque handler internal to the specific implementation used to store the data type of the file object.
 * This is tipically a file reference (e.g. the file descriptor/fd)
 * close(dvd_file_h *file) - called to close the file and cleanup any alloc'd file structs
 * seek(dvd_file_h *file, int64_t offset, int32_t origin) - used to seek into the given file (provided the offset and the origin). Returns
 * the position on the file after seek.
 * read(dvd_file_h *file, char *buf, int64_t size) - used to read the file into the passed buffer, given the read size. Returns the read
 * read from the file
 */
typedef struct dvd_filesystem_file_s dvd_file_h;
struct dvd_filesystem_file_s
{
    void* internal;
    int64_t (*close) (dvd_file_h *file);
    int64_t (*seek)  (dvd_file_h *file, int64_t offset, int32_t origin);
    int64_t (*read)  (dvd_file_h *file, char *buf, int64_t size);
};

/**
 * Abstraction for file open
 * filename - full path to the file
 * mode - openmode as a string (e.g. "rb")
 */
typedef dvd_file_h* (*dvd_reader_file_open)(const char* filename, const char *mode);


/*
 * Filesystem implementation
 */


/**
 * Groups all filesystem operations into a common struct. This is the struct external applications should override to
 * provide custom filesystem implementations:
 * close() -> called to destroy the filesystem implementation (free any alloc'd structs)
 * stat(const char *path, dvdstat_t* statbuf) - stat a file/dir provided the statbuf (see dvd_reader_stat), initializes the dvdstat_t.
 * dir_open(const char* dirname) - open the provided dir, initializes dvd_dir_h (see dvd_reader_dir_open)
 * file_open(const char* filename, const char *mode) - open a file, initializes dvd_file_h (see dvd_reader_file_open)
 */
typedef struct
{
    void (*close) (void *);
    int (*stat) (const char *path, dvdstat_t* statbuf);
    dvd_dir_h* (*dir_open) (const char* dirname);
    dvd_file_h* (*file_open) (const char* filename, const char *mode);
} dvd_reader_filesystem;

#endif /* DVDREAD_FILESYSTEM_H_ */
