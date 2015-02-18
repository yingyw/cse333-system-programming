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
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "libhw1/CSE333.h"
#include "memindex.h"
#include "filecrawler.h"

static void Usage(void);

int main(int argc, char **argv) {
  if (argc != 2)
    Usage();

  // Implement searchshell!  We're giving you very few hints
  // on how to do it, so you'll need to figure out an appropriate
  // decomposition into functions as well as implementing the
  // functions.  There are several major tasks you need to build:
  //
  //  - crawl from a directory provided by argv[1] to produce and index
  //  - prompt the user for a query and read the query from stdin, in a loop
  //  - split a query into words (check out strtok_r)
  //  - process a query against the index and print out the results
  //
  // When searchshell detects end-of-file on stdin (cntrl-D from the
  // keyboard), searchshell should free all dynamically allocated
  // memory and any other allocated resources and then exit.
  DocTable doc_table;
  MemIndex mem_index;
  printf("Indexing '%s'\n", argv[1]);
  size_t size = 100;
  CrawlFileTree(argv[1], &doc_table, &mem_index);
  char *target = (char*) malloc(sizeof(char) *size);
  // running for client for input
  while (1) {
    printf("enter query:\n");
    size_t readsize = 0;
    // shut down program for control-D
    if ((readsize = getline(&target, &size, stdin)) == EOF) {
      break;
    }
    // convert input words to lower case
    for (int i = 0; i < strlen(target); i++) {
      target[i] = tolower(target[i]);
    }

    char *query[readsize], *leftsize, *temp;
    int i = 0;
    temp = strtok_r(target, " \n", &leftsize);
    while (temp != NULL) {
       query[i] = temp;
       i++;
       temp = strtok_r(NULL, " \n", &leftsize);
    }

    LinkedList linklist = MIProcessQuery(mem_index, query, i);
    if (linklist != NULL && NumElementsInLinkedList(linklist) != 0) {
      LLIter itr = LLMakeIterator(linklist, 0);
       // go over to find match word
       // and print name and rank of this input word
       do {
        SearchResult *searchresult;
        LLIteratorGetPayload(itr, (LLPayload_t)&searchresult);
        char *name = DTLookupDocID(doc_table, searchresult->docid);
        printf("   %s (%d)\n", name, searchresult->rank);
       } while (LLIteratorNext(itr));
       LLIteratorFree(itr);
       FreeLinkedList(linklist, &free);
    } else if (linklist == NULL) {
      continue;
    } else if (NumElementsInLinkedList(linklist) == 0) {
      FreeLinkedList(linklist, &free);
      continue;
    }
  }
  free(target);
  FreeDocTable(doc_table);
  FreeMemIndex(mem_index);
  return EXIT_SUCCESS;
}

static void Usage(void) {
  fprintf(stderr, "Usage: ./searchshell <docroot>\n");
  fprintf(stderr,
          "where <docroot> is an absolute or relative " \
          "path to a directory to build an index under.\n");
  exit(-1);
}



