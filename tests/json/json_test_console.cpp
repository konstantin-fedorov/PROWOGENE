#define _CRT_SECURE_NO_WARNINGS

#include <cmath>
#include <iostream>
#include <map>
#include <string>

#include "utils/json.h"

using std::string;
using prowogene::utils::JsonValue;
using prowogene::utils::JsonObject;
using prowogene::utils::JsonArray;
using prowogene::utils::JsonType;
using prowogene::utils::InputString;

const float kEps = 0.000001f;

bool CheckInt(const int val_int, const string str_int) {
    JsonValue val_from_int = val_int;
    if (val_from_int.ToString() != str_int) {
        return false;
    }

    JsonValue val_from_str;
    val_from_str.Parse(str_int, InputString::DATA);
    int parsed_int = val_from_str;
    if (parsed_int != val_int) {
        return false;
    }

    if (val_from_int.ToString() != val_from_str.ToString()) {
        return false;
    }

    return true;
}

bool CheckFloat(const float val_float, const string str_float) {
    JsonValue val_from_str;
    val_from_str.Parse(str_float, InputString::DATA);
    float parsed_float = val_from_str;
    if (std::abs(parsed_float - val_float) > kEps) {
        return false;
    }

    JsonValue val_convert = val_float;
    val_convert.Parse(val_convert.ToString(), InputString::DATA);
    float converted_float = val_convert;
    if (std::abs(converted_float - val_float) > kEps) {
        return false;
    }

    return true;
}

bool NumberPositiveInteger() { return CheckInt(123, "123"); }

bool NumberNegativeInteger() { return CheckInt(-123, "-123"); }

bool NumberPositiveFloating() { return CheckFloat(123.123f, "123.123"); }

bool NumberNegativeFloating() { return CheckFloat(-123.123f, "-123.123"); }

bool NumberPositiveFloatingPositiveExponent() {
    return CheckFloat(123123.0f, "123.123E03") &&
           CheckFloat(123123.0f, "123.123e03");
}

bool NumberPositiveFloatingNegativeExponent() {
    return CheckFloat(0.123123f, "123.123E-03") &&
           CheckFloat(0.123123f, "123.123e-03");
}

bool NumberNegativeFloatingPositiveExponent() {
    return CheckFloat(-123123.0f, "-123.123E03") &&
           CheckFloat(-123123.0f, "-123.123e03");
}

bool NumberNegativeFloatingNegativeExponent() {
    return CheckFloat(-0.123123f, "-123.123E-03") &&
           CheckFloat(-0.123123f, "-123.123e-03");
}

bool BoolTrue() {
    JsonValue val;
    val.Parse("true", InputString::DATA);
    if (val.ToString() != "true") {
        return false;
    }
    val.Parse("True", InputString::DATA);
    if (val.GetType() != JsonType::UNDEFINED) {
        return false;
    }
    val.Parse("TRUE", InputString::DATA);
    if (val.GetType() != JsonType::UNDEFINED) {
        return false;
    }
    return true;
}

bool BoolFalse() {
    JsonValue val;
    val.Parse("false", InputString::DATA);
    if (val.ToString() != "false") {
        return false;
    }
    val.Parse("False", InputString::DATA);
    if (val.GetType() != JsonType::UNDEFINED) {
        return false;
    }
    val.Parse("FALSE", InputString::DATA);
    if (val.GetType() != JsonType::UNDEFINED) {
        return false;
    }
    return true;
}

bool Nullptr() {
    JsonValue val;
    val = nullptr;
    if (val.ToString() != "null") {
        return false;
    }
    val.Parse("null", InputString::DATA);
    if (val.GetType() != JsonType::NULLPTR) {
        return false;
    }

    val.Parse("NULL", InputString::DATA);
    if (val.GetType() != JsonType::UNDEFINED) {
        return false;
    }
    val.Parse("nullptr", InputString::DATA);
    if (val.GetType() != JsonType::UNDEFINED) {
        return false;
    }
    return true;
}

bool String() {
    std::vector<std::pair<string, string> > pairs = {
        {"123\\u5678", "\"123\\u5678\""},
        {"12\\u56783", "\"12\\u56783\""},
        {"\\u5678123", "\"\\u5678123\""},
        {"123\\u5678\\u5678", "\"123\\u5678\\u5678\""}};

    JsonValue val;
    for (const auto &pair : pairs) {
        val = pair.first;
        if (val.ToString() != pair.second) {
            return false;
        }
        val.Parse(pair.second, InputString::DATA);
        if (val.ToString() != pair.second) {
            return false;
        }
    }
    return true;
}

bool Array() {
    std::vector<std::string> must_success = {
        "[]",
        "[[[[]]]]",
        "[[],[],[]]",
        "[1,2,3,4,5]",
        "[1,\n2,\t3    , 4, 5]",
        "[1, null, [], {}, true, false, -0.5]"};
    std::vector<std::string> must_fail = {
        "[",
        "[[]",
        "[][",
        "][]",
        "[0,]",
        "[0,null 1, 3]",
        "[456,[[]],[[,]]]",
        "[[][]]",
        "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[\
         [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[\
         [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[\
         [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[\
         [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[\
         [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[\
         [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[\
         [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[\
         [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[\
         [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[\
         ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]\
         ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]\
         ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]\
         ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]\
         ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]\
         ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]\
         ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]\
         ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]\
         ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]\
         ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]",
        "[0,nu"};
    JsonValue val;
    for (auto &data : must_success) {
        val.Parse(data, InputString::DATA);
        if (val.GetType() != JsonType::ARRAY) {
            return false;
        }
    }
    for (auto &data : must_fail) {
        val.Parse(data, InputString::DATA);
        if (val.GetType() != JsonType::UNDEFINED) {
            return false;
        }
    }
    return true;
}

bool Object() {
    std::vector<std::string> must_success = {
        "{}", "{\"1\":1}", "{\"1\"\t\n:  1 } ", "{\"1\":[1,2,3,4,5]}",
        "{\"1\":{\"1\":{\"1\":{\"1\":1}}}, \"2\":1}"};
    std::vector<std::string> must_fail = {"{", "{1:1}", "{\"1\":1 \"2\":2}",
                                           "{\"1\":1, 2}", "{\"1\":1, []:2}"};
    JsonValue val;
    for (auto &data : must_success) {
        val.Parse(data, InputString::DATA);
        if (val.GetType() != JsonType::OBJECT) {
            return false;
        }
    }
    for (auto &data : must_fail) {
        val.Parse(data, InputString::DATA);
        if (val.GetType() != JsonType::UNDEFINED) {
            std::cout << data << std::endl;
            std::cout << val.ToString() << std::endl;
            return false;
        }
    }
    return true;
}

bool File() {
    std::string filename = "json_test_file.json";
    remove(filename.c_str());
    JsonValue val;
    val.Parse(filename, InputString::FILENAME);
    if (val.GetType() != JsonType::UNDEFINED) {
        return false;
    }

    string check_val = "{\"first\" : 1, \"second\" : null}    ";
    val.Parse(check_val, InputString::DATA);
    if (val.GetType() != JsonType::OBJECT) {
        return false;
    }

    val.Save(filename);
    val.Parse(filename, InputString::FILENAME);
    if (val.GetType() != JsonType::OBJECT) {
        return false;
    }
    JsonObject obj = val;
    int first_field = obj["first"];
    if (first_field != 1 ||
        obj["second"].GetType() != JsonType::NULLPTR) {
        return false;
    }

    val.Parse("", InputString::FILENAME);
    if (val.GetType() != JsonType::UNDEFINED) {
        return false;
    }

    remove(filename.c_str());
    return true;
}

typedef bool (*TestFuncPtr)();

const std::map<string, TestFuncPtr> kTests = {
    {"json-pos-int", NumberPositiveInteger},
    {"json-neg-int", NumberNegativeInteger},
    {"json-pos-float", NumberPositiveFloating},
    {"json-neg-float", NumberNegativeFloating},
    {"json-pos-float-pos-exp", NumberPositiveFloatingPositiveExponent},
    {"json-pos-float-neg-exp", NumberPositiveFloatingNegativeExponent},
    {"json-neg-float-pos-exp", NumberNegativeFloatingPositiveExponent},
    {"json-neg-float-neg-exp", NumberNegativeFloatingNegativeExponent},
    {"json-bool-true", BoolTrue},
    {"json-bool-false", BoolFalse},
    {"json-null", Nullptr},
    {"json-string", String},
    {"json-array", Array},
    {"json-object", Object},
    {"json-file", File}
};

int main(int argc, const char **argv) {
    if (argc != 2) {
        std::cout << "No command line arguements" << std::endl;
        return -1;
    }

    string test_name = argv[1];
    auto test_func = kTests.find(test_name);
    if (test_func == kTests.end()) {
        return -1;
    } else {
        if (test_func->second()) {
            std::cout << "Passed test \"" << test_name << "\"" << std::endl;
            return 0;
        } else {
            std::cout << "Not passed test \"" << test_name << "\"" << std::endl;
            return -1;
        }
    }
}