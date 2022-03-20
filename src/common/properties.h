#ifndef _ROBOT_PROPERTIES_H_
#define _ROBOT_PROPERTIES_H_

#include <cinttypes>
#include <string>
#include <variant>
#include <unordered_map>

namespace Robot {

    using PropertyValue = std::variant<std::string, bool, float, int>;
    
    class PropertyMap : public std::unordered_map<std::string, PropertyValue> {
        public:
            using std::unordered_map<std::string, PropertyValue>::unordered_map;

            template<typename T>
            T get(const std::string &key, T defaultValue) const
            {
                auto iter = find(key);
                if (iter==end()) {
                    return defaultValue;
                }
                return std::get<T>(iter->second);
            }

            template<typename T>
            T get(const std::string &key) const
            {
                return std::get<T>(at(key));
            }

            template<typename T>
            T get(const char *key) const
            {
                return std::get<T>(at(key));
            }

            template<typename T>
            void put(const std::string &key, T value)
            {
                (*this)[key] = value;
            }

            template<typename T>
            void put(const char *key, T value)
            {
                (*this)[key] = value;
            }
    };
    using Properties = std::unordered_map<std::string, PropertyMap>;

}

#endif