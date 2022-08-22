#include "FileCrawler.h"

FileCrawler::FileCrawler(ArgHandler flags)
{
  flags_ = flags;
  std::vector<std::string> paths = flags.GetPaths();
  for (int i = 0; i < paths.size(); i++)
  {
    locations.push_back(paths[i]);
    directories.push(paths[i]);
  }
}

ArgHandler FileCrawler::GetFlags()
{
  return flags_;
}

void FileCrawler::PrintLocations()
{
  std::cout << "-----------------------\n";
  std::cout << "Locations:\n";
  for (int i = 0; i < locations.size(); i++)
  {
    std::cout << locations[i] << std::endl;
  }
  std::cout << "-----------------------\n";
}

void FileCrawler::PrintDirectories()
{
  std::cout << "-----------------------\n";
  std::cout << "Directories:\n";
  std::queue<std::string> copy = directories;
  while (!copy.empty())
  {
    std::cout << copy.front() << std::endl;
    copy.pop();
  }
  std::cout << "-----------------------\n";
}

/* Gets the first character of the permissions string used with the -l flag
 */
std::string FileCrawler::GetFileType(mode_t st_mode)
{
  std::string type;
  if (S_ISDIR(st_mode))
  {
    type.append("d");
  }
  else if (S_ISFIFO(st_mode))
  {
    type.append("f");
  }
  else if (S_ISSOCK(st_mode))
  {
    type.append("s");
  }
  else if (S_ISLNK(st_mode))
  {
    type.append("l");
  }
  else if (S_ISBLK(st_mode))
  {
    type.append("b");
  }
  else if (S_ISCHR(st_mode))
  {
    type.append("c");
  }
  else
  {
    type.append("-");
  }
  return type;
}

/* Applies the protection masks to build the rest of the permissions string
 */
std::string FileCrawler::GetProtections(mode_t st_mode)
{
  std::string protections = GetFileType(st_mode);
  
  st_mode & S_IRUSR ? protections.append("r") : protections.append("-");
  st_mode & S_IWUSR ? protections.append("w") : protections.append("-");
  st_mode & S_IXUSR ? protections.append("x") : protections.append("-");
  st_mode & S_IRGRP ? protections.append("r") : protections.append("-");
  st_mode & S_IWGRP ? protections.append("w") : protections.append("-");
  st_mode & S_IXGRP ? protections.append("x") : protections.append("-");
  st_mode & S_IROTH ? protections.append("r") : protections.append("-");
  st_mode & S_IWOTH ? protections.append("w") : protections.append("-");
  st_mode & S_IXOTH ? protections.append("x") : protections.append("-");

  return protections;
}

/* Converts the time given by the stat object into a readable datetime format
 */
std::string FileCrawler::ToDateTime(time_t time)
{
  struct tm *datetime = localtime(&time);
  char date[20];
  strftime(date, sizeof(date), "%Y-%m-%d", datetime);
  return std::string(date);
}

std::string FileCrawler::GetLongFile(struct stat file_info, dirent* location)
{
    std::string long_file = GetProtections(file_info.st_mode);
    long_file.append("\t");
    long_file.append(std::to_string(file_info.st_nlink));
    long_file.append("\t");
    long_file.append(getpwuid(file_info.st_uid)->pw_name);
    long_file.append("\t");
    long_file.append(getgrgid(file_info.st_gid)->gr_name);
    long_file.append("\t");
    long_file.append(std::to_string(file_info.st_size));
    long_file.append("\t");
    long_file.append(std::string(ToDateTime(file_info.st_mtime)));
    long_file.append("\t");
    (S_ISLNK(file_info.st_mode) and flags_.GetFFlag()) ? long_file.append(GetLinkName(location)) : long_file.append(location->d_name);
    return long_file;
}

/*  Adds the proper string to the list of files. We either just send the file or the long form if -l flag present.
 *  Long format:
 *  permissions links user_id group_id file_size last_modified
 */ 
void FileCrawler::AddLocation(dirent* location)
{
  struct stat file_info;
  std::string path = directories.front();
  path.append("/");
  path.append(location->d_name);
  
  lstat(path.c_str(), &file_info);
  if (flags_.GetLFlag())
  {
    locations.push_back(GetLongFile(file_info, location));
  }
  else
  {
    (S_ISLNK(file_info.st_mode) and flags_.GetFFlag()) ? path = GetLinkName(location) : path = location->d_name;
    locations.push_back(path);
  }
}

/* Used in getting the relative path of a file
 */
int FileCrawler::GetNthLastOccurence(std::string str, char c, int n)
{
  int current_last = 0;
  for (int i = 0; i < n; i++)
  {
    current_last = str.find_last_of(c);
    str = str.substr(0, current_last);    
  }
  return current_last;
}

/* Formats a symbolic link in the format:
 * alias->relative_path
 */
std::string FileCrawler::GetLinkName(dirent* location)
{
  std::string link_name = location->d_name;
  link_name.append("->..");
  std::string real_path = GetRealPath(location);
  int start = GetNthLastOccurence(real_path, '/', 2);
  real_path = real_path.substr(start, real_path.length() - start);
  link_name.append(real_path);
  return link_name;
}

/* Gets the absolute path
 */
std::string FileCrawler::GetRealPath(dirent* location)
{
  char real_path[CHAR_MAX];
  char* link = realpath(location->d_name, real_path);
  return std::string(real_path);
}

/* Formats and adds the directory to the list of directories
 */
void FileCrawler::AddDirectory(dirent* location)
{
  struct stat file_info;
  lstat(location->d_name, &file_info);
  std::string new_directory = directories.front();
  new_directory.append("/");
  new_directory.append(location->d_name);
  if (flags_.GetFFlag() and S_ISLNK(file_info.st_mode))
  {
    new_directory = GetRealPath(location);
  }
  directories.push(new_directory);
}

/* check in link case if link points to a directory
 */
void FileCrawler::ReadLocation(dirent* location)
{
  std::string path = location->d_name;
  if (path != "." and path != ".." and path != "../." and path != "../.." and path != "./." and path != "./..")
  {
    if (!flags_.GetAFlag() and path[0] == '.')
    {
      return;
    }
    if (location->d_type == DT_REG)
    {
      AddLocation(location);
    }
    else if (location->d_type == DT_DIR)
    {
      AddLocation(location);
      if (!flags_.GetDFlag())
      {
        AddDirectory(location);
      }
    }
    else if (location->d_type == DT_LNK)
    {
      AddLocation(location);
      struct stat file_info;
      lstat(GetRealPath(location).c_str(), &file_info);
      if (!flags_.GetDFlag() and flags_.GetFFlag() and S_ISDIR(file_info.st_mode))
      {
        AddDirectory(location);
      }
    }
  }
}

void FileCrawler::Crawl()
{
  while(!directories.empty())
  {
    std::string curr_path = directories.front();
    DIR* directory = opendir(curr_path.c_str());
    if (directory == NULL)
    {
      std::cout << curr_path  << " results in errno: " << errno << std::endl;
    }
    else
    {
      dirent* location;
      while ((location = readdir(directory)) != NULL)
      {
        ReadLocation(location);
      }
    }
    closedir(directory);
    directories.pop();
  }
}
