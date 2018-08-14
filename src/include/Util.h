#ifndef NORENGINE_UTIL_H
#define NORENGINE_UTIL_H


#include <chrono>
#include <cstdio>
#include <cmath>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <SDL_pixels.h>
#include "Definitions.h"

struct Entity;
struct TransformComponent;
struct BodyComponent;

struct Util {
    static std::chrono::high_resolution_clock::time_point initTime;

    static float getTimeStamp();

    static float getBearing(float x1, float y1, float x2, float y2);

    template<typename T>
    static std::string toString(const T &n) {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }

    static float toFloat(std::string str) {
        return strtof(str.c_str(), 0);
    };

    static void addDebugEntities();

    static bool isIntersectRect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

    static SDL_Color getSDLColor(std::string color);

    static int getTag(const char *tag);

    static std::vector<std::string> splitString(const char *str, const char *delimiter);
};


#endif //NORENGINE_UTIL_H
