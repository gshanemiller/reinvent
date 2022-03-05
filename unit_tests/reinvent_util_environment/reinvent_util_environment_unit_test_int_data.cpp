#include <util/reinvent_util_errno.h>
#include <reinvent_util_environment_unit_test_int_data.h>

const IntTest REINVENT_UTIL_ENVIRONMENT_INT_DATA[] = {
    {
        __LINE__,
        0, // test number
        "Malformed integer not in UNIX env",
        "INT_ENV_VAR_0", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, // expected return code
        false, // install a value into UNIX ENV pre-test?
        "-1", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        1, // test number
        "Malformed integer not in UNIX env",
        "INT_ENV_VAR_1", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, // expected return code
        false, // install a value into UNIX ENV pre-test?
        "-1", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        2, // test number
        "Malformed integer not in UNIX env",
        "INT_ENV_VAR_2", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, // expected return code
        false, // install a value into UNIX ENV pre-test?
        "-1", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        3, // test number
        "Malformed integer not in UNIX env",
        "INT_ENV_VAR_3", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, // expected return code
        false, // install a value into UNIX ENV pre-test?
        "-1", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        4, // test number
        "Malformed integer not in UNIX env",
        "INT_ENV_VAR_4", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, // expected return code
        false, // install a value into UNIX ENV pre-test?
        "-1", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        5, // test number
        "Malformed integer not in UNIX env",
        "INT_ENV_VAR_5", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, // expected return code
        false, // install a value into UNIX ENV pre-test?
        "-1", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        6, // test number
        "Malformed integer not in UNIX env",
        "INT_ENV_VAR_6", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_NOTFOUND, // expected return code
        false, // install a value into UNIX ENV pre-test?
        "-1", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        7, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_7", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "abc", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        8, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_8", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "--9", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        9, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_9", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "+-1000", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        10, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_10", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "12.0", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        11, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_11", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "12,3", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        12, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_12", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "0x123", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        13, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_13", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "9a-+123", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        14, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_14", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "abc", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // non-default-range min value
        -1, // non-default-range man value
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        15, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_15", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "--9", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // non-default-range min value
        -1, // non-default-range man value
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        16, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_16", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "+-1000", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // non-default-range min value
        -1, // non-default-range man value
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        17, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_17", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "12.0", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // non-default-range min value
        -1, // non-default-range man value
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        18, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_18", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "12,3", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // non-default-range min value
        -1, // non-default-range man value
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        19, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_19", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "0x123", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // non-default-range min value
        -1, // non-default-range man value
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        20, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_20", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "9a-+123", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // non-default-range min value
        -1, // non-default-range man value
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        21, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_21", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "abc", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        22, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_22", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "--9", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        23, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_23", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "+-1000", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        24, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_24", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "12.0", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        25, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_25", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "12,3", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        26, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_26", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "0x123", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        27, // test number
        "Malformed integer in UNIX env",
        "INT_ENV_VAR_27", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "9a-+123", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        28, // test number
        "Valid integer in UNIX env",
        "INT_ENV_VAR_28", // variable to be tested
        113070533, // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "113070533", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        29, // test number
        "Valid integer in UNIX env inside custom range",
        "INT_ENV_VAR_29", // variable to be tested
        10, // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "10", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        10, // non-default-range min value
        20, // non-default-range man value
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        30, // test number
        "Valid integer in UNIX env inside custom range",
        "INT_ENV_VAR_30", // variable to be tested
        20, // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "20", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        10, // non-default-range min value
        20, // non-default-range man value
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        31, // test number
        "Valid integer in UNIX env inside custom range",
        "INT_ENV_VAR_31", // variable to be tested
        11, // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "11", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        10, // non-default-range min value
        20, // non-default-range man value
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        32, // test number
        "Valid integer in UNIX env inside custom range",
        "INT_ENV_VAR_32", // variable to be tested
        19, // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "19", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        10, // non-default-range min value
        20, // non-default-range man value
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        33, // test number
        "Valid integer in UNIX env outside custom range",
        "INT_ENV_VAR_33", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "9", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        10, // non-default-range min value
        20, // non-default-range man value
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        34, // test number
        "Valid integer in UNIX env outside custom range",
        "INT_ENV_VAR_34", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "21", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        10, // non-default-range min value
        20, // non-default-range man value
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        35, // test number
        "Valid integer in UNIX env inside default range",
        "INT_ENV_VAR_35", // variable to be tested
        0, // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "0", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        36, // test number
        "Valid integer in UNIX env inside default range",
        "INT_ENV_VAR_36", // variable to be tested
        65535, // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "65535", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        37, // test number
        "Valid integer in UNIX env inside default range",
        "INT_ENV_VAR_37", // variable to be tested
        1, // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "1", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        38, // test number
        "Valid integer in UNIX env inside default range",
        "INT_ENV_VAR_38", // variable to be tested
        65534, // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "65534", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        39, // test number
        "Valid integer in UNIX env outside default range",
        "INT_ENV_VAR_39", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "-1", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        40, // test number
        "Valid integer in UNIX env outside default range",
        "INT_ENV_VAR_40", // variable to be tested
        -1, // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_RANGE, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "65536", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
};

int numReinventUtilEnvironmentIntData() {
    return sizeof REINVENT_UTIL_ENVIRONMENT_INT_DATA / sizeof *REINVENT_UTIL_ENVIRONMENT_INT_DATA;
}

const IntTest *reinventUtilEnvironmentIntData(int i) {
    return (i>=0&&i<numReinventUtilEnvironmentIntData()) ? &(REINVENT_UTIL_ENVIRONMENT_INT_DATA[i]) : 0;
}

