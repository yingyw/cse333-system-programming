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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "./QueryProcessor.h"

static void Usage(char *progname) {
  std::cerr << "Usage: " << progname << " [index files+]" << std::endl;
  exit(EXIT_FAILURE);
}

// Your job is to implement the entire filesearchshell.cc
// functionality. We're essentially giving you a blank screen to work
// with; you need to figure out an appropriate design, to decompose
// the problem into multiple functions or classes if that will help,
// to pick good interfaces to those functions/classes, and to make
// sure that you don't leak any memory.
//
// Here are the requirements for a working solution:
//
// The user must be able to run the program using a command like:
//
//   ./filesearchshell ./foo.idx ./bar/baz.idx /tmp/blah.idx [etc]
//
// i.e., to pass a set of filenames of indices as command line
// arguments. Then, your program needs to implement a loop where
// each loop iteration it:
//
//  (a) prints to the console a prompt telling the user to input the
//      next query.
//
//  (b) reads a white-space separated list of query words from
//      std::cin, converts them to lowercase, and constructs
//      a vector of c++ strings out of them.
//
//  (c) uses QueryProcessor.cc/.h's QueryProcessor class to
//      process the query against the indices and get back a set of
//      query results.  Note that you should instantiate a single
//      QueryProcessor  object for the lifetime of the program, rather
//      than  instantiating a new one for every query.
//
//  (d) print the query results to std::cout in the format shown in
//      the transcript on the hw3 web page.
//
// Also, you're required to quit out of the loop when std::cin
// experiences EOF, which a user passes by pressing "control-D"
// on the console.  As well, users should be able to type in an
// arbitrarily long query -- you shouldn't assume anything about
// a maximum line length.  Finally, when you break out of the
// loop and quit the program, you need to make sure you deallocate
// all dynamically allocated memory.  We will be running valgrind
// on your filesearchshell implementation to verify there are no
// leaks or errors.
//
// You might find the following technique useful, but you aren't
// required to use it if you have a different way of getting the
// job done.  To split a std::string into a vector of words, you
// can use a std::stringstream to get the job done and the ">>"
// operator. See, for example, "gnomed"'s post on stackoverflow for
// his example on how to do this:
//
//   http://stackoverflow.com/questions/236129/c-how-to-split-a-string
//
// (Search for "gnomed" on that page.  He uses an istringstream, but
// a stringstream gets the job done too.)
//
// Good luck, and write beautiful code!
static void PrintResult(std::vector<hw3::QueryProcessor::QueryResult> result);
static void toLowerCase(std::string &s);
int main(int argc, char **argv) {
  if (argc < 2) Usage(argv[0]);
  std::list<string> index;
  // load arguments
  for (int i = 1; i <argc; i++) {
  	 std::string current = argv[i];
     index.push_back(current);
  }
  hw3::QueryProcessor qp(index, true);
  while (1) {
  	std::cout << "Enter query: \n";
  	std::string word, input;
  	// get user input
    getline(std::cin, input);
  	std::stringstream ss(input, std::stringstream::in);
    std::vector<std::string> in;
    while (ss >> word) {
      toLowerCase(word);
      in.push_back(word);
    }
    // check end case
    if (in.size() == 0)
      break;
    std::vector<hw3::QueryProcessor::QueryResult> result = qp.ProcessQuery(in);
    PrintResult(result);

  }

  return EXIT_SUCCESS;
}
// print result
static void PrintResult(std::vector<hw3::QueryProcessor::QueryResult> result) {
	  // if not found
    if (result.size() == 0) {
      std::cout << "  [no results]" << std::endl;
      return;
    }
    // if found, print it out
    for (size_t i = 0; i < result.size(); i++) {
      std::cout << "  " << result[i].document_name << " (" << result[i].rank << ")" << std::endl;
    }
}
// tansfer input to lowercase
static void toLowerCase(std::string &s) {
  transform(s.begin(), s.end(), s.begin(), ::tolower);
}







