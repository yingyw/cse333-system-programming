/*
 * Copyright 2011 Steven Gribble
 *
 *  This file is part of the UW CSE 333 course project sequence
 *  (333proj).
 *
 *  333proj is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  333proj is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with 333proj.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>  // for stat()
#include <sys/stat.h>   // for stat()
#include <unistd.h>     // for stat()

#include "./fileindexutil.h"   // for class CRC32.
#include "./FileIndexReader.h"

extern "C" {
  #include "libhw1/CSE333.h"
}

namespace hw3 {

FileIndexReader::FileIndexReader(std::string filename,
                                 bool validate) {
  // Stash a copy of the index file's name.
  filename_ = filename;

  // Open a (FILE *) associated with filename.  Crash on error.
  file_ = fopen(filename_.c_str(), "rb");
  Verify333(file_ != nullptr);

  // Make the (FILE *) be unbuffered.  ("man setbuf")
  // MISSING:
  setbuf(file_, NULL);


  // Read the entire file header and convert to host format.
  // MISSING:
  int read = fread(&header_, sizeof(header_), 1, file_);
  Verify333(read == 1);
  header_.toHostFormat();

  // Verify that the magic number is correct.  Crash if not.
  // MISSING:
  Verify333(header_.magic_number == MAGIC_NUMBER);
  // Make sure the index file's length lines up with the header fields.
  struct stat f_stat;
  Verify333(stat(filename_.c_str(), &f_stat) == 0);
  Verify333((HWSize_t) f_stat.st_size ==
            (HWSize_t) (sizeof(IndexFileHeader) +
                        header_.doctable_size +
                        header_.index_size));
  if (validate) {
    // Re-calculate the checksum, make sure it matches that in the header.
    // Use fread() and pass the bytes you read into the crcobj.
    // Note you don't need to do any host/network order conversion,
    // since we're doing this byte-by-byte.
    CRC32 crcobj;
    uint8_t buf[512];
    HWSize_t left_to_read = header_.doctable_size + header_.index_size;
    while (left_to_read > 0) {
      // MISSING:
      uint32_t read = fread(buf, 1, 1, file_);
      Verify333(read == 1);
      for (HWSize_t i = 0; i < read; i++) {
        crcobj.FoldByteIntoCRC(buf[i]);
      }
      left_to_read -= read;
    }
    Verify333(crcobj.GetFinalCRC() == header_.checksum);
  }

  // Everything looks good; we're done!
}

FileIndexReader::~FileIndexReader() {
  // Close the (FILE *).
  Verify333(fclose(file_) == 0);
}

DocTableReader FileIndexReader::GetDocTableReader() {
  // The docid->name mapping starts at offset sizeof(IndexFileHeader) in
  // the index file.  Be sure to dup the (FILE *) rather than sharing
  // it across objects, just so that we don't end up with the possibility
  // of threads contending for the (FILE *) and associated with race
  // conditions.
  FILE *fdup = FileDup(file_);
  IndexFileOffset_t file_offset = sizeof(IndexFileHeader);
  return DocTableReader(fdup, file_offset);
}

IndexTableReader FileIndexReader::GetIndexTableReader() {
  // The index (word-->docid table) mapping starts at offset
  // (sizeof(IndexFileHeader) + doctable_size_) in the index file.  Be
  // sure to dup the (FILE *) rather than sharing it across objects,
  // just so that we don't end up with the possibility of threads
  // contending for the (FILE *) and associated race conditions.
  return IndexTableReader(FileDup(file_),
                          sizeof(IndexFileHeader) + header_.doctable_size);
}

}  // namespace hw3
