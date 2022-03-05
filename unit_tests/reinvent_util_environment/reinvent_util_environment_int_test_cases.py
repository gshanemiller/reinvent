#!/usr/bin/python

defaultMin=0
defaultMax=65535

providedMin = 10
providedMax = 20

varnum = 0
varpref = "INT_ENV_VAR_"
varname = "undefined"

testNumber = 0

uniqueVal = 113070532

malFormedInt = ["abc", "--9", "+-1000", "12.0", "12,3", "0x123", "9a" "-+123" ]

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

def rangeBadValues(min, max):
    yield min-1
    yield max+1

def rangeGoodValues(min, max):
    yield min
    yield max
    if min+1<max:
        yield min+1
    if max-1>min:
        yield max-1

class EnvInt:
    def __init__(self):
        self.dataFid = None
        self.testFid = None

    def openFile(self):
        if self.dataFid is None:
            self.dataFid = open("reinvent_util_environment_unit_test_int_data.cpp", "wt")
            self.dataFid.truncate(0)
            self.dataFid.write("#include <util/reinvent_util_errno.h>\n")
            self.dataFid.write("#include <reinvent_util_environment_unit_test_int_data.h>\n\n")
            self.dataFid.write("{0}\n".format('const IntTest REINVENT_UTIL_ENVIRONMENT_INT_DATA[] = {'))
        if self.testFid is None:
            self.testFid = open("reinvent_util_environment_unit_test_int.cpp", "wt")
            self.testFid.truncate(0)
            self.testFid.write("#include <gtest/gtest.h>\n")
            self.testFid.write("#include <util/reinvent_util_environment.h>\n")
            self.testFid.write("#include <reinvent_util_environment_unit_test_int_data.h>\n\n")
            self.testFid.write("Reinvent::Util::Environment intTestEnv;\n\n")

    def closeFile(self):
        if self.dataFid is not None:
            self.dataFid.write("{0}\n\n".format('};'))
            self.dataFid.write("int numReinventUtilEnvironmentIntData() {0}\n".format('{'))
            self.dataFid.write("    return sizeof REINVENT_UTIL_ENVIRONMENT_INT_DATA / sizeof *REINVENT_UTIL_ENVIRONMENT_INT_DATA;\n")
            self.dataFid.write("{0}\n\n".format('}'))
            self.dataFid.write("const IntTest *reinventUtilEnvironmentIntData(int i) {0}\n".format('{'))
            self.dataFid.write("    return (i>=0&&i<numReinventUtilEnvironmentIntData()) ? &(REINVENT_UTIL_ENVIRONMENT_INT_DATA[i]) : 0;\n")
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
        self.dataFid.write('{0}{1}, // expected value on success or -1 if cannot succeed\n'.format(' '*8, expectedValue))
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

        self.testFid.write("TEST(reinvent_util_environment, intTest{0}) {1}\n".format(testNumber-1, '{'))
        self.testFid.write("{0}const int testNumber({1});\n".format(' '*4, testNumber-1))
        self.testFid.write("{0}EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());\n".format(' '*4))
        self.testFid.write("{0}const IntTest *data=reinventUtilEnvironmentIntData(testNumber);\n".format(' '*4))
        self.testFid.write("{0}EXPECT_TRUE(data);\n".format(' '*4))
        self.testFid.write("{0}std::string variable(data->d_variable);\n".format(' '*4))
        self.testFid.write("{0}EXPECT_TRUE(!variable.empty());\n".format(' '*4))
        self.testFid.write("{0}int value(-65536);\n".format(' '*4))
        self.testFid.write("{0}if (data->d_preInstall) {1}\n".format(' '*4, '{'))
        self.testFid.write("{0}EXPECT_TRUE(strlen(data->d_preInstallValue)>0);\n".format(' '*8))
        self.testFid.write("{0}EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));\n".format(' '*8))
        self.testFid.write("{0}{1}\n".format(' '*4, '}'))
        self.testFid.write("{0}int rc(-65536);\n".format(' '*4))
        self.testFid.write("{0}if (data->d_testRange && data->d_useDefaultRange) {1}\n".format(' '*4, '{'))
        self.testFid.write("{0}rc = intTestEnv.valueAsInt(variable, &value);\n".format(' '*8))
        self.testFid.write("{0}{1} else if (data->d_testRange && !data->d_useDefaultRange) {2}\n".format(' '*4, '}', '{'))
        self.testFid.write("{0}rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);\n".format(' '*8))
        self.testFid.write("{0}{1} else {2}\n".format(' '*4, '}', '{'))
        self.testFid.write("{0}rc = intTestEnv.valueAsInt(variable, &value, false);\n".format(' '*8))
        self.testFid.write("{0}{1}\n".format(' '*4, '}'))
        self.testFid.write("{0}EXPECT_EQ(rc, data->d_expectedReturnCode);\n".format(' '*4))
        self.testFid.write("{0}if(rc==0) {1}\n".format(' '*4, '{'))
        self.testFid.write("{0}EXPECT_EQ(value, data->d_expectedValue);\n".format(' '*8))
        self.testFid.write("{0}{1}\n".format(' '*4, '}'))
        self.testFid.write("{0}EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));\n".format(' '*4))
        self.testFid.write("{0}\n\n".format('}'))

    def MalformedInt_NotInUNIXEnv_NoRangeTest_CustomRange(self):
        # if we're not putting anything into ENV we'll get NOTFOUND
        # doesn't matter malformed or not
        for badInt in malFormedInt:
            self.emitData(
                desc                        = "Malformed integer not in UNIX env",
                var                         = nextVariableName(),
                expectedValue               = -1,
                expectedReturnCode          = "Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND",
                preInstall                  = False,
                preInstallValue             = -1,

                testRange                   = False,
                useDefaultRange             = False,
                min                         = -1,
                max                         = -1,
                postInCache                 = False,
            )

    def MalformedInt_NotInUNIXEnv_NoRangeTest_DefaultRange(self):
        # Covered above
        pass

    def MalformedInt_NotInUNIXEnv_RangeTest_CustomRange(self):
        # Covered above
        pass

    def MalformedInt_NotInUNIXEnv_RangeTest_DefaultRange(self):
        # Covered above
        pass

    def MalformedInt_InUnixEnv_NoRangeTest_CustomRange(self):
        for badInt in malFormedInt:
            self.emitData(
                desc                        = "Malformed integer in UNIX env",
                var                         = nextVariableName(),
                expectedValue               = -1,
                expectedReturnCode          = "Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT",
                preInstall                  = True,
                preInstallValue             = badInt,

                testRange                   = False,
                useDefaultRange             = False,
                min                         = -1,
                max                         = -1,
                postInCache                 = False,
            )

    def MalformedInt_InUnixEnv_NoRangeTest_DefaultRange(self):
        # differs from 'MalformedInt_InUnixEnv_NoRangeTest_CustomRange' such that
        # useDefaultRange=True not false. Since testRange false there's nothing 
        # new to test
        pass

    def MalformedInt_InUnixEnv_RangeTest_CustomRange(self):
        for badInt in malFormedInt:
            self.emitData(
                desc                        = "Malformed integer in UNIX env",
                var                         = nextVariableName(),
                expectedValue               = -1,
                expectedReturnCode          = "Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT",
                preInstall                  = True,
                preInstallValue             = badInt,

                testRange                   = True,
                useDefaultRange             = False,
                min                         = -1,
                max                         = -1,
                postInCache                 = False,
            )

    def MalformedInt_InUnixEnv_RangeTest_DefaultRange(self):
        for badInt in malFormedInt:
            self.emitData(
                desc                        = "Malformed integer in UNIX env",
                var                         = nextVariableName(),
                expectedValue               = -1,
                expectedReturnCode          = "Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT",
                preInstall                  = True,
                preInstallValue             = badInt,

                testRange                   = True,
                useDefaultRange             = True,
                min                         = -1,
                max                         = -1,
                postInCache                 = False,
            )

    def WellFormedInt_NotInUNIXEnv_NoRangeTest_CustomRange(self):
        # Covered above
        pass

    def WellFormedInt_NotInUNIXEnv_NoRangeTest_DefaultRange(self):
        # Covered above
        pass

    def WellFormedInt_NotInUNIXEnv_RangeTest_CustomRange(self):
        # Covered above
        pass

    def WellFormedInt_NotInUNIXEnv_RangeTest_DefaultRange(self):
        # Covered above
        pass

    def WellFormedInt_InUnixEnv_NoRangeTest_CustomRange(self):
        val = nextUniqueValue()
        self.emitData(
            desc                        = "Valid integer in UNIX env",
            var                         = nextVariableName(),
            expectedValue               = val,
            expectedReturnCode          = "0",
            preInstall                  = True,
            preInstallValue             = str(val),

            testRange                   = False,
            useDefaultRange             = False,
            min                         = -1,
            max                         = -1,
            postInCache                 = True,
        )

    def WellFormedInt_InUnixEnv_NoRangeTest_DefaultRange(self):
        # differs from 'WellFormedInt_InUnixEnv_NoRangeTest_CustomRange' such that
        # useDefaultRange=True not false. Since testRange false there's nothing 
        # new to test
        pass

    def WellFormedInt_InUnixEnv_RangeTest_CustomRange(self):
        # test well-formed integers in custom range 
        for val in rangeGoodValues(providedMin, providedMax):
            self.emitData(
                desc                        = "Valid integer in UNIX env inside custom range",
                var                         = nextVariableName(),
                expectedValue               = val,
                expectedReturnCode          = "0",
                preInstall                  = True,
                preInstallValue             = str(val),

                testRange                   = True,
                useDefaultRange             = False,
                min                         = providedMin,
                max                         = providedMax,
                postInCache                 = True,
            )
        # test well-formed integers outside custome range
        for val in rangeBadValues(providedMin, providedMax):
            self.emitData(
                desc                        = "Valid integer in UNIX env outside custom range",
                var                         = nextVariableName(),
                expectedValue               = -1,
                expectedReturnCode          = "Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE",
                preInstall                  = True,
                preInstallValue             = str(val),

                testRange                   = True,
                useDefaultRange             = False,
                min                         = providedMin,
                max                         = providedMax,
                postInCache                 = False,
            )

    def WellFormedInt_InUnixEnv_RangeTest_DefaultRange(self):
        # test well-formed integers in default range 
        for val in rangeGoodValues(defaultMin, defaultMax):
            self.emitData(
                desc                        = "Valid integer in UNIX env inside default range",
                var                         = nextVariableName(),
                expectedValue               = val,
                expectedReturnCode          = "0",
                preInstall                  = True,
                preInstallValue             = str(val),

                testRange                   = True,
                useDefaultRange             = True,
                min                         = defaultMin,
                max                         = defaultMax,
                postInCache                 = True,
            )
        # test well-formed integers outside default range
        for val in rangeBadValues(defaultMin, defaultMax):
            self.emitData(
                desc                        = "Valid integer in UNIX env outside default range",
                var                         = nextVariableName(),
                expectedValue               = -1,
                expectedReturnCode          = "Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE",
                preInstall                  = True,
                preInstallValue             = str(val),

                testRange                   = True,
                useDefaultRange             = True,
                min                         = -1,
                max                         = -1,
                postInCache                 = False,
            )

    def makeCases(self):
        for val_malformed in ["MalformedInt", "WellFormedInt"]:
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

envInt = EnvInt()
envInt.openFile()
envInt.makeCases()
envInt.closeFile()
