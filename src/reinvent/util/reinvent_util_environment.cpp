#include <util/reinvent_util_environment.h>

#include <string.h>
#include <stdarg.h>

namespace Reinvent {

const std::string Util::Environment::TRUE("TRUE");
const std::string Util::Environment::FALSE("FALSE");

void Util::Environment::intListToString(const std::vector<int> value, std::string *str) {
  assert(str);

  str->clear();

  std::vector<int>::const_iterator iter = value.begin();
  std::vector<int>::const_iterator eter = value.end();
  while (iter!=eter) {
    str->append(std::to_string(*iter));
    if (++iter!=eter) {
      str->append(1, ',');
    }
  }
}

int Util::Environment::extractIntList(std::string *str, std::vector<int> *value) {
  assert(str);
  assert(str->length());
  assert(value);                                                                                                        

  // Start empty
  value->clear();

  // Error out if whitespaces: not playing pointer arithmetic for noise
  for (std::string::size_type i=0; i<str->length(); ++i) {
    if (isspace(str->operator[](i))) {
      return 1;
    }
  }

  // Leading with a comma delimiter is ambiguous
  if (str->operator[](0)==',') {
    return 1;
  }

  // First tokenize by commas
  char *context, *token;
  const char commaDelimiter[]=",";
  int aInt(0), startRange(0), endRange(0), inRange(0);

  for (token = strtok_r(const_cast<char*>(str->c_str()), commaDelimiter, &context); token;
    token = strtok_r(0, commaDelimiter, &context)) {
    // Empty tokens are ambiguous
    if (strlen(token)==0) {
      return 1;
    }
    // See start of range?
    if (token[0]=='[') {
      if (++inRange!=1) {
        // Seen ...[10,[12,....
        return 1;
      } else if (toInt(token+1, &startRange, 0)) {
        // bad int for start of range
        return 1;
      } else {
        // ok, got good start of range
        continue;
      }
    }
    // If we've seen start of range we're still in range until upper limit
    if (inRange) {
      ++inRange;
    }
    if (inRange==2) {
      if (token[strlen(token)-1]!=']') {
        // Saw lower limit not immediately followed by upper limit
        return 1;
      } else if (toInt(token, &endRange, ']')) {                                                                      
        // bad int for start of range                                                                                   
        return 1;
      } else if (startRange>endRange) {
        // saw [10,9] ...
        return 1;
      } else {
        // ok, got end of range
        inRange = 0;
        for(; startRange<=endRange; ++startRange) {
          value->push_back(startRange);
        }
        continue;
      }
    }

    // OK, otherwise we have a vanilla single int
    if (toInt(token, &aInt, 0)) {
      // And it was bad
      return 1;
    } else if (inRange==0) {
      value->push_back(aInt);
    } else {
      return 1;
    }
  }

  // Final result cannot be be empty
  return (value->empty()) ? 1 : 0;
}

int Util::Environment::extractStringList(std::string *str, std::vector<std::string> *value) {
  assert(str);
  assert(str->length());
  assert(value);                                                                                                        

  // Start empty
  value->clear();

  // Leading with a comma delimiter is ambiguous
  if (str->operator[](0)==',') {
    return 1;
  }

  // First tokenize by commas
  char *context, *token;
  const char commaDelimiter[]=",";

  for (token = strtok_r(const_cast<char*>(str->c_str()), commaDelimiter, &context); token;
    token = strtok_r(0, commaDelimiter, &context)) {
    // Empty tokens are ambiguous
    if (strlen(token)==0) {
      return 1;
    }

    value->push_back(token);
  }

  // Final result cannot be be empty
  return (value->empty()) ? 1 : 0;
}

//                                                                                                                      
// Rarely needed workaround
// Tell GCC to not enforce '-Wunused-parameter' for 'variable, strValue'
// depending on build mode this will otherwise break -Werror -Wpendantic
//
#pragma GCC diagnostic push                                                                                             
#pragma GCC diagnostic ignored "-Wunused-parameter"
int Util::Environment::rangeCheckList(const std::string& variable, const std::vector<int>& value, const std::string& strValue, int min, int max) {
  assert(max>=min);

  for (unsigned i=0; i<value.size(); ++i) {
    if (value[i]<min || value[i]>max) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_RANGE_LIST, (value[i]<min || value[i]>max),
        variable.c_str(), strValue.c_str(), "contains an element", value[i], min, max);
    }
  }

  return 0;
}

int Util::Environment::rangeCheckList(const std::string& variable, const std::vector<std::string>& value, const std::string& strValue, int min, int max) {
  assert(max>=min);

  for (unsigned i=0; i<value.size(); ++i) {
    int len = static_cast<int>(value[i].length());
    if (len<min || len>max) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_RANGE_LIST, (len<min || len>max),
        variable.c_str(), strValue.c_str(), "contains an element", len, min, max);
    }
  }

  return 0;
}
#pragma GCC diagnostic pop 

int Util::Environment::valueAsIntList(const std::string& variable, std::vector<int> *value, bool check, int min, int max) {
  assert(!variable.empty());                                                                                            
  assert(value);                                                                                                        

  std::lock_guard<std::mutex> lock(d_mux);

  std::map<std::string, std::string>::const_iterator iter = d_values.find(variable);                                    
  if (iter!=d_values.end()) {                                                                                           
    std::string tmp(iter->second);
    if (0==extractIntList(&tmp, value)) {
      return check ? rangeCheckList(variable, *value, iter->second, min, max) : 0;
    }
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (extractIntList(&tmp, value)==0), variable.c_str(), iter->second.c_str(), "invalid integer list");
  }                                                                                                                     

  const char *envValue = getenv(variable.c_str());                                                                      
  if (envValue==0) {                                                                                                    
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, (envValue!=0), variable.c_str());
  }                                                                                                                     

  std::string tmp(envValue);
  if (0==extractIntList(&tmp, value)) {
    int rc = check ? rangeCheckList(variable, *value, envValue, min, max) : 0;
    if (rc==0) {
      d_values[variable] = std::string(envValue);
    }
    return rc;
  }
  REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (extractIntList(&tmp, value)==0), variable.c_str(), envValue, "invalid integer list");
}

int Util::Environment::valueAsStringList(const std::string& variable, std::vector<std::string> *value, bool check, int min, int max) {
  assert(!variable.empty());                                                                                            
  assert(value);                                                                                                        

  std::lock_guard<std::mutex> lock(d_mux);

  std::map<std::string, std::string>::const_iterator iter = d_values.find(variable);                                    
  if (iter!=d_values.end()) {                                                                                           
    std::string tmp(iter->second);
    if (0==extractStringList(&tmp, value)) {
      return check ? rangeCheckList(variable, *value, iter->second, min, max) : 0;
    }
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (extractStringList(&tmp, value)==0), variable.c_str(), iter->second.c_str(), "invalid string list");
  }                                                                                                                     

  const char *envValue = getenv(variable.c_str());                                                                      
  if (envValue==0) {                                                                                                    
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, (envValue!=0), variable.c_str());
  }                                                                                                                     

  std::string tmp(envValue);
  if (0==extractStringList(&tmp, value)) {
    int rc = check ? rangeCheckList(variable, *value, envValue, min, max) : 0;
    if (rc==0) {
      d_values[variable] = std::string(envValue);
    }
    return rc;
  }
  REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (extractStringList(&tmp, value)==0), variable.c_str(), envValue, "invalid string list");
}

std::ostream& Util::Environment::print(std::ostream& stream) const {
  stream << "Environment: { ";

  std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(d_mux));                                                     

  std::map<std::string, std::string>::const_iterator iter = d_values.begin();
  std::map<std::string, std::string>::const_iterator eter = d_values.end();

  while(iter!=eter) {
    stream << "'"
           << iter->first
           << "': '"
           << iter->second
           << "'";
    if (++iter!=eter) {
      stream << ", ";
    }
  }

  stream << " }";

  return stream;
}

} // namespace Reinvent
