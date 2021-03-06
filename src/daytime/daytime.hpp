#ifndef __DAYTIME_HPP__
#define __DAYTIME_HPP__

#include <ctime>
#include <string>

inline std::string make_daytime_string()
{
  using namespace std;
  auto now = time(nullptr);
  return std::string(ctime(&now));
}


#endif
