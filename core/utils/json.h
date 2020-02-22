#ifndef PROWOGENE_CORE_UTILS_JSON_H_
#define PROWOGENE_CORE_UTILS_JSON_H_

#include <map>
#include <string>
#include <vector>

namespace prowogene {
namespace utils {

/** Maximal nesting in both JSON objects and arrays. */
static const int kJsonMaxNesting = 255;
/** Default indentation of serialized JSON file. */
static const int kJsonDefaultIndent = 2;


/** Type of JSON value. */
enum class JsonType {
    /** Unitialized or corrupted data. */
    UNDEFINED,
    /** String. */
    STRING,
    /** Number (both integer or floating point). */
    NUMBER,
    /** Boolean. */
    BOOL,
    /** JSON object. */
    OBJECT,
    /** JSON array. */
    ARRAY,
    /** JSON null. */
    NULLPTR
};


/** Input string interpretation type. */
enum class InputString {
    /** Input string is a filename of JSON file. */
    FILENAME,
    /** Input string is JSON data itself. */
    DATA
};


class JsonValue;
/** JSON array. */
using JsonArray = std::vector<JsonValue> ;
/** JSON object. */
using JsonObject = std::map<std::string, JsonValue>;


/** @brief JSON value that stores value of some type. */
class JsonValue {
 public:
    /** Constructor */
    JsonValue();

    /** Constructor
    @param [in] str - String value. */
    JsonValue(const std::string& str);

    /** @copydoc JsonValue::JsonValue(const std::string&) */
    JsonValue(const char* str);

    /** Constructor
    @param [in] num - Number value. */
    JsonValue(int num);

    /** @copydoc JsonValue::JsonValue(int) */
    JsonValue(unsigned int num);

    /** @copydoc JsonValue::JsonValue(int) */
    JsonValue(float num);

    /** Constructor
    @param [in] boolean - Boolean value. */
    JsonValue(bool boolean);

    /** Constructor
    @param [in] arr - JSON array value. */
    JsonValue(const JsonArray& arr);

    /** Constructor
    @param [in] obj - JSON object value. */
    JsonValue(const JsonObject& obj);

    /** Copy constructor
    @param [in] val - JSON value value. */
    JsonValue(const JsonValue& val);

    /** Constructor
    @param [in] ptr - Null value. */
    JsonValue(const nullptr_t& ptr);


    /** Get string if type is correct.
    Otherwise, returns @c "" . */
    virtual operator std::string() const;

    /** Get int if type is correct.
    Otherwise, returns @c 0 . */
    virtual operator int() const;

    /** Get unsigned int if type is correct.
    Otherwise, returns @c 0 . */
    virtual operator unsigned int() const;

    /** Get float if type is correct.
    Otherwise, returns @c 0.0f . */
    virtual operator float() const;

    /** Get bool if type is correct.
    Otherwise, returns @c false . */
    virtual operator bool() const;

    /** Get JsonArray if type is correct.
    Otherwise, returns empty JsonArray. */
    virtual operator JsonArray() const;

    /** Get JsonObject if type is correct.
    Otherwise, returns empty JsonObject . */
    virtual operator JsonObject() const;

    /** Get nullptr. */
    virtual operator nullptr_t() const;

    /** Get JsonValue by index if type is JsonType::ARRAY.
    Otherwise, returns JsonValue with type JsonType::UNDEFINED. */
    virtual JsonValue& operator[](std::size_t idx);

    /** @copydoc JsonValue::operator[](std::size_t). */
    virtual const JsonValue& operator[](std::size_t idx) const;

    /** Parse JSON.
    @param [in] data - String data.
    @param [in] input - Type of data parsing.
    @returns @c true if pasing succeeded, @c false otherwise. */
    virtual bool Parse(const std::string& data, InputString input);

    /** Serialize JSON value to string.
    @param [in] pretty - add indentations and new lines or not. */
    virtual std::string ToString(bool pretty = true) const;

    /** @copydoc operator std::string() */
    virtual std::string Str() const;

    /** Copy all members from another value.
    @param [in] src - Input JSON value for copying. */
    virtual void Copy(const JsonValue& src);

    /** Save JSON to file.
    @param [in] filename - File name to save.
    @param [in] pretty - add indentations and new lines or not. */
    virtual void Save(const std::string& filename, bool pretty = true) const;

    /** Clear all members. */
    virtual void Clear();

    /** Get current JSON type for this object.
    @return Current JSON type. */
    virtual JsonType GetType();

 protected:
    /** Parse JSON data.
    @param [in] data  - Data to parse.
    @param [in] depth - Current depth. */
    virtual bool Parse(const std::string& data, int depth);

    /** Serialize JSON value to string.
    @param [in] pretty - Add indentations and new lines or not.
    @param [in] indent - Indentation per one level in spaces.
    @return JSON data itself. */
    virtual std::string ToString(bool pretty, int indent) const;

    /** Serialize JSON object value to string.
    @param [in] pretty - Add indentations and new lines or not.
    @param [in] indent - Indentation per one level in spaces.
    @return JSON data itself. */
    virtual std::string ObjectToString(bool pretty, int indent) const;

    /** Serialize JSON array value to string.
    @param [in] pretty - Add indentations and new lines or not.
    @param [in] indent - Indentation per one level in spaces.
    @return JSON data itself. */
    virtual std::string ArrayToString(bool pretty, int indent) const;


    /** Parse JSON data.
    @param [in] data  - Data to parse.
    @param [in] beg   - First symbol position to start parse.
    @param [in] pos   - First symbol position that didn't parsed.
    @param [in] depth - Current depth.
    @return Parsed JSON value. */
    static JsonValue Parse(const std::string& data,
                           int beg,
                           int& pos,
                           int depth);

    /** Get pattern comparision to current data at specified position.
    @param [in] data   - Data to parse.
    @param [in] beg    - First symbol position to start parse.
    @param [in] pos    - First symbol position that didn't parsed.
    @param [in] patern - Pattern to detect.
    @return @c true if pattern founded at specified position, @c false otherwise. */
    static bool PatternDetected(const std::string& data,
                                int beg,
                                int& pos,
                                const std::string& pattern);

    /** Skip non-data symbols like spaces, tabs, new lines, etc.
    @param [in] data - Data to skip spaces.
    @param [in] beg  - First symbol position to start skip.
    @return Position of first non-space value. */
    static int SkipSpaces(const std::string& data, int beg);

    /** Parse data part as string.
    @param [in] data - Data to parse.
    @param [in] beg  - First symbol position to start parse.
    @param [in] pos  - First symbol position that didn't parsed.
    @return JSON value with string. */
    static JsonValue ParseString(const std::string& data, int beg, int& pos);

    /** Parse data part as number.
    @param [in] data - Data to parse.
    @param [in] beg  - First symbol position to start parse.
    @param [in] pos  - First symbol position that didn't parsed.
    @return JSON value with number. */
    static JsonValue ParseNumber(const std::string& data, int beg, int& pos);

    /** Parse data part as boolean.
    @param [in] data - Data to parse.
    @param [in] beg  - First symbol position to start parse.
    @param [in] pos  - First symbol position that didn't parsed.
    @return JSON value with boolean. */
    static JsonValue ParseBoolean(const std::string& data, int beg, int& pos);

    /** Parse data part as null.
    @param [in] data - Data to parse.
    @param [in] beg  - First symbol position to start parse.
    @param [in] pos  - First symbol position that didn't parsed.
    @return JSON value with null. */
    static JsonValue ParseNull(const std::string& data, int beg, int& pos);

    /** Parse data part as JSON object.
    @param [in] data - Data to parse.
    @param [in] beg  - First symbol position to start parse.
    @param [in] pos  - First symbol position that didn't parsed.
    @return JSON value with JSON object. */
    static JsonValue ParseObject(const std::string& data,
                                 int beg,
                                 int& pos,
                                 int depth);

    /** Parse data part as JSON array.
    @param [in] data - Data to parse.
    @param [in] beg  - First symbol position to start parse.
    @param [in] pos  - First symbol position that didn't parsed.
    @return JSON value with JSON array. */
    static JsonValue ParseArray(const std::string& data,
                                int beg,
                                int& pos,
                                int depth);


    /** Type of value. */
    JsonType    type_ = JsonType::UNDEFINED;
    /** Integer value. */
    int         int_number_ = 0;
    /** Floating point value. */
    float       float_number_ = 0.0f;
    /** Which number value is primary - integer or floating point. */
    bool        is_integer_ = false;
    /** String value. */
    std::string string_ = "";
    /** JSON array value. */
    JsonArray   array_;
    /** Boolean value. */
    bool        bool_ = false;
    /** JSON object value. */
    JsonObject  object_;
};

} // namespace prowogene
} // namespace utils

#endif  // PROWOGENE_CORE_UTILS_JSON_H_
