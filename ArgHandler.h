#ifndef ARGHANDLER_H
#define ARGHANDLER_H

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <vector>

class ArgHandler
{
public:

  // getters
  bool GetAFlag();
  bool GetDFlag();
  bool GetFFlag();
  bool GetLFlag();
  bool GetHFlag();
  std::vector<std::string> GetPaths();

  // utility functions
  std::string ToString();
  void PrintPaths();

  // operators
  friend std::ostream& operator << (std::ostream& out, ArgHandler handler);

  // other functions
  void ParseArgs(int argc, char* argv[]);

private:
  // field members
  bool a_flag_ = false;
  bool d_flag_ = false;
  bool f_flag_ = false;
  bool l_flag_ = false;
  bool h_flag_ = false;
  std::vector<std::string> paths_;

  // other utility functions
  void PrintUsage();
  bool IsInputArg(char* argv[], int index); 
};

#endif
