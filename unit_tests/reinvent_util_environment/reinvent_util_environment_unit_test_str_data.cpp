#include <util/reinvent_util_errno.h>
#include <reinvent_util_environment_unit_test_str_data.h>

const StringTest REINVENT_UTIL_ENVIRONMENT_STRING_DATA[] = {
    {
        __LINE__,
        0, // test number
        "String not in UNIX env",
        "STR_ENV_VAR_0", // variable to be tested
        "-1", // expected value on success or -1 if cannot succeed
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
        "Valid string in UNIX env",
        "STR_ENV_VAR_1", // variable to be tested
        "YARMKUKMLVMUFFSWSRSXUQCOSSWNQMFF", // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "YARMKUKMLVMUFFSWSRSXUQCOSSWNQMFF", // value to install into UNIX ENV or -1 not used. can be malformed
        false, // is value subject to range verification?
        false, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        2, // test number
        "String in UNIX env inside custom length range",
        "STR_ENV_VAR_2", // variable to be tested
        "KMPXEBZIPT", // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "KMPXEBZIPT", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        10, // non-default-range min value
        20, // non-default-range man value
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        3, // test number
        "String in UNIX env inside custom length range",
        "STR_ENV_VAR_3", // variable to be tested
        "DAIUHIBATORLUUNNFFNV", // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "DAIUHIBATORLUUNNFFNV", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        10, // non-default-range min value
        20, // non-default-range man value
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        4, // test number
        "String in UNIX env inside custom length range",
        "STR_ENV_VAR_4", // variable to be tested
        "FHKCTHEEHSM", // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "FHKCTHEEHSM", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        10, // non-default-range min value
        20, // non-default-range man value
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        5, // test number
        "String in UNIX env inside custom length range",
        "STR_ENV_VAR_5", // variable to be tested
        "VAPPLOWXQJEWYXFECFL", // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "VAPPLOWXQJEWYXFECFL", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        10, // non-default-range min value
        20, // non-default-range man value
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        6, // test number
        "String in UNIX env outside custom length range",
        "STR_ENV_VAR_6", // variable to be tested
        "-1", // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_STRING_LENGTH, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "TATZEHKGU", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        10, // non-default-range min value
        20, // non-default-range man value
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        7, // test number
        "String in UNIX env outside custom length range",
        "STR_ENV_VAR_7", // variable to be tested
        "-1", // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_STRING_LENGTH, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "SSPEFTIJWQJMBFFVPMHCR", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        false, // if range verification, is default range used?
        10, // non-default-range min value
        20, // non-default-range man value
        false, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        8, // test number
        "String in UNIX env inside default length range",
        "STR_ENV_VAR_8", // variable to be tested
        "P", // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "P", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        9, // test number
        "String in UNIX env inside default length range",
        "STR_ENV_VAR_9", // variable to be tested
        "YIAJQOJVIXPAOLPGMOTCAOYSTPEMAJREWIEPWVAXFOLMXSAKQYWURGHHSNTASWWMMDXVVWHZAEBORUXWHIDVNCZHSSLPRRWVIUVRKCEQZIGQYWCJGLHAFPHWWJQNABI", // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "YIAJQOJVIXPAOLPGMOTCAOYSTPEMAJREWIEPWVAXFOLMXSAKQYWURGHHSNTASWWMMDXVVWHZAEBORUXWHIDVNCZHSSLPRRWVIUVRKCEQZIGQYWCJGLHAFPHWWJQNABI", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        10, // test number
        "String in UNIX env inside default length range",
        "STR_ENV_VAR_10", // variable to be tested
        "LD", // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "LD", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        11, // test number
        "String in UNIX env inside default length range",
        "STR_ENV_VAR_11", // variable to be tested
        "WMVDWXFTHVXISORMYLXQWZSYNBKYEWAJWCCGXOHJWGLFPZTSZTKCWUVBFLMCRHBDWQLZKNNHAWNYEPEMSXUNMZOWDTBAPZQVYDMXIFQUCVWJBQKBVYQFIZMBMFUQJR", // expected value on success or -1 if cannot succeed
        0, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "WMVDWXFTHVXISORMYLXQWZSYNBKYEWAJWCCGXOHJWGLFPZTSZTKCWUVBFLMCRHBDWQLZKNNHAWNYEPEMSXUNMZOWDTBAPZQVYDMXIFQUCVWJBQKBVYQFIZMBMFUQJR", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        true, // after test is done should there be a value in cache?
    },
    {
        __LINE__,
        12, // test number
        "String in UNIX env outside default range",
        "STR_ENV_VAR_12", // variable to be tested
        "-1", // expected value on success or -1 if cannot succeed
        Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_STRING_LENGTH, // expected return code
        true, // install a value into UNIX ENV pre-test?
        "KSZBOHHOSRPBBWMSSRWRLATFNXFISOPPDALDAYMDXZEEJUDORMWMGFKQULFVIAHLXHHOVSSBLPWGYDLREJQACOMYFUIKDLDILCLQXVJOEXJKJCODJEPKHRJBVUTRMIAM", // value to install into UNIX ENV or -1 not used. can be malformed
        true, // is value subject to range verification?
        true, // if range verification, is default range used?
        -1, // range min unused or useDefaultRange true
        -1, // range max unused or useDefaultRange true
        false, // after test is done should there be a value in cache?
    },
};

int numReinventUtilEnvironmentStringData() {
    return sizeof REINVENT_UTIL_ENVIRONMENT_STRING_DATA / sizeof *REINVENT_UTIL_ENVIRONMENT_STRING_DATA;
}

const StringTest *reinventUtilEnvironmentStringData(int i) {
    return (i>=0&&i<numReinventUtilEnvironmentStringData()) ? &(REINVENT_UTIL_ENVIRONMENT_STRING_DATA[i]) : 0;
}

