#include "json.h"

#include <cmath>

#include <fstream>

namespace prowogene {
namespace utils {

using std::map;
using std::vector;
using std::string;

JsonValue::JsonValue()
    : type_(JsonType::UNDEFINED)
    , int_number_(0)
    , float_number_(0.0f)
    , is_integer_(true)
    , bool_(false) {
}

JsonValue::JsonValue(int num) : JsonValue::JsonValue() {
    type_ = JsonType::NUMBER;
    int_number_ = num;
    float_number_ = static_cast<float>(num);
    is_integer_ = true;
}

JsonValue::JsonValue(unsigned int num) : JsonValue::JsonValue() {
    type_ = JsonType::NUMBER;
    int_number_ = static_cast<int>(num);
    float_number_ = static_cast<float>(num);
    is_integer_ = true;
}

JsonValue::JsonValue(float num) : JsonValue::JsonValue() {
    type_ = JsonType::NUMBER;
    int_number_ = static_cast<int>(num);
    float_number_ = num;
    is_integer_ = false;
}

JsonValue::JsonValue(bool boolean) : JsonValue::JsonValue() {
    type_ = JsonType::BOOL;
    bool_ = boolean;
}

JsonValue::JsonValue(const JsonArray& arr) : JsonValue::JsonValue() {
    type_ = JsonType::ARRAY;
    array_ = arr;
}

JsonValue::JsonValue(const JsonObject& obj)
    : JsonValue::JsonValue() {
    type_ = JsonType::OBJECT;
    object_ = obj;
}

JsonValue::JsonValue(const JsonValue& val)
        : JsonValue::JsonValue() {
    type_ =         val.type_;
    int_number_ =   val.int_number_;
    float_number_ = val.float_number_;
    is_integer_ =   val.is_integer_;
    string_ =       val.string_;
    array_ =        val.array_;
    bool_ =         val.bool_;
    object_ =       val.object_;
}

JsonValue::JsonValue(const nullptr_t& ptr) : JsonValue::JsonValue() {
    type_ = JsonType::NULLPTR;
}

JsonValue::JsonValue(const string& str) : JsonValue::JsonValue() {
    type_ = JsonType::STRING;
    string_ = str;
}

JsonValue::JsonValue(const char* str)
    : JsonValue::JsonValue(string(str)) {
}

JsonValue::operator int() const {
    if (type_ == JsonType::NUMBER) {
        return int_number_;
    } else {
        return 0;
    }
}

JsonValue::operator unsigned int() const {
    if (type_ == JsonType::NUMBER) {
        return static_cast<unsigned int>(int_number_);
    } else {
        return 0;
    }
}

JsonValue::operator float() const {
    if (type_ == JsonType::NUMBER) {
        return float_number_;
    } else {
        return 0.0f;
    }
}

JsonValue::operator bool() const {
    if (type_ == JsonType::BOOL) {
        return bool_;
    } else {
        return false;
    }
}

JsonValue::operator JsonArray() const {
    if (type_ == JsonType::ARRAY) {
        return array_;
    } else {
        return vector<JsonValue>();
    }
}

JsonValue::operator JsonObject() const {
    if (type_ == JsonType::OBJECT) {
        return object_;
    } else {
        return JsonObject();
    }
}

JsonValue::operator nullptr_t() const { return nullptr; }

JsonValue::operator string() const {
    if (type_ == JsonType::STRING) {
        return string_;
    } else {
        return "";
    }
}

JsonValue& JsonValue::operator[](size_t idx) {
    if (type_ == JsonType::ARRAY && array_.size() > idx) {
        return array_[idx];
    } else {
        return *this;
    }
}

const JsonValue& JsonValue::operator[](size_t idx) const {
    if (type_ == JsonType::ARRAY && array_.size() > idx) {
        return array_[idx];
    } else {
        return *this;
    }
}

bool JsonValue::Parse(const string& data, InputString input) {
    if (input == InputString::DATA) {
        return Parse(data, 0);
    } else if (input == InputString::FILENAME) {
        const string name(data.begin(), data.end());
        std::ifstream fs(name);
        if (!fs.is_open()) {
            type_ = JsonType::UNDEFINED;
            return false;
        }
        const string content((std::istreambuf_iterator<char>(fs)),
                             (std::istreambuf_iterator<char>()));
        return Parse(content, 0);
    }
    type_ = JsonType::UNDEFINED;
    return false;
}

string JsonValue::ToString(bool pretty) const {
    return ToString(pretty, 0);
}

string JsonValue::Str() const {
    if (type_ == JsonType::STRING) {
        return string_;
    } else {
        return "";
    }
}

void JsonValue::Copy(const JsonValue& src) {
    type_ =         src.type_;
    int_number_ =   src.int_number_;
    float_number_ = src.float_number_;
    is_integer_ =   src.is_integer_;
    string_ =       src.string_;
    array_ =        src.array_;
    bool_ =         src.bool_;
    object_ =       src.object_;
}

void JsonValue::Save(const string& filename, bool pretty) const {
    std::ofstream ofs;
    ofs.open(filename);
    if (!ofs.is_open()) {
        return;
    }
    ofs << ToString(pretty);
    ofs.close();
}

void JsonValue::Clear() {
    type_ = JsonType::UNDEFINED;
    string_.clear();
    array_.clear();
    object_.clear();
}

JsonType JsonValue::GetType() {
    return type_;
}

bool JsonValue::Parse(const string& data, int depth) {
    if (depth >= kJsonMaxNesting) {
        return false;
    }
    int pos = 0;
    const JsonValue buffer = Parse(data, 0, pos, depth);
    this->Copy(buffer);
    const int data_length = static_cast<int>(data.length());
    pos = SkipSpaces(data, pos);
    if ((pos < data_length) && (!depth)) {
        this->type_ = JsonType::UNDEFINED;
        return false;
    }
    return true;
}

string JsonValue::ToString(bool pretty, int indent) const {
    switch (type_) {
        case JsonType::STRING: {
            string str = string_;
            size_t index = 0;
            while (true) {
                index = str.find("\"", index);
                if (index == string::npos) {
                    break;
                }
                str.replace(index, 2, "\\\"");
                index += 2;
            }
            return "\"" + str + "\"";
        } break;
        case JsonType::NUMBER:
            if (is_integer_) {
                return std::to_string(int_number_);
            } else {
                return std::to_string(float_number_);
            }
            break;
        case JsonType::BOOL:
            if (bool_) {
                return "true";
            } else {
                return "false";
            }
            break;
        case JsonType::OBJECT:
            return ObjectToString(pretty, indent);
            break;
        case JsonType::ARRAY:
            return ArrayToString(pretty, indent);
            break;
        case JsonType::NULLPTR:
            return "null";
            break;
        case JsonType::UNDEFINED:
        default:
            return "UNDEFINED";
            break;
    }
}

string JsonValue::ObjectToString(bool pretty, int indent) const {
    if (!pretty) {
        string out = "{";
        auto it = object_.begin();
        while (it != object_.end()) {
            out += "\"" + it->first + "\":" +
                   it->second.ToString(pretty, indent);
            if (++it != object_.end()) {
                out += ",";
            }
        }
        out += "}";
        return out;
    }

    string indent_str;
    for (int i = 0; i < indent; ++i) {
        indent_str += " ";
    }
    string indent_big_str = indent_str;
    for (int i = 0; i < kJsonDefaultIndent; ++i) {
        indent_big_str += " ";
    }
    indent += kJsonDefaultIndent;
    string out = "{";
    auto it = object_.begin();
    while (it != object_.end()) {
        out += "\n" + indent_big_str + "\"" + it->first + "\" : " +
               it->second.ToString(pretty, indent);
        if (++it != object_.end()) {
            out += ",";
        }
    }
    indent -= kJsonDefaultIndent;
    if (object_.size()) {
        out += "\n";
        out += indent_str;
    }
    out += "}";
    return out;
}

string JsonValue::ArrayToString(bool pretty, int indent) const {
    if (!pretty) {
        string out = "[";
        for (size_t i = 0; i < array_.size(); ++i) {
            out += array_[i].ToString(pretty, indent);
            if (i < array_.size() - 1) {
                out += ",";
            }
        }
        out += "]";
        return out;
    }

    string indent_str;
    for (int i = 0; i < indent; ++i) {
        indent_str += " ";
    }
    string indent_big_str = indent_str;
    for (int i = 0; i < kJsonDefaultIndent; ++i) {
        indent_big_str += " ";
    }
    string out = "[";
    indent += kJsonDefaultIndent;
    if (array_.size()) {
        out += "\n";
        out += indent_big_str;
    }
    for (size_t i = 0; i < array_.size(); ++i) {
        out += array_[i].ToString(pretty, indent);
        if (i < array_.size() - 1) {
            out += ",\n";
            out += indent_big_str;
        }
    }
    indent -= kJsonDefaultIndent;
    if (array_.size()) {
        out += "\n";
        out += indent_str;
    }
    out += "]";
    return out;
}

JsonValue JsonValue::Parse(const string& data, int beg, int& pos, int depth) {
    if (depth >= kJsonMaxNesting) {
        return JsonValue();
    }
    pos = SkipSpaces(data, beg);
    JsonValue val;
    switch (data[pos]) {
        case L'{':
            val = val.ParseObject(data, pos, pos, depth + 1);
            break;
        case L'\"':
            val = val.ParseString(data, pos, pos);
            break;
        case L't':
        case L'f':
            val = val.ParseBoolean(data, pos, pos);
            break;
        case L'n':
            val = val.ParseNull(data, pos, pos);
            break;
        case L'-':
        case L'0':
        case L'1':
        case L'2':
        case L'3':
        case L'4':
        case L'5':
        case L'6':
        case L'7':
        case L'8':
        case L'9':
            val = val.ParseNumber(data, pos, pos);
            break;
        case L'[':
            val = val.ParseArray(data, pos, pos, depth + 1);
            break;
        default:
            val = JsonValue();
            break;
    }
    return val;
}

inline bool JsonValue::PatternDetected(const string& data, int beg, int& pos,
        const string& pattern) {
    const int data_length = static_cast<int>(data.length());
    const int pattern_length = static_cast<int>(pattern.length());

    if (data_length < beg + pattern_length) {
        return false;
    }
    for (int i = 0; i < pattern_length; ++i) {
        const int data_i = beg + i;
        if (data[data_i] != pattern[i]) {
            return false;
        }
    }
    pos = beg + pattern_length;
    return true;
}

int JsonValue::SkipSpaces(const string& data, int beg) {
    const int char_count = static_cast<int>(data.length());
    while (beg < char_count) {
        if (data[beg] == L' ' ||
                data[beg] == L'\t' ||
                data[beg] == L'\r' ||
                data[beg] == L'\n') {
            ++beg;
            continue;
        } else {
            break;
        }
    }
    return beg;
}

JsonValue JsonValue::ParseString(const string& data, int beg, int& pos) {
    const int data_length = static_cast<int>(data.length());
    if (data[beg] != L'\"') {
        return JsonValue();
    }
    ++pos;
    string str = "";
    while (pos < data_length) {
        char c = data[pos];
        if (c == L'\"') {
            break;
        }
        if (c == L'\\') {
            ++pos;
            c = data[pos];
            if (pos == data_length) {
                return JsonValue();
            }
            int offset = 1;
            switch (c) {
                case L'"':
                    str.push_back('\"');
                    break;
                case L'\\':
                    str.push_back('\\');
                    break;
                case L'/':
                    str.push_back('/');
                    break;
                case L'b':
                    str.push_back('\b');
                    break;
                case L'f':
                    str.push_back('\f');
                    break;
                case L'n':
                    str.push_back('\n');
                    break;
                case L'r':
                    str.push_back('\r');
                    break;
                case L't':
                    str.push_back('\t');
                    break;
                case L'u':
                    if (data_length < pos + 5) {
                        return JsonValue();
                    }
                    for (int i = pos + 1; i < pos + 5; ++i) {
                        if ((data[i] < L'A' || data[i] > 'F') &&
                                (data[i] < L'0' || data[i] > '9')) {
                            return JsonValue();
                        }
                    }
                    offset = 5;
                    str += data.substr(pos - 1, 6);
                    break;
                default:
                    offset = 0;
                    break;
            }
            pos += offset;
            c = data[pos];
            continue;
        }
        str.push_back(data[pos]);
        ++pos;
    }
    if (data[pos] != '\"') {
        return JsonValue();
    }
    ++pos;
    return JsonValue(str);
}

JsonValue JsonValue::ParseNumber(const string& data, int beg, int& pos) {
    pos = beg;
    const int data_length = static_cast<int>(data.length());
    char c = data[pos];

    bool is_minus = false;
    if (c == L'-') {
        is_minus = true;
        ++pos;
        c = data[pos];
    }

    int integer = 0;
    bool counting_leading_zeros = true;
    int leading_zeros_count = 0;
    while ((pos < data_length) && (c >= L'0' && c <= L'9')) {
        if (counting_leading_zeros) {
            if (c == L'0') {
                ++leading_zeros_count;
                if (leading_zeros_count > 1) {
                    return JsonValue();
                }
            } else {
                counting_leading_zeros = false;
            }
        }
        integer *= 10;
        integer += c - L'0';
        ++pos;
        c = data[pos];
    }
    integer = is_minus ? -integer : integer;

    if ((pos == data_length) || (c != L'.' && c != L'e' && c != L'E')) {
        return JsonValue(integer);
    }

    int fraction = 0;
    int fraction_numbers = 0;
    if (c == L'.') {
        ++pos;
        c = data[pos];
        while ((pos < data_length) && (c >= L'0' && c <= L'9')) {
            fraction *= 10;
            fraction += c - L'0';
            ++fraction_numbers;
            ++pos;
            c = data[pos];
        }
    }
    if (!fraction_numbers && (c != L'e' && c != L'E')) {
        return JsonValue();
    }

    float real_frac = static_cast<float>(fraction) /
                      powf(10.0f, static_cast<float>(fraction_numbers));
    float floating = static_cast<float>(integer) +
                     (is_minus ? -real_frac : real_frac);

    if ((pos == data_length) || (c != L'e' && c != L'E')) {
        return JsonValue(floating);
    }

    ++pos;
    if (pos == data_length) {
        return JsonValue();
    }
    c = data[pos];
    if ((c < L'0' || c > L'9') && (c != L'-' && c != L'+')) {
        return JsonValue();
    }
    bool is_exponent_minus = false;
    if (c == L'-') {
        is_exponent_minus = true;
        ++pos;
        c = data[pos];
    } else if (c == L'+') {
        ++pos;
        c = data[pos];
    }

    int exponent = 0;
    while ((pos < data_length) && (c >= L'0' && c <= L'9')) {
        exponent *= 10;
        exponent += c - L'0';
        ++pos;
        c = data[pos];
    }
    if (is_exponent_minus) {
        exponent = -exponent;
    }
    floating *= powf(10.0f, static_cast<float>(exponent));
    return JsonValue(floating);
}

JsonValue JsonValue::ParseBoolean(const string& data, int beg, int& pos) {
    if (data[beg] == L't') {
        if (PatternDetected(data, beg, pos, "true")) {
            return JsonValue(true);
        } else {
            return JsonValue();
        }
    }
    if (data[beg] == L'f') {
        if (PatternDetected(data, beg, pos, "false")) {
            return JsonValue(false);
        } else {
            return JsonValue();
        }
    }
    return JsonValue();
}

JsonValue JsonValue::ParseNull(const string& data, int beg, int& pos) {
    if (PatternDetected(data, beg, pos, "null")) {
        return JsonValue(nullptr);
    } else {
        return JsonValue();
    }
}

JsonValue JsonValue::ParseObject(const string& data, int beg, int& pos,
        int depth) {
    if (depth >= kJsonMaxNesting) {
        return JsonValue();
    }
    const int data_length = static_cast<int>(data.length());
    if (data[beg] != L'{') {
        return JsonValue();
    }
    pos = beg + 1;
    bool needed_key = true;
    bool needed_colon = false;
    bool needed_comma = false;
    string key;
    JsonValue obj;
    obj.type_ = JsonType::OBJECT;
    while (pos < data_length) {
        pos = SkipSpaces(data, pos);
        if (data[pos] == L'}' && !needed_colon) {
            ++pos;
            return obj;
        }
        if (data[pos] == L':') {
            if (needed_colon) {
                ++pos;
                needed_colon = false;
                continue;
            } else {
                return JsonValue();
            }
        }
        if (data[pos] == L',') {
            if (needed_comma) {
                ++pos;
                needed_comma = false;
                continue;
            } else {
                return JsonValue();
            }
        }
        if (needed_key && !needed_comma) {
            JsonValue val = ParseString(data, pos, pos);
            if (val.type_ == JsonType::UNDEFINED) {
                return JsonValue();
            }
            key = val.string_;
            needed_key = false;
            needed_colon = true;
            needed_comma = false;
            continue;
        } else {
            JsonValue val = Parse(data, pos, pos, depth);
            if (val.type_ == JsonType::UNDEFINED) {
                return JsonValue();
            }
            needed_key = true;
            needed_colon = false;
            needed_comma = true;
            obj.object_[key] = val;
        }
    }
    return JsonValue();
}

JsonValue JsonValue::ParseArray(const string& data, int beg, int& pos,
        int depth) {
    if (depth >= kJsonMaxNesting) {
        return JsonValue();
    }
    const int data_length = static_cast<int>(data.length());
    pos = beg;
    if (data[beg] != L'[') {
        return JsonValue();
    }
    JsonArray arr;
    char c = data[pos];
    pos = beg + 1;
    bool needed_val = true;
    bool needed_comma = false;
    bool last_is_comma = false;
    while (pos < data_length) {
        pos = SkipSpaces(data, pos);
        c = data[pos];
        if (c == L',') {
            if (needed_val && !needed_comma) {
                return JsonValue();
            } else {
                ++pos;
                needed_val = true;
                last_is_comma = true;
                needed_comma = false;
                continue;
            }
        }
        if (c == L']') {
            ++pos;
            if (last_is_comma) {
                return JsonValue();
            } else {
                return arr;
            }
        }
        if (needed_comma) {
            return JsonValue();
        }
        JsonValue val = Parse(data, pos, pos, depth);
        if (val.type_ == JsonType::UNDEFINED) {
            return JsonValue();
        }
        arr.push_back(val);
        needed_val = false;
        last_is_comma = false;
        needed_comma = true;
    }
    return JsonValue();
}

} // namespace prowogene
} // namespace utils
