#pragma once
#include <numbers>

#include <string>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>
#include <algorithm>
#include <vector>
//#include "raymath.h"


namespace Utils
{
    // Clamp value between min and max
    template<typename T>
    inline T Clamp(T value, T minVal, T maxVal) {
        if (value < minVal) return minVal;
        if (value > maxVal) return maxVal;
        return value;
    }

    // Linear interpolation between a and b by t (0.0f - 1.0f)
    template<typename T>
    inline T Lerp(T a, T b, float t) {
        return static_cast<T>(a + (b - a) * t);
    }

    // Degrees to radians
    inline float ToRadians(float degrees) {
        return degrees * std::numbers::pi_v<float> / 180.0f;
    }

    // Radians to degrees
    inline float ToDegrees(float radians) {
        return radians * 180.0f / std::numbers::pi_v<float>;
    }

    // Check if file exists
    inline bool FileExists(const std::string& path) {
        std::filesystem::path p(path);
        return std::filesystem::exists(p) && std::filesystem::is_regular_file(p);
    }

    // Read entire file content into a string
    inline std::string ReadFileText(const std::string& path) {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file) {
            throw std::runtime_error("Unable to open file: " + path);
        }
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    inline std::string ReadFileToString(const std::string& path) {
        return ReadFileText(path);
    }
    // Get current time as formatted string YYYY-MM-DD HH:MM:SS
    inline std::string GetCurrentTimeString() {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
#if defined(_WIN32)
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    // Logging functions
    inline void LogInfo(const std::string& msg) {
        std::cout << "[INFO] [" << GetCurrentTimeString() << "] " << msg << std::endl;
    }

    inline void LogWarn(const std::string& msg) {
        std::cout << "[WARN] [" << GetCurrentTimeString() << "] " << msg << std::endl;
    }

    inline void LogError(const std::string& msg) {
        std::cerr << "[ERROR] [" << GetCurrentTimeString() << "] " << msg << std::endl;
    }

    // Random number generator utilities
    class Random {
    public:
        static void Seed(unsigned int seed) {
            GetEngine().seed(seed);
        }

        static int Int(int minVal, int maxVal) {
            std::uniform_int_distribution<int> dist(minVal, maxVal);
            return dist(GetEngine());
        }

        static float Float(float minVal, float maxVal) {
            std::uniform_real_distribution<float> dist(minVal, maxVal);
            return dist(GetEngine());
        }

    private:
        static std::mt19937& GetEngine() {
            static std::random_device rd;
            static std::mt19937 engine(rd());
            return engine;
        }
    };

    // Convenience wrappers
    inline int RandomInt(int minVal, int maxVal) {
        return Random::Int(minVal, maxVal);
    }

    inline float RandomFloat(float minVal, float maxVal) {
        return Random::Float(minVal, maxVal);
    }

    // String utilities
    inline std::string ToUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    inline std::string ToLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    inline std::string Trim(const std::string& str) {
        const std::string whitespace = "\t\n\r ";
        const auto strBegin = str.find_first_not_of(whitespace);
        if (strBegin == std::string::npos) return "";
        const auto strEnd = str.find_last_not_of(whitespace);
        const auto strRange = strEnd - strBegin + 1;
        return str.substr(strBegin, strRange);
    }

    inline std::vector<std::string> Split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::istringstream iss(str);
        std::string token;
        while (std::getline(iss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    // Path joining utility
    inline std::string JoinPath(const std::string& a, const std::string& b) {
        std::filesystem::path pa(a);
        pa /= b;
        return pa.string();
    }

    template<typename ... Args>
    inline std::string JoinPath(const std::string& first, const Args& ... rest) {
        std::filesystem::path p(first);
        (void)std::initializer_list<int>{(p /= rest, 0)...};
        return p.string();
    }


    //inline Vector3 Vector3Subtract(Vector3 one, Vector3 other)
    //{
    //    return { one.x - other.x , one.y - other.y, one.z - other.z};
    //}

}

