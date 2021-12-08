
#include <algorithm>

#include "EGWDefine.hpp"

std::string EGWCommon::trim(std::string & _out) {
    const std::string & chars = "\t\n\v\f\r *";
    _out.erase(_out.find_last_not_of(chars)+1);
    _out.erase(0, _out.find_first_not_of(chars));
    return _out;
}

std::tuple<std::string, std::string> EGWCommon::split(std::string & _line, const std::string _sep) {

  std::size_t found = _line.find_first_of(_sep);

  if(found == std::string::npos) {
    std::string out = EGWCommon::trim(_line);
    return std::tuple<std::string, std::string>(out, "");
  }

  std::string first = _line.substr(0, found);
  std::string second = _line.substr(found+1);

  first = EGWCommon::trim(first);
  second = EGWCommon::trim(second);

  return std::tuple<std::string, std::string>(first, second);

}

size_t EGWCommon::split(std::vector<std::string> & _vec, const std::string & _line, const std::string _sep) {

  if(_line.empty())
    return _vec.size();

  std::size_t found = _line.find_first_of(_sep);

  if(found == std::string::npos) {
    _vec.emplace_back(_line);
    return _vec.size();
  }

  std::string first = _line.substr(0, found);
  std::string second = _line.substr(found+1);

  first = EGWCommon::trim(first);
  second = EGWCommon::trim(second);

  _vec.emplace_back(first);

  return EGWCommon::split(_vec, second, _sep);
}
