/*
 * This file is part of libdvdread.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 */

#include "filesystem.h"

#include <stdlib.h>

dvd_reader_file_open file_open_default(void);
dvd_reader_dir_open dir_open_default(void);
dvd_reader_stat stat_default(void);

void default_filesystem_close(void *fs) {
  free(fs);
}

dvd_reader_filesystem* InitInternalFilesystem() {
  dvd_reader_filesystem* fs = calloc( 1, sizeof(dvd_reader_filesystem));
  if (!fs) {
    return NULL;
  }
  fs->dir_open = dir_open_default();
  fs->stat = stat_default();
  fs->file_open = file_open_default();
  fs->close = default_filesystem_close;
  return fs;
}
