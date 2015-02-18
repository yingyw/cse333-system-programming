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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <string>

#include "./filelayout.h"
#include "./fileindexutil.h"
#include "./test_fileindexreader.h"
#include "./test_suite.h"

namespace hw3 {

TEST_F(Test_FileIndexReader, TestFileIndexReaderBasic) {
  // Open up the FILE * for ./unit_test_indices/enron.idx
  std::string idx("./unit_test_indices/enron.idx");
  FileIndexReader fir(idx);

  // Make sure the header fields line up correctly with the file size.
  struct stat f_stat;
  ASSERT_EQ(stat(idx.c_str(), &f_stat), 0);
  ASSERT_EQ((HWSize_t) f_stat.st_size,
            (HWSize_t) (get_doctable_size(&fir) +
                        get_index_size(&fir) +
                        sizeof(IndexFileHeader)));
  HW3Addpoints(10);

  // Manufacture a DocTableReader.
  DocTableReader dtr = fir.GetDocTableReader();

  // Try a lookup with it.
  std::string str;
  bool success = dtr.LookupDocID((DocID_t) 10, &str);
  ASSERT_TRUE(success);
  ASSERT_EQ(std::string("test_tree/enron_email/107."), str);

  // Done!.
  HW3Addpoints(20);
}

}  // namespace hw3
