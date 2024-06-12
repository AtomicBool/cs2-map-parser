#ifndef KV3_PARSER_HPP
#define KV3_PARSER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <variant>
#include <optional>

class c_kv3_parser
{
public:
    struct value_struct_t;

    using object_t = std::map<std::string, value_struct_t>;
    using array_t = std::vector<value_struct_t>;
    using value_t = std::variant<std::string, object_t, array_t>;

    struct value_struct_t
    {
        value_t value;
    };

public:
    void parse(const std::string &content)
    {
        this->content = content;
        this->index = 0;

        skip_comments_and_metadata();
        parsed_data = parse_value().value;
    }

    ~c_kv3_parser() {
        this->content = "";
        this->index = 0;
        this->parsed_data = value_t();
    }

    std::string get_value(const std::string &path) const
    {
        const value_t *current_value = &parsed_data;
        std::stringstream ss(path);
        std::string segment;

        while (std::getline(ss, segment, '.'))
        {
            std::string key = segment;
            std::optional<size_t> array_index;

            size_t bracket_pos = segment.find('[');
            if (bracket_pos != std::string::npos)
            {
                key = segment.substr(0, bracket_pos);
                size_t end_bracket_pos = segment.find(']', bracket_pos);
                array_index = std::stoi(segment.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1));
            }

            if (std::holds_alternative<object_t>(*current_value))
            {
                const object_t &obj = std::get<object_t>(*current_value);
                auto it = obj.find(key);
                if (it != obj.end())
                {
                    current_value = &(it->second.value);
                }
            }

            if (array_index.has_value())
            {
                if (std::holds_alternative<array_t>(*current_value))
                {
                    const array_t &arr = std::get<array_t>(*current_value);
                    if (array_index.value() < arr.size())
                    {
                        current_value = &(arr[array_index.value()].value);
                    }
                }
            }
        }

        if (std::holds_alternative<std::string>(*current_value))
        {
            return std::get<std::string>(*current_value);
        }

        return "";
    }

    std::vector<std::string> find_key_paths_with_key_name(const std::string &search_key) const
    {
        std::vector<std::string> paths;
        find_key_paths_with_key_name(parsed_data, search_key, paths);
        return paths;
    }

private:
    void find_key_paths_with_key_name(const value_t &root, const std::string &search_key, std::vector<std::string> &paths, const std::string &current_path = "") const
    {
        if (std::holds_alternative<object_t>(root))
        {
            const object_t &obj = std::get<object_t>(root);
            for (const auto &[key, val] : obj)
            {
                std::string new_path = current_path.empty() ? key : current_path + "." + key;
                if (key == search_key)
                {
                    paths.push_back(new_path);
                }
                find_key_paths_with_key_name(val.value, search_key, paths, new_path);
            }
        }
        else if (std::holds_alternative<array_t>(root))
        {
            const array_t &arr = std::get<array_t>(root);
            size_t index = 0;
            for (const auto &val : arr)
            {
                find_key_paths_with_key_name(val.value, search_key, paths, current_path + "[" + std::to_string(index++) + "]");
            }
        }
    }

    void skip_comments_and_metadata()
    {
        while (index < content.size() && content[index] != '{')
        {
            index = content.find('\n', index) + 1;
        }
    }

    void skip_whitespace()
    {
        while (index < content.size() && std::isspace(content[index]))
        {
            ++index;
        }
    }

    void skip_comments()
    {
        while (index < content.size() && content[index] == '/')
        {
            index = content.find('\n', index) + 1;
        }
    }

    size_t get_key_or_value_end()
    {
        const std::string delimiters = "= \n { [ } ] ,";
        size_t end = content.find_first_of(delimiters, index);
        if (end == std::string::npos)
        {
            end = content.size();
        }
        return end;
    }

    value_struct_t parse_value()
    {
        skip_comments();
        skip_whitespace();

        if (content[index] == '{')
        {
            return {parse_object()};
        }
        else if (content[index] == '[')
        {
            return {parse_array()};
        }
        else if (content[index] == '#' && (index + 1) < content.size() && content[index + 1] == '[')
        {
            ++index;
            return parse_byte_array();
        }
        else
        {
            size_t valueStart = index;
            size_t valueEnd = get_key_or_value_end();
            index = valueEnd;
            return {content.substr(valueStart, valueEnd - valueStart)};
        }
    }

    value_struct_t parse_byte_array()
    {
        skip_whitespace();
        ++index;

        size_t valueStart = index;
        size_t valueEnd = content.find(']', index);
        std::string raw_byte_data = content.substr(valueStart, valueEnd - valueStart);
        std::stringstream ss(raw_byte_data);
        std::string byte;
        std::vector<std::string> bytes;

        while (ss >> byte)
        {
            bytes.push_back(byte);
        }

        std::string cleaned_byte_data = join(bytes, " ");

        index = valueEnd + 1;
        return {cleaned_byte_data};
    }

    value_struct_t parse_object()
    {
        object_t obj;
        skip_whitespace();
        ++index;

        while (index < content.size())
        {
            skip_comments();
            skip_whitespace();

            if (content[index] == '}')
            {
                ++index;
                return {obj};
            }

            size_t keyStart = index;
            size_t keyEnd = get_key_or_value_end();
            std::string key = content.substr(keyStart, keyEnd - keyStart);
            index = keyEnd;

            skip_whitespace();
            if (content[index] == '=')
            {
                ++index;
            }

            value_struct_t value = parse_value();
            obj[key] = value;

            skip_whitespace();
            if (content[index] == ',')
            {
                ++index;
            }
        }

        return {};
    }

    value_struct_t parse_array()
    {
        array_t arr;
        skip_whitespace();
        ++index;

        while (index < content.size())
        {
            skip_comments();
            skip_whitespace();

            if (content[index] == ']')
            {
                ++index;
                return {arr};
            }

            value_struct_t value = parse_value();
            arr.push_back(value);

            skip_whitespace();
            if (content[index] == ',')
            {
                ++index;
            }
        }

        return {};
    }

    std::string join(const std::vector<std::string> &vec, const std::string &delim)
    {
        std::ostringstream result;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            result << vec[i];
            if (i < vec.size() - 1)
                result << delim;
        }
        return result.str();
    }

private:
    std::string content;
    size_t index;
    value_t parsed_data;
};

#endif
