#include <string>
#include <algorithm>



namespace Utils{

//String splitting with delimiter
std::vector<std::string> splitString(std::string str, char delim){
    std::vector<std::string> res;
    int occurrences = std::count(str.begin(), str.end(), delim);
    int i = 0;
    while(i <= occurrences){
        std::string token = str.substr(0, str.find(delim));
        str.erase(0, str.find(delim) + 1);
        res.push_back(token);
        i++;
    }
    return res;
}

//String trimming functions
const std::string WHITESPACE = " \n\r\t\f\v";
 
std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}


};
