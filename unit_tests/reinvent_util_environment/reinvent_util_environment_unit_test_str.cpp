#include <gtest/gtest.h>
#include <util/reinvent_util_environment.h>
#include <reinvent_util_environment_unit_test_str_data.h>

Reinvent::Util::Environment testStringEnv;

TEST(reinvent_util_environment, strTest0) {
    const int testNumber(0);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

TEST(reinvent_util_environment, strTest1) {
    const int testNumber(1);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

TEST(reinvent_util_environment, strTest2) {
    const int testNumber(2);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

TEST(reinvent_util_environment, strTest3) {
    const int testNumber(3);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

TEST(reinvent_util_environment, strTest4) {
    const int testNumber(4);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

TEST(reinvent_util_environment, strTest5) {
    const int testNumber(5);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

TEST(reinvent_util_environment, strTest6) {
    const int testNumber(6);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

TEST(reinvent_util_environment, strTest7) {
    const int testNumber(7);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

TEST(reinvent_util_environment, strTest8) {
    const int testNumber(8);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

TEST(reinvent_util_environment, strTest9) {
    const int testNumber(9);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

TEST(reinvent_util_environment, strTest10) {
    const int testNumber(10);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

TEST(reinvent_util_environment, strTest11) {
    const int testNumber(11);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

TEST(reinvent_util_environment, strTest12) {
    const int testNumber(12);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentStringData());
    const StringTest *data=reinventUtilEnvironmentStringData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::string value("ERROR");
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = testStringEnv.valueAsString(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = testStringEnv.valueAsString(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        EXPECT_EQ(value, data->d_expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, testStringEnv.exists(variable));
}

