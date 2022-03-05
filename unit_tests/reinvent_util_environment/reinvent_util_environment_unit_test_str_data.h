#pragma once

struct StringTest {                                                                                                        
  int d_line;                     // line number of test; see below for test case decls
  int d_testNumber;               // test number
  const char *d_description;      // summary of test
  const char *d_variable;         // variable name to check in environment
  const char *d_expectedValue;    // expected value of variable
  int d_expectedReturnCode;       // expected return code
  bool d_preInstall;              // are we putting something into ENV before test?
  const char *d_preInstallValue;  // value to put into ENV if d_preInstallValue true
  bool d_testRange;               // True if range enforcement on. False disables all range checking
  bool d_useDefaultRange;         // If 'testRange' true use default range enforcement as per API?
  int  d_min;                     // Min allowable value if testRange=true, useDefaultRange=false
  int  d_max;                     // Max allowable value if testRange=true, useDefaultRange=false
  bool d_postTestInCache;         // After the test is done should there be a value in cache?
};

int numReinventUtilEnvironmentStringData();
const StringTest *reinventUtilEnvironmentStringData(int);
