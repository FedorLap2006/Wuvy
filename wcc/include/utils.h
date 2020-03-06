#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>

#define errorf(module, message, file, line, index) \
	std::cerr << "Error at [" + file + " (" << line << ", " << index << ")] in " << #module << ": " << message << std::endl
	//std::cout << "error " << "(" << file << ", " << line + 1 << " at " << sidx << " )" << msg << std::endl
	

#define error(module, message) \
	std::cerr << "Error in " << #module << ": " << message << std::endl
	//std::cout << "error " << msg << std::endl

bool isall_number(const std::string& s);
bool isall_punct(const std::string& s);
#endif