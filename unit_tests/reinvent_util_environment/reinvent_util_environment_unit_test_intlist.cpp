#include <gtest/gtest.h>
#include <util/reinvent_util_environment.h>
#include <reinvent_util_environment_unit_test_intlist_data.h>

Reinvent::Util::Environment intListTestEnv;

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

TEST(reinvent_util_environment, intListTest0) {
    const int testNumber(0);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest1) {
    const int testNumber(1);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest2) {
    const int testNumber(2);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest3) {
    const int testNumber(3);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest4) {
    const int testNumber(4);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest5) {
    const int testNumber(5);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest6) {
    const int testNumber(6);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest7) {
    const int testNumber(7);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest8) {
    const int testNumber(8);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest9) {
    const int testNumber(9);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest10) {
    const int testNumber(10);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest11) {
    const int testNumber(11);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest12) {
    const int testNumber(12);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest13) {
    const int testNumber(13);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest14) {
    const int testNumber(14);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest15) {
    const int testNumber(15);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest16) {
    const int testNumber(16);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest17) {
    const int testNumber(17);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest18) {
    const int testNumber(18);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest19) {
    const int testNumber(19);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest20) {
    const int testNumber(20);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest21) {
    const int testNumber(21);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest22) {
    const int testNumber(22);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest23) {
    const int testNumber(23);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest24) {
    const int testNumber(24);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest25) {
    const int testNumber(25);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest26) {
    const int testNumber(26);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest27) {
    const int testNumber(27);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest28) {
    const int testNumber(28);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest29) {
    const int testNumber(29);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest30) {
    const int testNumber(30);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest31) {
    const int testNumber(31);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest32) {
    const int testNumber(32);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest33) {
    const int testNumber(33);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest34) {
    const int testNumber(34);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest35) {
    const int testNumber(35);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest36) {
    const int testNumber(36);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest37) {
    const int testNumber(37);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest38) {
    const int testNumber(38);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest39) {
    const int testNumber(39);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest40) {
    const int testNumber(40);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest41) {
    const int testNumber(41);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest42) {
    const int testNumber(42);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest43) {
    const int testNumber(43);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest44) {
    const int testNumber(44);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest45) {
    const int testNumber(45);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest46) {
    const int testNumber(46);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest47) {
    const int testNumber(47);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest48) {
    const int testNumber(48);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest49) {
    const int testNumber(49);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest50) {
    const int testNumber(50);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest51) {
    const int testNumber(51);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest52) {
    const int testNumber(52);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest53) {
    const int testNumber(53);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest54) {
    const int testNumber(54);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest55) {
    const int testNumber(55);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest56) {
    const int testNumber(56);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest57) {
    const int testNumber(57);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest58) {
    const int testNumber(58);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest59) {
    const int testNumber(59);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest60) {
    const int testNumber(60);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest61) {
    const int testNumber(61);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest62) {
    const int testNumber(62);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest63) {
    const int testNumber(63);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest64) {
    const int testNumber(64);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest65) {
    const int testNumber(65);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest66) {
    const int testNumber(66);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest67) {
    const int testNumber(67);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest68) {
    const int testNumber(68);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest69) {
    const int testNumber(69);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest70) {
    const int testNumber(70);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest71) {
    const int testNumber(71);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest72) {
    const int testNumber(72);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest73) {
    const int testNumber(73);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest74) {
    const int testNumber(74);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest75) {
    const int testNumber(75);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest76) {
    const int testNumber(76);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest77) {
    const int testNumber(77);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest78) {
    const int testNumber(78);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest79) {
    const int testNumber(79);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest80) {
    const int testNumber(80);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest81) {
    const int testNumber(81);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest82) {
    const int testNumber(82);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest83) {
    const int testNumber(83);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest84) {
    const int testNumber(84);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest85) {
    const int testNumber(85);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest86) {
    const int testNumber(86);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest87) {
    const int testNumber(87);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest88) {
    const int testNumber(88);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest89) {
    const int testNumber(89);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest90) {
    const int testNumber(90);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest91) {
    const int testNumber(91);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest92) {
    const int testNumber(92);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest93) {
    const int testNumber(93);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest94) {
    const int testNumber(94);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest95) {
    const int testNumber(95);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest96) {
    const int testNumber(96);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest97) {
    const int testNumber(97);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest98) {
    const int testNumber(98);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest99) {
    const int testNumber(99);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest100) {
    const int testNumber(100);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest101) {
    const int testNumber(101);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest102) {
    const int testNumber(102);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest103) {
    const int testNumber(103);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest104) {
    const int testNumber(104);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest105) {
    const int testNumber(105);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest106) {
    const int testNumber(106);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest107) {
    const int testNumber(107);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest108) {
    const int testNumber(108);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest109) {
    const int testNumber(109);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest110) {
    const int testNumber(110);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest111) {
    const int testNumber(111);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest112) {
    const int testNumber(112);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest113) {
    const int testNumber(113);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest114) {
    const int testNumber(114);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest115) {
    const int testNumber(115);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest116) {
    const int testNumber(116);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest117) {
    const int testNumber(117);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest118) {
    const int testNumber(118);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest119) {
    const int testNumber(119);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest120) {
    const int testNumber(120);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest121) {
    const int testNumber(121);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest122) {
    const int testNumber(122);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest123) {
    const int testNumber(123);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest124) {
    const int testNumber(124);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest125) {
    const int testNumber(125);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest126) {
    const int testNumber(126);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest127) {
    const int testNumber(127);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest128) {
    const int testNumber(128);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {9};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest129) {
    const int testNumber(129);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {0};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest130) {
    const int testNumber(130);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {1,2};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest131) {
    const int testNumber(131);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {3,4,5};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest132) {
    const int testNumber(132);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {10,11,12,13,100};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest133) {
    const int testNumber(133);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {10,1,3,2,100,43,16};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest134) {
    const int testNumber(134);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {0,1,2,3,4,5};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest135) {
    const int testNumber(135);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {100};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest136) {
    const int testNumber(136);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {100,101,102};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest137) {
    const int testNumber(137);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {9991,9992,9993,9994,9995,9996,9997,9998,9999};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest138) {
    const int testNumber(138);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {9,100,3,100,101,102,103,2,0};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest139) {
    const int testNumber(139);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {100,3,100,101,102,103,2,0};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest140) {
    const int testNumber(140);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {100,3,100,101,102,103,104,105,106,107,108,109,2,0,422,423,424};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest141) {
    const int testNumber(141);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {9};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest142) {
    const int testNumber(142);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {0};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest143) {
    const int testNumber(143);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {1,2};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest144) {
    const int testNumber(144);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {3,4,5};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest145) {
    const int testNumber(145);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {10,11,12,13,100};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest146) {
    const int testNumber(146);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {10,1,3,2,100,43,16};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest147) {
    const int testNumber(147);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {0,1,2,3,4,5};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest148) {
    const int testNumber(148);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {100};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest149) {
    const int testNumber(149);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {100,101,102};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest150) {
    const int testNumber(150);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {9991,9992,9993,9994,9995,9996,9997,9998,9999};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest151) {
    const int testNumber(151);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {9,100,3,100,101,102,103,2,0};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest152) {
    const int testNumber(152);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {100,3,100,101,102,103,2,0};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest153) {
    const int testNumber(153);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {100,3,100,101,102,103,104,105,106,107,108,109,2,0,422,423,424};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest154) {
    const int testNumber(154);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest155) {
    const int testNumber(155);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest156) {
    const int testNumber(156);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest157) {
    const int testNumber(157);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest158) {
    const int testNumber(158);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest159) {
    const int testNumber(159);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest160) {
    const int testNumber(160);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest161) {
    const int testNumber(161);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest162) {
    const int testNumber(162);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest163) {
    const int testNumber(163);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest164) {
    const int testNumber(164);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest165) {
    const int testNumber(165);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest166) {
    const int testNumber(166);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest167) {
    const int testNumber(167);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {9};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest168) {
    const int testNumber(168);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {0};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest169) {
    const int testNumber(169);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {1,2};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest170) {
    const int testNumber(170);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {3,4,5};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest171) {
    const int testNumber(171);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {10,11,12,13,100};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest172) {
    const int testNumber(172);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {10,1,3,2,100,43,16};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest173) {
    const int testNumber(173);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {0,1,2,3,4,5};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest174) {
    const int testNumber(174);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {100};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest175) {
    const int testNumber(175);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {100,101,102};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest176) {
    const int testNumber(176);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {9991,9992,9993,9994,9995,9996,9997,9998,9999};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest177) {
    const int testNumber(177);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {9,100,3,100,101,102,103,2,0};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest178) {
    const int testNumber(178);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {100,3,100,101,102,103,2,0};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest179) {
    const int testNumber(179);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {100,3,100,101,102,103,104,105,106,107,108,109,2,0,422,423,424};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest180) {
    const int testNumber(180);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest181) {
    const int testNumber(181);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest182) {
    const int testNumber(182);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest183) {
    const int testNumber(183);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest184) {
    const int testNumber(184);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest185) {
    const int testNumber(185);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest186) {
    const int testNumber(186);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest187) {
    const int testNumber(187);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

TEST(reinvent_util_environment, intListTest188) {
    const int testNumber(188);
    EXPECT_TRUE(testNumber>=0 && testNumber<numReinventUtilEnvironmentIntListData());
    const IntListTest *data=reinventUtilEnvironmentIntListData(testNumber);
    EXPECT_TRUE(data);
    std::string variable(data->d_variable);
    EXPECT_TRUE(!variable.empty());
    std::vector<int> value;
    if (data->d_preInstall) {
        EXPECT_TRUE(strlen(data->d_preInstallValue)>0);
        EXPECT_TRUE(0==setenv(variable.c_str(), data->d_preInstallValue, 1));
    }
    int rc(-65536);
    if (data->d_testRange && data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value);
    } else if (data->d_testRange && !data->d_useDefaultRange) {
        rc = intListTestEnv.valueAsIntList(variable, &value, true, data->d_min, data->d_max);
    } else {
        rc = intListTestEnv.valueAsIntList(variable, &value, false);
    }
    EXPECT_EQ(rc, data->d_expectedReturnCode);
    if(rc==0) {
        std::vector<int> expectedValue = {-65535};
        compareEqual(value, expectedValue);
    }
    EXPECT_EQ(data->d_postTestInCache, intListTestEnv.exists(variable));
}

