#pragma once

#include <optional>

#include "nlohmann/json.hpp"

template<typename T>
struct JsonOptional {
    public:
    JsonOptional(std::nullopt_t value = std::nullopt) : mValue(value) {}
    JsonOptional(T value) : mValue(value) {}
    JsonOptional(JsonOptional &other) = default;
    JsonOptional(JsonOptional &&other) = default;
    ~JsonOptional() = default;

    JsonOptional &operator=(const JsonOptional &other) = default;
    JsonOptional &operator=(JsonOptional &&other) = default;

    bool operator==(const JsonOptional<T> &other) const {
        // One doesn't contain a value
        if (mValue.has_value() != other.has_value()) {
            return false;
        }

        if (mValue.has_value()) {
            return mValue == other.mValue;
        }

        // Both don't contain any value
        return true;
    }

    bool operator!=(const JsonOptional<T> &other) {
        return !operator==(other);
    }

    bool has_value() const {
        return mValue.has_value();
    }

    operator bool() const {
        return has_value();
    }

    auto operator*() const {
        return *mValue;
    }

    auto operator*() {
        return *mValue;
    }

    T &value() { return *mValue; }

    const T &value() const { return *mValue; }

    T value_or(const T &init) const { 
        if (has_value()) {
            return *mValue;
        }

        return init;
    }

    private:
    std::optional<T> mValue;
};

template<typename T>
bool operator==(const JsonOptional<T> &lhs, const JsonOptional<T> &rhs) {
    return lhs == rhs;
}

template<typename T>
bool operator!=(const JsonOptional<T> &lhs, const JsonOptional<T> &rhs) {
    return lhs == rhs;
}

namespace nlohmann {
template<typename T>
    struct adl_serializer<JsonOptional<T>> {
        static void to_json(json &j, const JsonOptional<T> &opt) {
            if (opt == std::nullopt) {
                j = nullptr;
            } else {
                j = *opt;
            }
        }

        static void from_json(const json &j, JsonOptional<T> &opt) {
            if (j.is_null()) {
                opt = std::nullopt;
            } else {
                opt = j.template get<T>();
            }
        }
    };
}