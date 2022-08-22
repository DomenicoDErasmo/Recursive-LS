#include "ArgHandler.h"

bool ArgHandler::GetAFlag()
{
  return a_flag_;
}

bool ArgHandler::GetDFlag()
{
  return d_flag_;
}

bool ArgHandler::GetFFlag()
{
  return f_flag_;
}

bool ArgHandler::GetLFlag()
{
  return l_flag_;
}

bool ArgHandler::GetHFlag()
{
  return h_flag_;
}

std::vector<std::string> ArgHandler::GetPaths()
{
  return paths_;
}

std::string ArgHandler::ToString()
{
  std::string result = "a_flag_: ";
  a_flag_ ? result.append("true") : result.append("false");
  result.append("\nd_flag: ");
  d_flag_ ? result.append("true") : result.append("false");
  result.append("\nf_flag: ");
  f_flag_ ? result.append("true") : result.append("false");
  result.append("\nl_flag: ");
  l_flag_ ? result.append("true") : result.append("false");
  result.append("\nh_flag: ");
  h_flag_ ? result.append("true") : result.append("false");
  
  return result;
}

void ArgHandler::PrintPaths()
{
  for (int i = 0; i < paths_.size(); i++)
  {
    std::cout << paths_[i] << ", ";
  }
  std::cout << "\n";
}

std::ostream& operator << (std::ostream& out, ArgHandler handler)
{
  out << handler.ToString();
  return out;
}

bool ArgHandler::IsInputArg(char* argv[], int index)
{
  return argv[index][0] != '-';
}

void ArgHandler::ParseArgs(int argc, char* argv[])
{
  int opt;
  while ((opt = getopt(argc, argv, "adflh")) != -1)
  {
    switch(opt)
    {
      case 'a':
        a_flag_ = true;
        break;
      case 'd':
        d_flag_ = true;
        break;
      case 'f':
        f_flag_ = true;
        break;
      case 'l':
        l_flag_ = true;
        break;
      case 'h':
        h_flag_ = true;
        PrintUsage();
        break;
    }
  }
  for (int i = optind; i < argc; i++)
  {
    char real_path[CHAR_MAX];
    char* link = realpath(argv[i], real_path);
    paths_.push_back(std::string(real_path));
  }
}

void ArgHandler::PrintUsage()
{
  std::cout << "Usage: dirls [(-adflh)+) (dir)]*\n";
  std::cout << "\t\t-a: include dot files\n";
  std::cout << "\t\t-d: only this directory\n";
  std::cout << "\t\t-f: follow symbolic links\n";
  std::cout << "\t\t-l: long form\n";
  std::cout << "\t\t-h: prints this message\n";
}
