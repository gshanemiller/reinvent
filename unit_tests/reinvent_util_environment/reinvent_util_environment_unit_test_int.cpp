#include <gtest/gtest.h>
#include <util/reinvent_util_environment.h>
#include <reinvent_util_environment_unit_test_int_data.h>

Reinvent::Util::Environment intTestEnv;

TEST(reinvent_util_environment, intTest0) {
    const int testNumber(0);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest1) {
    const int testNumber(1);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest2) {
    const int testNumber(2);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest3) {
    const int testNumber(3);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest4) {
    const int testNumber(4);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest5) {
    const int testNumber(5);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest6) {
    const int testNumber(6);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest7) {
    const int testNumber(7);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest8) {
    const int testNumber(8);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest9) {
    const int testNumber(9);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest10) {
    const int testNumber(10);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest11) {
    const int testNumber(11);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest12) {
    const int testNumber(12);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest13) {
    const int testNumber(13);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest14) {
    const int testNumber(14);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest15) {
    const int testNumber(15);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest16) {
    const int testNumber(16);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest17) {
    const int testNumber(17);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest18) {
    const int testNumber(18);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest19) {
    const int testNumber(19);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest20) {
    const int testNumber(20);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest21) {
    const int testNumber(21);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest22) {
    const int testNumber(22);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest23) {
    const int testNumber(23);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest24) {
    const int testNumber(24);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest25) {
    const int testNumber(25);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest26) {
    const int testNumber(26);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest27) {
    const int testNumber(27);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest28) {
    const int testNumber(28);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest29) {
    const int testNumber(29);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest30) {
    const int testNumber(30);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest31) {
    const int testNumber(31);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest32) {
    const int testNumber(32);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest33) {
    const int testNumber(33);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest34) {
    const int testNumber(34);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest35) {
    const int testNumber(35);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest36) {
    const int testNumber(36);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest37) {
    const int testNumber(37);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest38) {
    const int testNumber(38);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest39) {
    const int testNumber(39);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intTest40) {
    const int testNumber(40);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntData());
    const IntTest *data=reinventUtilEnvironmentIntData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    int value(-65536);
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intTestEnv.valueAsInt(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intTestEnv.valueAsInt(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intTestEnv.exists(variable));
}

