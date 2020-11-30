#include <string>
#include <vector>
#include <fstream>

std::string trim(std::string& s);

void extractArgs(std::ifstream&, int mode, std::vector<std::string>& args);

std::string vector_to_str(std::vector<std::string>& v);
