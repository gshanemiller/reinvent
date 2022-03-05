#!/usr/bin/python

defaultMin=0
defaultMax=65535

providedMin = -100
providedMax = 10000

providedBadMin = 999999
providedBadMax = 1000000

varnum = 0
varpref = "INTLIST_ENV_VAR_"
varname = "undefined"

testNumber = 0

uniqueVal = 113070532

wellFormedIntList = [
    # Single integer
    "9",
    "0",
    # Comma seperated integers
    "1,2",
    "3,4,5",
    "10,11,12,13,100",
    # Comma seperated integers not increasing
    "10,1,3,2,100,43,16",
    # Non-decreasing Ranges
    "[0,5]",
    "[100,100]",
    "[100,102]",
    "[9991,9999]",
    # Combinations of above
    "9,[100,100],3,[100,103],2,0",
    "[100,100],3,[100,103],2,0",
    "[100,100],3,[100,109],2,0,[422,424]",
]

wellFormedIntListOutSideDefaultRange = [
    # Single integer
    "9000000",
    # Comma seperated integers
    "1000000,2",
    "3,4000000,5",
    "10,11,12,13000000,100",
    # Comma seperated integers not increasing
    "10,1,3,2,100,43,16000000",
    # Non-decreasing Ranges
    "[5000000,5000001]",
    # Combinations of above
    "90000000,[100,100],3,[100,103],2,0",
    "[100,100],3,[10000000,10300000],2,0",
    "[100,100],3,[100,109],20000000,0,[422,424]",
]

wellFormedIntListExpectedResult = [
    # Single integer
    [9],
    [0],
    # Comma seperated integers
    [1,2],
    [3,4,5],
    [10,11,12,13,100],
    # Comma seperated integers not increasing
    [10,1,3,2,100,43,16],
    # Non-decreasing Ranges
    [0,1,2,3,4,5],
    [100],
    [100,101,102],
    [9991,9992,9993,9994,9995,9996,9997,9998,9999],
    # Combinations of above
    [9,100,3,100,101,102,103,2,0],
    [100,3,100,101,102,103,2,0],
    [100,3,100,101,102,103,104,105,106,107,108,109,2,0,422,423,424],
]

malFormedIntList = [
    # Bad single non-integers
    "a",
    "0x1213",
    "ABC",
    # Integer with embedded white space
    " 9",
    "9 ",
    "392 33",
    # Lists with whitespaces
    "9, 10, 11",
    " 9,10,11",
    "9,10 , 11",
    "9,10,11 ",
    # Leading/trailing bad commas
    ",9,10",
    " ,9,10",
    "9,10,"
    "9,10, ",
    # Non-decreasing ranges
    "[10,9]",
    # Good range but w/ whitespaces
    " [10,13]",
    "[10,13] ",
    # Not range
    "[10 9]",
    "[10]",
    "[10,]",
    "[10, ]",
    "[10,  ]",
    "[,1]",
    "[, 1]",
    "[,1 ]",
    # Unclosed ranges in bigger lists
    "[1,],10,12",
    "[,1],10,12",
    "10,[1,],12",
    "10,[,1],12",
    "10,12,[1,]",
    "10,12,[,1]",
    "10,12,[1,],5",
    "10,12,[,1],5",
]

cannotSucceedExpectedList=[-65535]

def boolToString(t):
  if (t==True):
    return "true"
  else:
    return "false"

def nextUniqueValue():
    global uniqueVal
    uniqueVal=uniqueVal+1
    return uniqueVal

def nextVariableName():
    global varnum
    global varname
    varname = "{0}{1}".format(varpref, varnum)
    varnum = varnum + 1
    return varname

def nextTestNumber():
    global testNumber
    old = testNumber
    testNumber = testNumber + 1
    return old

class EnvIntList:
    def __init__(self):
        self.dataFid = None
        self.testFid = None

    def openFile(self):
        if self.dataFid is None:
            self.dataFid = open("reinvent_util_environment_unit_test_intlist_data.cpp", "wt")
            self.dataFid.truncate(0)
            self.dataFid.write("#include <util/reinvent_util_errno.h>\n")
            self.dataFid.write("#include <reinvent_util_environment_unit_test_intlist_data.h>\n\n")
            self.dataFid.write("{0}\n".format('const IntListTest REINVENT_UTIL_ENVIRONMENT_INTLIST_DATA[] = {'))
        if self.testFid is None:
            self.testFid = open("reinvent_util_environment_unit_test_intlist.cpp", "wt")
            self.testFid.truncate(0)
            self.testFid.write("#include <gtest/gtest.h>\n")
            self.testFid.write("#include <util/reinvent_util_environment.h>\n")
            self.testFid.write("#include <reinvent_util_environment_unit_test_intlist_data.h>\n\n")
            self.testFid.write("Reinvent::Util::Environment intListTestEnv;\n")
            helperFunc = '''
void compareEqual(const std::vector<int>& lhs, const std::vector<int>& rhs) {
  EXPECT_EQ(lhs.size(), rhs.size());

  auto infimum = lhs.size();
  if (rhs.size()<infimum) {
    infimum = rhs.size();
  }

  for (std::vector<int>::size_type i=0; i<infimum; ++i) {
    if (lhs[i]!=rhs[i]) {
      EXPECT_EQ(lhs[i], rhs[i]);
      std::cout << "mismatch at index " << i << " lhs: " << lhs[i] << " rhs: " << rhs[i] << std::endl;
    }
  } 

  auto c = infimum;
  for (; c<lhs.size(); ++c) {
    EXPECT_TRUE(false);
    std::cout << "unmatched lhs index " << c << " lhs: " << lhs[c] << std::endl;
  }
 
  c = infimum;
  for (; c<rhs.size(); ++c) {
    EXPECT_TRUE(false);
    std::cout << "unmatched rhs index " << c << " rhs: " << rhs[c] << std::endl;
  }
}
'''
            self.testFid.write(helperFunc)
            self.testFid.write('\n')

    def closeFile(self):
        if self.dataFid is not None:
            self.dataFid.write("{0}\n\n".format('};'))
            self.dataFid.write("int numReinventUtilEnvironmentIntListData() {0}\n".format('{'))
            self.dataFid.write("    return sizeof REINVENT_UTIL_ENVIRONMENT_INTLIST_DATA / sizeof *REINVENT_UTIL_ENVIRONMENT_INTLIST_DATA;\n")
            self.dataFid.write("{0}\n\n".format('}'))
            self.dataFid.write("const IntListTest *reinventUtilEnvironmentIntListData(int i) {0}\n".format('{'))
            self.dataFid.write("    return (i>=0&&i<numReinventUtilEnvironmentIntListData()) ? &(REINVENT_UTIL_ENVIRONMENT_INTLIST_DATA[i]) : 0;\n")
            self.dataFid.write("{0}\n\n".format('}'))
            self.dataFid.close()
            self.dataFid = None
        if self.testFid is not None:
            self.testFid.close()
            self.testFid = None

    def emitData(self, desc, var, expectedValue, expectedReturnCode, preInstall, preInstallValue, testRange, useDefaultRange, min, max, postInCache):
        self.dataFid.write("{0}{1}\n".format(' '*4, '{'))
        self.dataFid.write("{0}__LINE__,\n".format(' '*8))
        self.dataFid.write("{0}{1}, // test number\n".format(' '*8, nextTestNumber()))
        self.dataFid.write('{0}"{1}",\n'.format(' '*8, desc))
        self.dataFid.write('{0}"{1}", // variable to be tested\n'.format(' '*8, var))
        self.dataFid.write('{0}{1}, // expected return code\n'.format(' '*8, expectedReturnCode))
        self.dataFid.write('{0}{1}, // install a value into UNIX ENV pre-test?\n'.format(' '*8, boolToString(preInstall)))
        self.dataFid.write('{0}"{1}", // value to install into UNIX ENV or -1 not used. can be malformed\n'.format(' '*8, preInstallValue))
        self.dataFid.write('{0}{1}, // is value subject to range verification?\n'.format(' '*8, boolToString(testRange)))
        self.dataFid.write('{0}{1}, // if range verification, is default range used?\n'.format(' '*8, boolToString(useDefaultRange)))
        if (testRange and useDefaultRange==False):
            self.dataFid.write('{0}{1}, // non-default-range min value\n'.format(' '*8, min))
            self.dataFid.write('{0}{1}, // non-default-range man value\n'.format(' '*8, max))
        else: 
            self.dataFid.write('{0}{1}, // range min unused or useDefaultRange true\n'.format(' '*8, -1))
            self.dataFid.write('{0}{1}, // range max unused or useDefaultRange true\n'.format(' '*8, -1))
        self.dataFid.write('{0}{1}, // after test is done should there be a value in cache?\n'.format(' '*8, boolToString(postInCache)))
        self.dataFid.write("{0}{1},\n".format(' '*4, '}'))

        self.testFid.write("TEST(reinvent_util_environment, intListTest{0}) {1}\n".format(testNumber-1, '{'))
        self.testFid.write("{0}const int testNumber({1});\n".format(' '*4, testNumber-1))
        self.testFid.write("{0}EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());\n".format(' '*4))
        self.testFid.write("{0}const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);\n".format(' '*4))
        self.testFid.write("{0}EXPECT_TRUE(data);\n".format(' '*4))
        self.testFid.write("{0}std::string variable(data->d_variable);\n".format(' '*4))
        self.testFid.write("{0}EXPECT_TRUE(!variable.empty());\n".format(' '*4))
        self.testFid.write("{0}std::vector<int> value;\n".format(' '*4))

        self.testFid.write("{0}if (data->d_preInstall) {1}\n".format(' '*4, '{'))
        self.testFid.write("{0}EXPECT_TRUE(strlen(data->d_preInstallValue)>0);\n".format(' '*8))
        self.testFid.write("{0}EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));\n".format(' '*8))
        self.testFid.write("{0}{1}\n".format(' '*4, '}'))
        self.testFid.write("{0}int rc(-65536);\n".format(' '*4))
        self.testFid.write("{0}if (data->d_testRange && data->d_useDefaultRange) {1}\n".format(' '*4, '{'))
        self.testFid.write("{0}rc = intListTestEnv.valueAsIntList(variable, &value);\n".format(' '*8))
        self.testFid.write("{0}{1} else if (data->d_testRange && !data->d_useDefaultRange) {2}\n".format(' '*4, '}', '{'))
        self.testFid.write("{0}rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);\n".format(' '*8))
        self.testFid.write("{0}{1} else {2}\n".format(' '*4, '}', '{'))
        self.testFid.write("{0}rc = intListTestEnv.valueAsIntList(variable, &value, false);\n".format(' '*8))
        self.testFid.write("{0}{1}\n".format(' '*4, '}'))
        self.testFid.write("{0}EXPECT_EQ(rc, data->d_expectedReturnCode);\n".format(' '*4))
        self.testFid.write("{0}if(rc==0) {1}\n".format(' '*4, '{'))

        self.testFid.write("{0}std::vector<int> expectedValue = {1}{2}{3};\n".format(' '*8, '{', ','.join([str(x) for x in expectedValue]), '}'))
        self.testFid.write("{0}compareEqual(value, expectedValue);\n".format(' '*8))

        self.testFid.write("{0}{1}\n".format(' '*4, '}'))
        self.testFid.write("{0}EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));\n".format(' '*4))
        self.testFid.write("{0}\n\n".format('}'))

    def MalformedIntList_NotInUNIXEnv_NoRangeTest_CustomRange(self):
        # if we're not putting anything into ENV we'll get NOTFOUND
        # doesn't matter malformed or not
        for badInt in malFormedIntList:
            self.emitData(
                desc                        = "Malformed integer-list not in UNIX env",
                var                         = nextVariableName(),
                expectedValue               = cannotSucceedExpectedList,
                expectedReturnCode          = "Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND",
                preInstall                  = False,
                preInstallValue             = "-1",

                testRange                   = False,
                useDefaultRange             = False,
                min                         = -1,
                max                         = -1,
                postInCache                 = False,
            )

    def MalformedIntList_NotInUNIXEnv_NoRangeTest_DefaultRange(self):
        # Covered above
        pass

    def MalformedIntList_NotInUNIXEnv_RangeTest_CustomRange(self):
        # Covered above
        pass

    def MalformedIntList_NotInUNIXEnv_RangeTest_DefaultRange(self):
        # Covered above
        pass

    def MalformedIntList_InUnixEnv_NoRangeTest_CustomRange(self):
        for badInt in malFormedIntList:
            self.emitData(
                desc                        = "Malformed integer-list in UNIX env",
                var                         = nextVariableName(),
                expectedValue               = cannotSucceedExpectedList,
                expectedReturnCode          = "Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT",
                preInstall                  = True,
                preInstallValue             = badInt,

                testRange                   = False,
                useDefaultRange             = False,
                min                         = -1,
                max                         = -1,
                postInCache                 = False,
            )

    def MalformedIntList_InUnixEnv_NoRangeTest_DefaultRange(self):
        # differs from 'MalformedIntList_InUnixEnv_NoRangeTest_CustomRange' such that
        # useDefaultRange=True not false. Since testRange false there's nothing 
        # new to test
        pass

    def MalformedIntList_InUnixEnv_RangeTest_CustomRange(self):
        # malformed data never gets to range testing because already broken
        for badInt in malFormedIntList:
            self.emitData(
                desc                        = "Malformed integer-list in UNIX env",
                var                         = nextVariableName(),
                expectedValue               = cannotSucceedExpectedList,
                expectedReturnCode          = "Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT",
                preInstall                  = True,
                preInstallValue             = badInt,

                testRange                   = True,
                useDefaultRange             = False,
                min                         = -1,
                max                         = -1,
                postInCache                 = False,
            )

    def MalformedIntList_InUnixEnv_RangeTest_DefaultRange(self):
        # malformed data never gets to range testing because already broken
        for badInt in malFormedIntList:
            self.emitData(
                desc                        = "Malformed integer-list in UNIX env",
                var                         = nextVariableName(),
                expectedValue               = cannotSucceedExpectedList,
                expectedReturnCode          = "Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT",
                preInstall                  = True,
                preInstallValue             = badInt,

                testRange                   = True,
                useDefaultRange             = True,
                min                         = -1,
                max                         = -1,
                postInCache                 = False,
            )

    def WellFormedIntList_NotInUNIXEnv_NoRangeTest_CustomRange(self):
        # Covered above
        pass

    def WellFormedIntList_NotInUNIXEnv_NoRangeTest_DefaultRange(self):
        # Covered above
        pass

    def WellFormedIntList_NotInUNIXEnv_RangeTest_CustomRange(self):
        # Covered above
        pass

    def WellFormedIntList_NotInUNIXEnv_RangeTest_DefaultRange(self):
        # Covered above
        pass

    def WellFormedIntList_InUnixEnv_NoRangeTest_CustomRange(self):
        idx=0
        for val in wellFormedIntList:
            self.emitData(
                desc                        = "Valid integer-list in UNIX env not range tested",
                var                         = nextVariableName(),
                expectedValue               = wellFormedIntListExpectedResult[idx],
                expectedReturnCode          = "0",
                preInstall                  = True,
                preInstallValue             = val,
    
                testRange                   = False,
                useDefaultRange             = False,
                min                         = -1,
                max                         = -1,
                postInCache                 = True,
            )
            idx=idx+1

    def WellFormedIntList_InUnixEnv_NoRangeTest_DefaultRange(self):
        # differs from 'WellFormedIntList_InUnixEnv_NoRangeTest_CustomRange' such that
        # useDefaultRange=True not false. Since testRange false there's nothing 
        # new to test
        pass

    def WellFormedIntList_InUnixEnv_RangeTest_CustomRange(self):
        # test well-formed int-list in custom range 
        idx=0
        for val in wellFormedIntList:
            self.emitData(
                desc                        = "Valid integer-list in UNIX env inside custom range",
                var                         = nextVariableName(),
                expectedValue               = wellFormedIntListExpectedResult[idx],
                expectedReturnCode          = "0",
                preInstall                  = True,
                preInstallValue             = val,

                testRange                   = True,
                useDefaultRange             = False,
                min                         = providedMin,
                max                         = providedMax,
                postInCache                 = True,
            )
            idx=idx+1
        # test well-formed int-list outside custom range
        idx=0
        for val in wellFormedIntList:
            self.emitData(
                desc                        = "Valid integer-list in UNIX env outside custom range",
                var                         = nextVariableName(),
                expectedValue               = cannotSucceedExpectedList,
                expectedReturnCode          = "Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE_LIST",
                preInstall                  = True,
                preInstallValue             = val,

                testRange                   = True,
                useDefaultRange             = False,
                min                         = providedBadMin,
                max                         = providedBadMax,
                postInCache                 = False,
            )
            idx=idx+1

    def WellFormedIntList_InUnixEnv_RangeTest_DefaultRange(self):
        # test well-formed int-list in default range 
        idx = 0
        for val in wellFormedIntList:
            self.emitData(
                desc                        = "Valid integer-list in UNIX env inside default range",
                var                         = nextVariableName(),
                expectedValue               = wellFormedIntListExpectedResult[idx],
                expectedReturnCode          = "0",
                preInstall                  = True,
                preInstallValue             = val,

                testRange                   = True,
                useDefaultRange             = True,
                min                         = defaultMin,
                max                         = defaultMax,
                postInCache                 = True,
            )
            idx=idx+1
        # test well-formed int-list outside default range
        idx = 0
        for val in wellFormedIntListOutSideDefaultRange:
            self.emitData(
                desc                        = "Valid integer-list in UNIX env outside default range",
                var                         = nextVariableName(),
                expectedValue               = cannotSucceedExpectedList,
                expectedReturnCode          = "Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE_LIST",
                preInstall                  = True,
                preInstallValue             = val,

                testRange                   = True,
                useDefaultRange             = True,
                min                         = -1,
                max                         = -1,
                postInCache                 = False,
            )

    def makeCases(self):
        for val_malformed in ["MalformedIntList", "WellFormedIntList"]:
            for val_preInstall in ["NotInUNIXEnv", "InUnixEnv"]:
                for val_testRange in ["NoRangeTest", "RangeTest"]:
                    for val_defaultInRange in ["CustomRange", "DefaultRange"]:
                        # function name as string
                        s = "_".join([val_malformed, val_preInstall, val_testRange, val_defaultInRange])
                        # function as object pointer form string
                        f = getattr(self, s, None)
                        # call if (if it exists)
                        if f is not None:
                            f()
                        else:
                            raise(BaseException("no method {0}".format(s)))

envIntList = EnvIntList()
envIntList.openFile()
envIntList.makeCases()
envIntList.closeFile()
