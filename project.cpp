// Domenico D'Erasmo
// 9 May 2021
// Recursive LS Submission

#include <iostream>
#include "ArgHandler.h"
#include "FileCrawler.h"

int main(int argc, char* argv[])
{
  ArgHandler handler;
  handler.ParseArgs(argc, argv);
  FileCrawler crawler(handler);

  crawler.Crawl();
  crawler.PrintLocations();

  return 0;
}

