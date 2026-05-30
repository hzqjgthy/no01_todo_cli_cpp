#include "todo/json_mini.hpp"

#include <cctype>
#include <cstdint>
#include <sstream>
#include <vector>

namespace todo::json_mini {

// =====================================================================
// 序列化
// =====================================================================

namespace {

void write_escaped(std::ostringstream& os, const std::string& s) {
    os << '"';
    for (char c : s) {
        switch (c) {
            case '"':  os << "\\\""; break;
            case '\\': os << "\\\\"; break;
            case '\n': os << "\\n";  break;
            case '\r': os << "\\r";  break;
            case '\t': os << "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[8];
                    std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                    os << buf;
                } else {
                    os << c;
                }
        }
    }
    os << '"';
}

}  // namespace

std::string dump(const TaskList& list) {
    std::ostringstream os;
    os << "{\n";
    os << "  \"next_id\": " << list.next_id() << ",\n";
    os << "  \"tasks\": [";

    const auto& tasks = list.tasks();
    for (size_t i = 0; i < tasks.size(); ++i) {
        const auto& t = tasks[i];
        os << (i == 0 ? "\n" : ",\n");
        os << "    {\n";
        os << "      \"id\": " << t.id << ",\n";
        os << "      \"title\": ";
        write_escaped(os, t.title);
        os << ",\n";
        os << "      \"done\": " << (t.done ? "true" : "false") << ",\n";
        os << "      \"created_at\": " << t.created_at << "\n";
        os << "    }";
    }
    if (!tasks.empty()) os << "\n  ";
    os << "]\n";
    os << "}\n";
    return os.str();
}

// =====================================================================
// 反序列化（极简递归下降，只支持本项目结构）
// =====================================================================

namespace {

struct Parser {
    const std::string& s;
    size_t             pos = 0;

    void skip_ws() {
        while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) ++pos;
    }

    [[noreturn]] void err(const std::string& msg) {
        throw ParseError("json parse error at " + std::to_string(pos) + ": " + msg);
    }

    void expect(char c) {
        skip_ws();
        if (pos >= s.size() || s[pos] != c) err(std::string("expect '") + c + "'");
        ++pos;
    }

    bool consume(char c) {
        skip_ws();
        if (pos < s.size() && s[pos] == c) { ++pos; return true; }
        return false;
    }

    std::string parse_string() {
        skip_ws();
        if (pos >= s.size() || s[pos] != '"') err("expect string");
        ++pos;
        std::string out;
        while (pos < s.size() && s[pos] != '"') {
            char c = s[pos++];
            if (c == '\\' && pos < s.size()) {
                char esc = s[pos++];
                switch (esc) {
                    case '"':  out += '"';  break;
                    case '\\': out += '\\'; break;
                    case '/':  out += '/';  break;
                    case 'n':  out += '\n'; break;
                    case 'r':  out += '\r'; break;
                    case 't':  out += '\t'; break;
                    case 'u': {
                        if (pos + 4 > s.size()) err("bad \\u");
                        // 简化：只支持 ASCII 范围内的转义
                        std::string hex = s.substr(pos, 4);
                        pos += 4;
                        int code = std::stoi(hex, nullptr, 16);
                        if (code < 0x80) out += static_cast<char>(code);
                        else err("non-ascii \\u not supported in mini parser");
                        break;
                    }
                    default: err("bad escape");
                }
            } else {
                out += c;
            }
        }
        if (pos >= s.size()) err("unterminated string");
        ++pos;  // 吃掉结尾 "
        return out;
    }

    std::int64_t parse_int() {
        skip_ws();
        size_t start = pos;
        if (pos < s.size() && (s[pos] == '-' || s[pos] == '+')) ++pos;
        while (pos < s.size() && std::isdigit(static_cast<unsigned char>(s[pos]))) ++pos;
        if (start == pos) err("expect int");
        return std::stoll(s.substr(start, pos - start));
    }

    bool parse_bool() {
        skip_ws();
        if (s.compare(pos, 4, "true") == 0)  { pos += 4; return true;  }
        if (s.compare(pos, 5, "false") == 0) { pos += 5; return false; }
        err("expect bool");
    }
};

}  // namespace

void parse(const std::string& text, TaskList& list) {
    Parser p{text};
    p.expect('{');

    int               next_id = 1;
    std::vector<Task> tasks;

    bool first = true;
    while (true) {
        p.skip_ws();
        if (p.consume('}')) break;
        if (!first) p.expect(',');
        first = false;

        std::string key = p.parse_string();
        p.expect(':');

        if (key == "next_id") {
            next_id = static_cast<int>(p.parse_int());
        } else if (key == "tasks") {
            p.expect('[');
            bool first_task = true;
            while (true) {
                p.skip_ws();
                if (p.consume(']')) break;
                if (!first_task) p.expect(',');
                first_task = false;

                p.expect('{');
                Task t;
                bool first_field = true;
                while (true) {
                    p.skip_ws();
                    if (p.consume('}')) break;
                    if (!first_field) p.expect(',');
                    first_field = false;

                    std::string fk = p.parse_string();
                    p.expect(':');
                    if      (fk == "id")         t.id         = static_cast<int>(p.parse_int());
                    else if (fk == "title")      t.title      = p.parse_string();
                    else if (fk == "done")       t.done       = p.parse_bool();
                    else if (fk == "created_at") t.created_at = p.parse_int();
                    else                         p.err("unknown field: " + fk);
                }
                tasks.push_back(std::move(t));
            }
        } else {
            p.err("unknown key: " + key);
        }
    }

    list.set_state(next_id, std::move(tasks));
}

}  // namespace todo::json_mini
