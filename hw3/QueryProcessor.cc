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

#include <iostream>
#include <algorithm>
#include <utility>
#include "./QueryProcessor.h"

extern "C" {
  #include "./libhw1/CSE333.h"
}
namespace hw3 {

QueryProcessor::QueryProcessor(list<string> indexlist, bool validate) {
  // Stash away a copy of the index list.
  indexlist_ = indexlist;
  arraylen_ = indexlist_.size();
  Verify333(arraylen_ > 0);

  // Create the arrays of DocTableReader*'s. and IndexTableReader*'s.
  dtr_array_ = new DocTableReader *[arraylen_];
  itr_array_ = new IndexTableReader *[arraylen_];

  // Populate the arrays with heap-allocated DocTableReader and
  // IndexTableReader object instances.
  list<string>::iterator idx_iterator = indexlist_.begin();
  for (HWSize_t i = 0; i < arraylen_; i++) {
    FileIndexReader fir(*idx_iterator, validate);
    dtr_array_[i] = new DocTableReader(fir.GetDocTableReader());
    itr_array_[i] = new IndexTableReader(fir.GetIndexTableReader());
    idx_iterator++;
  }
}

QueryProcessor::~QueryProcessor() {
  // Delete the heap-allocated DocTableReader and IndexTableReader
  // object instances.
  Verify333(dtr_array_ != nullptr);
  Verify333(itr_array_ != nullptr);
  for (HWSize_t i = 0; i < arraylen_; i++) {
    delete dtr_array_[i];
    delete itr_array_[i];
  }

  // Delete the arrays of DocTableReader*'s and IndexTableReader*'s.
  delete[] dtr_array_;
  delete[] itr_array_;
  dtr_array_ = nullptr;
  itr_array_ = nullptr;
}
std::vector<QueryProcessor::QueryResult>
QueryProcessor::ProcessQuery(const vector<string> &query) {
  Verify333(query.size() > 0);
  vector<QueryProcessor::QueryResult> finalresult;


  //MISSING:
  HWSize_t first = 0;
  while (first < query.size()) {    
    //get the word
    std::string word = query[first];
    vector<QueryProcessor::QueryResult> query_result = finalresult;
    for (HWSize_t j = 0; j < arraylen_; j++) {
      // get current index;
      IndexTableReader *index_table_reader = itr_array_[j];
      DocTableReader *doc_table_reader = dtr_array_[j];
      // get doc id table
      DocIDTableReader *doc_id_table = index_table_reader -> LookupWord(word);

      // get doc id table related to word
      if (doc_id_table == NULL) {
        continue;
      }
      // get doc id list
      list<docid_element_header> doc_list = doc_id_table->GetDocIDList();
      //go through each element in the list
      list<docid_element_header>::iterator itr;
      for (auto itr = doc_list.begin(); itr != doc_list.end(); itr++) {
        docid_element_header p_pair = *itr;
        uint64_t doc_id = p_pair.docid;
        uint32_t num_pos = p_pair.num_positions;
        std::string filename;
       
        doc_table_reader->LookupDocID(doc_id, &filename);
        // check if this is the first word
        uint32_t count = 0;
        // if the docid alreday in the list, then increase the rank
        while (count < finalresult.size()) {
          if (filename.compare(finalresult[count].document_name) == 0
              && first != 0) {
            finalresult[count].rank += num_pos;
          }
          count++;
        }
        if (first == 0) {
          // if the docid not in the list, add it into list
          QueryResult cur_result = {filename, num_pos};
          finalresult.push_back(cur_result);
        }        
      }
      delete doc_id_table;

    }
   
    // check if rank in document is  changed
    for (uint32_t docnumber = 0; docnumber < query_result.size(); docnumber++) {
      uint32_t count = 0;
      while (count < finalresult.size()) {
        // if in the same document and also match the rank of it
        if ((query_result[docnumber].document_name.compare(finalresult[count].document_name) == 0)
            & (query_result[docnumber].rank == finalresult[count].rank)){
            finalresult.erase(finalresult.begin() + count);
        }
        count++;
      }
    }
    first++;  
  }
  // Sort the final results.
  std::sort(finalresult.begin(), finalresult.end());
  return finalresult;
}

}  // namespace hw3






































