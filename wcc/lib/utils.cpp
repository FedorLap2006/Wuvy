#include <utils.h>

bool isall_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool isall_punct(const std::string& s) {
	std::string::const_iterator it = s.begin();
  while (it != s.end() && std::ispunct(*it)) ++it;
  return !s.empty() && it == s.end();	
}