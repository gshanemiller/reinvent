#pragma once

// Purpose: Access values exported in UNIX environment
//
// Classes:
//    Util::Environment: Find and return exported environment variable values in requested type
//
// Thread Safety: MT safe
// Exception Policy: No exceptions
//
// Description: Given a variable name, find its value in the process' UNIX environment converting it to a caller
// determined return type (string, int etc). The value is cached and served out of cache until reset. This class never
// changes the UNIX environment. The cache values can be synched to the UNIX environment after caching with 'reset' or
// its variants. See https://12factor.net for motivation and background. 
//
// It doesn't take much imagination to realize time, date, or IPv4/6 addresses etc types are missing. But including
// those types in 'Util::Environment' would diverge into a glorified string-to-type conversion class. That's not the
// design point here. This API emphasizes minimal not complete. Second, conversion routines exist in DPDK for typical
// cases. Prefer those.
//
// Example usage:
//
// Assume the following variables are exported in UNIX environment prior to running code:
//
//  ENABLED=true
//  PORT_UDP="4000"
//  SERV_IPV4="1.2.3.4"
//
//  {
//    Util::Environment env;
//    int udp_port(-1);
//    std::string ipv4;
//    bool enabled(false);
//    std::string mac("-ERROR-");
//    rc0=env.valueAsInt("PORT_UDP", &udp_port);
//    assert(rc0==0);
//    rc1=env.valueAsString("SERV_IPV4", &ipv4);
//    assert(rc1==0);
//    rc2=env.valueAsBoolean("ENABLED", &enabled);
//    assert(rc2==0);
//    rc3=env.valueAsString("MAC_ADDRESS", &mac);
//    assert(rc3)==Errno::REINVENT_UTIL_ERRNO_NOTFOUND);
//    printf("%s:%d %d %s\n", ipv4.c_str(), udp_port, enabled, mac.c_str());
//  }
// 
//  Will produce the output:
//
//  1.2.3.4:4000 1 -ERROR-
//
//  Non-boolean lookup methods support optional range/length checks. This way a lookup combines existence and simple
//  validity checks in one call.

#include <util/reinvent_util_errno.h>

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <mutex>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

namespace Reinvent {
namespace Util {

class Environment {
  // STATIC DATA
  const static std::string TRUE; 
  const static std::string FALSE; 

  // DATA
  std::mutex d_mux;
  std::map<std::string, std::string> d_values;    // Cached environment variable/value pairs

public:
  // CREATORS
  Environment() = default;
    // Create an empty Environment object.

  ~Environment() = default;
    // Destroy this object

  Environment(const Environment& otherObject) = delete;
    // Copy constructor not provided.

private:
  // PRIVATE ACCESSORS
  int toBoolean(const char *stringValue, bool *value);
    // Return 0 if specified 'stringValue' was converted to a boolean and assigned to specified 'value' and non-zero
    // otherwise.

  int toInt(const char* stringValue, int *value, const char terminal);
    // Return 0 if specified 'stringValue' was converted to an integer and assigned to specified 'value' and non-zero
    // otherwise. Specified 'terminal' is the expected first character not matching an integer.

  void intListToString(const std::vector<int> value, std::string *str);
    // Printf specified 'value' into specified 'str' resulting in a comma separated list of integers.

  int extractIntList(std::string *str, std::vector<int> *value);
    // Return 0 if specified integer list 'str' was converted into an array of integers stored in specified 'value' and
    // non-zero otherwise. Valid int lists are comma separated elements where each element is possibly negative, zero,
    // or positive integer e.g. '7' or an inclusive increasing range e.g. '[11,13]'. The contents of 'value' if any are
    // first discarded. Then integers from 'str' are appended to 'value' left to right sans sorting or de-duplication.
    // Ex. "[3,5],-44,[11,13],3" yields '[3,4,5,-44,11,12,13,3]'. Whitespaces are not ignored. Also note 'str' is 
    // modified to support parsing; pass a copy to avoid side-effects in 'str'.

  int rangeCheckList(const std::string& variable, const std::vector<int>& value, const std::string& strValue, int min, int max);
    // Return 0 if every element in specified 'value' satisfies 'min<=elem<=max' and non-zero otherwise. The specified
    // 'variable, strValue' are used to provided an approrpriate ERRNO log, if that was enabled for the build and
    // otherwise unused. The behvior is defined provided 'max>=min'.

  int extractStringList(std::string *str, std::vector<std::string> *value);
    // Return 0 if specified string list 'str' was converted into an array of strings stored in specified 'value' and
    // non-zero otherwise. A valid format is one or more comma separated strings. Escape processing is not done so the
    // intended string value(s) cannot contain commas. Empty strings are invalid. 

  int rangeCheckList(const std::string& variable, const std::vector<std::string>& value, const std::string& strValue, int min, int max);
    // Return 0 if every element in specified 'value' satisfies 'min<=len(elem)<=max' and non-zero otherwise. The specified
    // 'variable, strValue' are used to provided an approrpriate ERRNO log, if that was enabled for the build and
    // otherwise unused. The behvior is defined provided 'max>=min'.

public:
  // ACCESSORS
  bool exists(const std::string& variable) const;
    // Return true of specified variable exists in cache and false otherwise. The behavior is defined provided
    // '!variable.empty()'. Note that true does not imply its cached value confirms to any type 'T'.

  // MANIPULATORS
  int valueAsInt(const std::string& variable, int *value, bool check=true, int min=0, int max=65535);
    // Return 0 if specified 'variable' was found in cache and its value was converted to integer and assigned to
    // specified 'value' and non-zero otherwise. If not found in cache, inspect the UNIX environment for 'variable'.
    // If its value is found and converted to integer it is assigned to 'value' and 0 is returned. The value is also
    // cached. If the variable is not found in the UNIX enviroment a non-zero value is returned. If 'check' is true 0
    // is returned if, in addition, 'min<=*value<=max'. The behavior is undefined if 'variable.empty()' or
    // 'check && max<min'.

  int valueAsPrefixedInt(const std::string& prefix, const std::string& variable, int *value, bool check=true,
      int min=0, int max=65535);                                                                                  
    // Return 0 if specified 'variable' whose value is of the form '<prefix>_<integer>' was found in cache and its
    // value was converted to integer after removing the '<prefix>_' (e.g. value of 'export ABC=shrimp_53' is 53)
    // and non-zero otherwise. If not found in cache, inspect the UNIX environment for 'variable'. If its value is
    // found and converted to integer sans prefix it is assigned to 'value' and 0 is returned. The value is also
    // cached. If the variable is not found in the UNIX enviroment a non-zero value is returned. If 'check' is true 0
    // is returned if, in addition, 'min<=*value<=max'. The behavior is undefined if 'variable.empty()' or
    // 'check && max<min'.

  int valueAsString(const std::string& variable, std::string *value, bool check=true, unsigned min=1,
    unsigned max=127);
    // Return 0 if specified 'variable' was found in cache and its value was converted to string and assigned to
    // specified 'value' and non-zero otherwise. If not found in cache, inspect the UNIX environment for 'variable'.
    // If its value is found and converted to string it is assigned to 'value' and 0 is returned. The value is also
    // cached. If the variable is not found in the UNIX enviroment a non-zero value is returned. If 'check' true then
    // return 0 if, in addition, 'min<=value->size()<=max'. The behavior is undefined if 'variable.empty()' or
    // 'check && max<min'. Also note algorithm does not recognize escaped commas intended to be part of string value.
  
  int valueAsBool(const std::string& variable, bool *value);
    // Return 0 if specified 'variable' was found in cache and its value was converted to boolean and assigned to
    // specified 'value' and non-zero otherwise. If not found in cache, inspect the UNIX environment for 'variable'.
    // If its value is found and converted to boolean it is assigned to 'value' and 0 is returned. The value is also
    // cached. If the variable is not found in the UNIX enviroment a non-zero value is returned. The behavior is
    // undefined if 'variable.empty()'. Valid boolean values are 'true|false'.

  int valueAsIntList(const std::string& variable, std::vector<int> *value, bool check=true, int min=0, int max=65535);
    // Return 0 if specified 'variable' was found in cache and its value was converted to an integer list and assigned
    // to specified 'value' and non-zero otherwise. If not found in cache, inspect the UNIX environment for 'variable'.
    // If its value is found and converted to an integer list it is assigned to 'value' and 0 is returned. The value is
    // also cached. If the variable is not found in the UNIX enviroment a non-zero value is returned. If 'check' is
    // true 0 is returned if, in addition, 'min<=elem<=max' for each element in 'value'. The behavior is undefined
    // 'variable.empty()' or 'check && max<min'. Refer to 'extractIntList' for int list format.
    //
  int valueAsStringList(const std::string& variable, std::vector<std::string> *value, bool check=true,
    int min=1, int max=127);
    // Return 0 if specified 'variable' was found in cache and its value was converted to an string list and assigned
    // to specified 'value' and non-zero otherwise. If not found in cache, inspect the UNIX environment for 'variable'.
    // If its value is found and converted to an string list it is assigned to 'value' and 0 is returned. The value is
    // also cached. If the variable is not found in the UNIX enviroment a non-zero value is returned. If 'check' is
    // true 0 is returned if, in addition, 'min<=len(elem)<=max' for each element in 'value'. The behavior is undefined
    // 'variable.empty()' or 'check && max<min'. Refer to 'extractStringList' for int list format.

  void reset(const std::string& variable);
    // If the value for specified 'variable' was found in the UNIX environment it's updated in cache to match. If
    // 'variable' is not found in the UNIX envionment, it is removed from cache if it exists. The behavior is undefined
    // if 'variable.empty()'.

  void semiReset(const std::string &variable);
    // If the value for specified 'variable' was found in the UNIX environment it's updated in cache to match. If
    // 'variable' is not found in the UNIX envionment no further action is taken. The behavior is undefined if
    // 'variable.empty()'.

  Environment& operator=(const Environment& rhs) = delete;
    // Assignment operator not provided

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Print into specified 'stream' a human readable dump of this object returning 'stream'.
};

// FREE OPERATORS
std::ostream& operator<<(std::ostream& stream, const Environment& object);
  // Print into specified 'stream' a human readable dump of specified 'object' returning 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// PRIVATE ACCESSORS
inline
int Environment::toBoolean(const char *stringValue, bool *value) {
  assert(stringValue);
  assert(value);

  if (TRUE==stringValue) {
    *value = true;
  } else if (FALSE==stringValue) {
    *value = false;
  } else {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ATOB, (FALSE==stringValue||TRUE==stringValue), stringValue);
  }
  return 0;
}

// ACCESSORS
inline
bool Environment::exists(const std::string& variable) const {
  assert(!variable.empty());

  std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(d_mux));
  std::map<std::string, std::string>::const_iterator iter = d_values.find(variable);
  return (iter!=d_values.end());
}

// MANIPULATORS
inline
int Environment::toInt(const char* stringValue, int *value, const char terminal) {
  assert(stringValue);
  assert(value);

  char *invalid(0);
  if (strlen(stringValue)) {
    *value = strtol(stringValue, &invalid, 10);
    if (invalid && *invalid==terminal) {
      return 0;
    }
  }
  REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ATOI, (strlen(stringValue)&&invalid&&*invalid==terminal), stringValue);
}

inline
int Environment::valueAsInt(const std::string& variable, int *value, bool check, int min, int max) {
  assert(!variable.empty());
  assert(value);
  assert(max>=min);

  std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(d_mux));

  std::map<std::string, std::string>::const_iterator iter = d_values.find(variable);
  if (iter!=d_values.end()) {
    if (0==toInt(iter->second.c_str(), value, 0)) {
      if (check && (*value<min || *value>max)) {
        REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_RANGE, (check && *value>=min && *value<=max), variable.c_str(), "value", *value, min, max);
      } 
      return 0;
    }
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (toInt(iter->second.c_str(), value, 0)==0), variable.c_str(), iter->second.c_str(), "invalid integer");
  }

  const char *envValue = getenv(variable.c_str());
  if (envValue==0) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, (envValue!=0), variable.c_str());     
  }

  if (0==toInt(envValue, value, 0)) {
    if (check && (*value<min || *value>max)) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_RANGE, (check && *value>=min && *value<=max), variable.c_str(), "value", *value, min, max);
    } 
    d_values[variable] = std::string(envValue);
    return 0;
  }
  REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (toInt(envValue, value, 0)==0), variable.c_str(), envValue, "invalid integer");
}

inline                                                                                                                  
int Environment::valueAsPrefixedInt(const std::string& prefix, const std::string& variable, int *value, bool check, int min, int max) {
  assert(!variable.empty());
  assert(value);
  assert(max>=min);

  const char *strValue(0);
  std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(d_mux));

  // Look in cache
  std::map<std::string, std::string>::const_iterator iter = d_values.find(variable);

  if (iter==d_values.end()) {
    // Not in cache; look in UNIX env
    strValue = getenv(variable.c_str());
    if (strValue==0) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, (strValue!=0), variable.c_str());     
    }
  } else {
    // In cache: get value
    strValue = iter->second.c_str();
  }

  // To ignore the prefix in value, the prefix must be shorter than the
  // value. The +1 constant is due to the trailing '_' delimiting prefix
  // and integer in suffix of the value
  if ((prefix.length()+1)>=strlen(strValue)) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, !((prefix.length()+1)>=strlen(strValue)), variable.c_str(), strValue, "invalid prefixed integer for given specified prefix");
  }

  // Try to convert to integer
  if (toInt(strValue+prefix.length()+1, value, 0)) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (toInt(strValue+prefix.length()+1, value, 0)==0), variable.c_str(), strValue, "invalid prefixed integer value");
  }

  // Range check if requestt
  if (check && (*value<min || *value>max)) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_RANGE, (check && *value>=min && *value<=max), variable.c_str(), "value", *value, min, max);
  } 

  // Cache value if needed
  if (iter==d_values.end()) {
    d_values[variable] = std::string(strValue);
  }

  return 0;
}

inline
int Environment::valueAsString(const std::string& variable, std::string *value, bool check, unsigned min, unsigned max) {
  assert(!variable.empty());
  assert(value);
  assert(max>=min);

  std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(d_mux));

  std::map<std::string, std::string>::const_iterator iter = d_values.find(variable);
  if (iter!=d_values.end()) {
    *value = iter->second;
    if (check && (value->size()<min || value->size()>max)) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_STRING_LENGTH, (check && value->size()>=min && value->size()<=max),
        variable.c_str(), value->c_str(), "string length", value->size(), min, max);
    } 
    return 0;
  }

  const char *envValue = getenv(variable.c_str());
  if (envValue==0) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, (envValue!=0), variable.c_str());     
  }

  *value = envValue;
  if (check && (value->size()<min || value->size()>max)) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_STRING_LENGTH, (check && value->size()>=min && value->size()<=max),
      variable.c_str(), value->c_str(), "string length", value->size(), min, max);
  } 
  d_values[variable] = std::string(envValue);

  return 0;
}

inline
int Environment::valueAsBool(const std::string& variable, bool *value) {
  assert(!variable.empty());
  assert(value);

  std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(d_mux));

  std::map<std::string, std::string>::const_iterator iter = d_values.find(variable);
  if (iter!=d_values.end()) {
    if (0==toBoolean(iter->second.c_str(), value)) {
      return 0;
    }
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (toBoolean(iter->second.c_str(), value)==0), variable.c_str(), iter->second.c_str(), "invalid boolean");
  }

  const char *envValue = getenv(variable.c_str());
  if (envValue==0) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, (envValue!=0), variable.c_str());     
  }

  if (0==toBoolean(envValue, value)) {
    d_values[variable] = *value;
    return 0;
  }

  REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (toBoolean(envValue, value)==0), variable.c_str(), envValue, "invalid boolean");
}

inline
void Environment::reset(const std::string& variable) {
  assert(!variable.empty());

  std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(d_mux));

  const char *envValue = getenv(variable.c_str());
  if (envValue==0) {
    std::map<std::string, std::string>::const_iterator iter = d_values.find(variable);
    if (iter!=d_values.end()) {
      d_values.erase(iter);
    }
  }

  d_values[variable] = std::string(envValue);
}

inline
void Environment::semiReset(const std::string &variable) {
  assert(!variable.empty());

  std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(d_mux));

  const char *envValue = getenv(variable.c_str());
  if (envValue!=0) {
    d_values[variable] = std::string(envValue);
  } 
}

// ASPECTS
inline
std::ostream& operator<<(std::ostream& stream, const Environment& object) {
  return object.print(stream);
}

} // namespace util
} // namespace reinvent
