#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>

using namespace std;

string trim(string& s){
    size_t first = s.find_first_not_of(' ');
    if (string::npos == first)
    {
        return s;
    }
    size_t last = s.find_last_not_of(' ');
    return s.substr(first, (last - first + 1));
}

void extractArgs(std::ifstream& modeFile, int mode, std::vector<string>& args){
    // Expects a YAMLish file
    // Each comment or empty line is ignored

    string buffer;

    string begin = to_string(mode) + ":";
    int len = begin.size();
    while (getline(modeFile, buffer)){
        if (buffer.rfind(begin, 0) == 0){
            break;
        }
    }
    
    while (getline(modeFile, buffer)){
        if (buffer.rfind(':') != -1)
            break;
        if (buffer.size() > 0 && buffer[0] != '#'){
            args.push_back(trim(buffer));
        }
    }
}

string vector_to_str(std::vector<std::string>& v){
    ostringstream oss;
    
    for (int i = 0; i < v.size(); i++) {
        oss << v.at(i) << ' ';
    }
    return oss.str();
}