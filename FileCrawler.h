#ifndef FILECRAWLER_H
#define FILECRAWLER_H

#include <dirent.h>
#include <vector>
#include <queue>
#include <string>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include "ArgHandler.h"

class FileCrawler
{
public:

  // constructors
  FileCrawler(ArgHandler flags);

  // getters
  ArgHandler GetFlags();

  // other functions
  void PrintLocations();
  void PrintDirectories();
  void Crawl();

private:
  // utility functions
  std::string GetFileType(mode_t st_mode);
  std::string GetProtections(mode_t st_mode);
  std::string ToDateTime(time_t time);
  std::string GetLinkName(dirent* location);
  std::string GetLongFile(struct stat file_info, dirent* location);
  std::string GetRealPath(dirent* location);
  int GetNthLastOccurence(std::string str, char c, int n);
  void AddLocation(dirent* location);
  void AddLink(dirent* location);
  void AddDirectory(dirent* location);
  void ReadLocation(dirent* location);
  
  // fields
  ArgHandler flags_;
  std::vector<std::string> locations;
  std::queue<std::string> directories;
};

#endif
