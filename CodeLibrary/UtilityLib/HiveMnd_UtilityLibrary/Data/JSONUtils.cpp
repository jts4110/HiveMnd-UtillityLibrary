// -----------------------------------------------------------------------------
//  File    : Core/Data/JSONUtils.cpp
//  Depends : <fstream>, <sstream>, <vector>, <cctype>
//  Logs    : HiveMnd::Core::LogUtils for all error/info output
// -----------------------------------------------------------------------------
#include "JSONUtils.h"
#include "Core/LogUtils.h"

#include <map>
#include <optional>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cctype>


using HiveMnd::Core::LogUtils;

namespace HiveMnd::Data
{
    // --- small helpers -------------------------------------------------------
    static bool IsWS(char c)
    {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
    }

    void JSONUtils::Trim(std::string& s)
    {
        auto it1 = std::find_if_not(s.begin(), s.end(), IsWS);
        auto it2 = std::find_if_not(s.rbegin(), s.rend(), IsWS).base();
        s = (it1 < it2) ? std::string(it1, it2) : std::string();
    }

    size_t JSONUtils::SkipWS(const std::string& s, size_t i)
    {
        while (i < s.size() && IsWS(s[i])) ++i;
        return i;
    }

    // --- escaping / unescaping ----------------------------------------------
    std::string JSONUtils::Escape(const std::string& s)
    {
        std::string out;
        out.reserve(s.size() + 8);
        for (unsigned char c : s)
        {
            switch (c)
            {
            case '\"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b";  break;
            case '\f': out += "\\f";  break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (c < 0x20)
                {
                    // control char -> \u00XX
                    std::ostringstream oss;
                    oss << "\\u"
                        << std::hex << std::uppercase << std::setw(4) << std::setfill('0')
                        << static_cast<int>(c);
                    out += oss.str();
                }
                else
                {
                    out += static_cast<char>(c);
                }
            }
        }
        return out;
    }

    static int HexVal(char h)
    {
        if (h >= '0' && h <= '9') return h - '0';
        if (h >= 'a' && h <= 'f') return 10 + (h - 'a');
        if (h >= 'A' && h <= 'F') return 10 + (h - 'A');
        return -1;
    }

    bool JSONUtils::Unescape(const std::string& in, std::string& out)
    {
        out.clear();
        out.reserve(in.size());
        for (size_t i = 0; i < in.size(); ++i)
        {
            char c = in[i];
            if (c != '\\') { out.push_back(c); continue; }

            if (i + 1 >= in.size()) return false;
            char esc = in[++i];
            switch (esc)
            {
            case '\"': out.push_back('\"'); break;
            case '\\': out.push_back('\\'); break;
            case '/':  out.push_back('/');  break;
            case 'b':  out.push_back('\b'); break;
            case 'f':  out.push_back('\f'); break;
            case 'n':  out.push_back('\n'); break;
            case 'r':  out.push_back('\r'); break;
            case 't':  out.push_back('\t'); break;
            case 'u':
            {
                // Basic \uXXXX -> UTF-8 (BMP only, no surrogate pairs)
                if (i + 4 >= in.size()) return false;
                int d0 = HexVal(in[i + 1]);
                int d1 = HexVal(in[i + 2]);
                int d2 = HexVal(in[i + 3]);
                int d3 = HexVal(in[i + 4]);
                if (d0 < 0 || d1 < 0 || d2 < 0 || d3 < 0) return false;
                i += 4;
                unsigned code = (d0 << 12) | (d1 << 8) | (d2 << 4) | d3;

                if (code <= 0x7F) out.push_back(static_cast<char>(code));
                else if (code <= 0x7FF)
                {
                    out.push_back(static_cast<char>(0xC0 | ((code >> 6) & 0x1F)));
                    out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
                }
                else
                {
                    out.push_back(static_cast<char>(0xE0 | ((code >> 12) & 0x0F)));
                    out.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
                    out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
                }
                break;
            }
            default:
                return false;
            }
        }
        return true;
    }

    bool JSONUtils::ParseJSONString(const std::string& s, size_t& pos, std::string& outStr)
    {
        if (pos >= s.size() || s[pos] != '\"') return false;
        ++pos; // skip opening quote

        std::string raw;
        while (pos < s.size())
        {
            char c = s[pos++];
            if (c == '\"') break;        // end
            raw.push_back(c);
            if (c == '\\' && pos < s.size()) // ensure we keep escape pair together
            {
                raw.push_back(s[pos++]);
            }
        }
        std::string un;
        if (!Unescape(raw, un)) return false;
        outStr = std::move(un);
        return true;
    }

    // --- stringify -----------------------------------------------------------
    std::string JSONUtils::ToJSONString(
        const std::map<std::string, std::string>& obj,
        bool pretty,
        int indent)
    {
        std::ostringstream oss;
        const std::string nl = pretty ? "\n" : "";
        const std::string sp = pretty ? std::string(indent, ' ') : "";
        const std::string sep = pretty ? ",\n" : ",";

        oss << "{" << nl;
        size_t count = 0, total = obj.size();
        for (const auto& [k, v] : obj)
        {
            ++count;
            oss << (pretty ? sp : "")
                << "\"" << Escape(k) << "\": \"" << Escape(v) << "\"";
            if (count < total) oss << sep;
        }
        if (pretty && !obj.empty()) oss << nl;
        oss << "}";
        return oss.str();
    }

    // --- validators ----------------------------------------------------------
    bool JSONUtils::ValidateBasic(const std::string& json)
    {
        std::string s = json;
        Trim(s);
        if (s.empty()) return false;
        bool ok = s.front() == '{' && s.back() == '}';
        if (!ok)
        {
            LogUtils::Error("JSON ValidateBasic failed: not an object.");
        }
        return ok;
    }

    // --- parsing of simple object -------------------------------------------
    bool JSONUtils::FromSimpleObject(
        const std::string& json,
        std::map<std::string, std::string>& out)
    {
        out.clear();

        std::string s = json;
        Trim(s);
        if (!ValidateBasic(s)) return false;

        size_t i = 0;
        i = SkipWS(s, i);
        if (s[i] != '{') { LogUtils::Error("JSON parse: missing '{'."); return false; }
        ++i;

        while (true)
        {
            i = SkipWS(s, i);
            if (i >= s.size()) { LogUtils::Error("JSON parse: unexpected end."); return false; }
            if (s[i] == '}') { ++i; break; } // end of object (allow empty)

            // key
            std::string key;
            if (!ParseJSONString(s, i, key))
            {
                LogUtils::Error("JSON parse: expected string key.");
                return false;
            }

            i = SkipWS(s, i);
            if (i >= s.size() || s[i] != ':')
            {
                LogUtils::Error("JSON parse: missing ':' after key '" + key + "'.");
                return false;
            }
            ++i;

            i = SkipWS(s, i);

            // only supports string values for simplicity
            if (i >= s.size() || s[i] != '\"')
            {
                LogUtils::Error("JSON parse: only string values are supported for key '" + key + "'.");
                return false;
            }

            std::string val;
            if (!ParseJSONString(s, i, val))
            {
                LogUtils::Error("JSON parse: invalid string for key '" + key + "'.");
                return false;
            }

            out[key] = val;

            i = SkipWS(s, i);
            if (i >= s.size())
            {
                LogUtils::Error("JSON parse: unexpected end after value.");
                return false;
            }

            if (s[i] == ',') { ++i; continue; }
            if (s[i] == '}') { ++i; break; }

            LogUtils::Error("JSON parse: expected ',' or '}' after key '" + key + "'.");
            return false;
        }

        return true;
    }

    std::optional<std::string> JSONUtils::GetString(
        const std::string& json,
        const std::string& key)
    {
        std::map<std::string, std::string> tmp;
        if (!FromSimpleObject(json, tmp)) return std::nullopt;

        auto it = tmp.find(key);
        if (it == tmp.end()) return std::nullopt;
        return it->second;
    }

    // --- file I/O ------------------------------------------------------------
    bool JSONUtils::WriteObjectToFile(
        const std::string& path,
        const std::map<std::string, std::string>& obj,
        bool pretty)
    {
        std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
        if (!ofs)
        {
            LogUtils::Error("JSON WriteObjectToFile: failed to open '" + path + "' for write.");
            return false;
        }
        auto text = ToJSONString(obj, pretty);
        ofs.write(text.data(), static_cast<std::streamsize>(text.size()));
        if (!ofs.good())
        {
            LogUtils::Error("JSON WriteObjectToFile: write failed for '" + path + "'.");
            return false;
        }
        LogUtils::Info("JSON Write: " + path + " (" + std::to_string(text.size()) + " bytes)");
        return true;
    }

    bool JSONUtils::ReadObjectFromFile(
        const std::string& path,
        std::map<std::string, std::string>& out)
    {
        std::ifstream ifs(path, std::ios::binary);
        if (!ifs)
        {
            LogUtils::Error("JSON ReadObjectFromFile: failed to open '" + path + "'.");
            return false;
        }
        std::ostringstream oss;
        oss << ifs.rdbuf();
        auto text = oss.str();
        if (text.empty())
        {
            LogUtils::Warning("JSON Read: '" + path + "' is empty.");
            out.clear();
            return true; // treat empty as empty object
        }

        if (!FromSimpleObject(text, out))
        {
            LogUtils::Error("JSON ReadObjectFromFile: parse failed for '" + path + "'.");
            return false;
        }
        LogUtils::Info("JSON Read: " + path + " (ok)");
        return true;
    }
}
