// -----------------------------------------------------------------------------
//  File    : Core/Data/JSONUtils.h
//  Module  : HiveMnd :: Data
//  Purpose : Lightweight JSON helpers without third-party libs.
//            - Builds JSON from flat maps (string->string).
//            - Parses *simple* JSON objects (no nesting).
//            - Validates basic JSON shape.
//            - Read/Write JSON to disk.
//  Notes   : This is intentionally minimal to avoid external deps. It safely
//            escapes strings and tolerates whitespace. For complex JSON
//            (arrays, nesting, numbers/booleans), consider adding a vendor
//            parser later. All errors are logged via LogUtils.
// -----------------------------------------------------------------------------

#pragma once
#include <string>
#include <map>
#include <optional>

namespace HiveMnd::Data
{
    class JSONUtils
    {
    public:
        // ---------------------------------------------------------------------
        //  Function: Escape
        //  Purpose : JSON-escape a string (quotes, backslashes, control chars).
        //  @param   s - raw input
        //  @return  escaped string (without wrapping quotes)
        // ---------------------------------------------------------------------
        static std::string Escape(const std::string& s);

        // ---------------------------------------------------------------------
        //  Function: ToJSONString
        //  Purpose : Convert a flat map<string,string> to a JSON object string.
        //  @param   obj     - key/value pairs
        //  @param   pretty  - pretty-print with indentation if true
        //  @param   indent  - number of spaces per indentation level
        //  @return  JSON text like: {"k":"v", "a":"b"}
        // ---------------------------------------------------------------------
        static std::string ToJSONString(
            const std::map<std::string, std::string>& obj,
            bool pretty = true,
            int indent = 2);

        // ---------------------------------------------------------------------
        //  Function: ValidateBasic
        //  Purpose : Quick shape check: looks like a single JSON object?
        //  @param   json - input text
        //  @return  true if it starts with '{' and ends with '}' (ignoring ws)
        // ---------------------------------------------------------------------
        static bool ValidateBasic(const std::string& json);

        // ---------------------------------------------------------------------
        //  Function: FromSimpleObject
        //  Purpose : Parse *flat* JSON object into a map<string,string>.
        //            Assumes values are JSON strings; ignores nested objects.
        //  @param   json - input text
        //  @param   out  - receives key/value pairs
        //  @return  true on success, false with errors logged
        // ---------------------------------------------------------------------
        static bool FromSimpleObject(
            const std::string& json,
            std::map<std::string, std::string>& out);

        // ---------------------------------------------------------------------
        //  Function: GetString
        //  Purpose : Convenience: extract a single string value by key from a
        //            JSON object string (simple parser).
        //  @param   json - input text
        //  @param   key  - key to find
        //  @return  value if found/parsed, std::nullopt otherwise
        // ---------------------------------------------------------------------
        static std::optional<std::string> GetString(
            const std::string& json,
            const std::string& key);

        // ---------------------------------------------------------------------
        //  Function: WriteObjectToFile
        //  Purpose : Serialize a flat map to JSON and save to a file.
        //  @param   path   - output file path
        //  @param   obj    - data to write
        //  @param   pretty - pretty-print output
        //  @return  true on success, false if file or write failed (logged)
        // ---------------------------------------------------------------------
        static bool WriteObjectToFile(
            const std::string& path,
            const std::map<std::string, std::string>& obj,
            bool pretty = true);

        // ---------------------------------------------------------------------
        //  Function: ReadObjectFromFile
        //  Purpose : Load a JSON file and parse into a flat map (string->string)
        //  @param   path - file path
        //  @param   out  - receives parsed pairs
        //  @return  true on success, false with errors logged
        // ---------------------------------------------------------------------
        static bool ReadObjectFromFile(
            const std::string& path,
            std::map<std::string, std::string>& out);

    private:
        // Trim helpers (ASCII whitespace).
        static void Trim(std::string& s);
        static size_t SkipWS(const std::string& s, size_t i);

        // Internal string-parse helper: parses a JSON string literal at s[pos],
        // returns unescaped contents and advances pos past ending quote.
        static bool ParseJSONString(const std::string& s, size_t& pos, std::string& outStr);

        // Unescape \" \\ \n \r \t and \uXXXX (basic BMP hex handling).
        static bool Unescape(const std::string& in, std::string& out);
    };
}

