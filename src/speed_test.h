#ifndef SPEED_TEST_H
#define SPEED_TEST_H

#include <iostream>
#include <chrono>

const int waitingTimeBeforeDisablingReadFromDeviceS = 2; /*! 2s */
const int waitingTimeBeforeDisablingParseDataS = 2; /*! 2s */
const std::vector <std::string> typeName = {"Read from device", "Parse data", "Get next message"};

typedef enum SpeedTestTypes {
    SpeedTestReadFromDevice,
    SpeedTestParseData,
    SpeedTestGetNextMessage,
    SpeedTestTypesNum
} SpeedTestTypes_t;

/*! decomment only one of the following options */
// #define SPT_LOG_EVERYTHING
// #define SPT_READ_FROM_DEVICE_MAX_SPEED
// #define SPT_PARSE_DATA_MAX_SPEED
// #define SPT_GET_NEXT_MESSAGE_MAX_SPEED
// #define SPT_READ_FROM_DEVICE_STUBBED
// #define SPT_PARSE_DATA_STUBBED
// #define SPT_GET_NEXT_MESSAGE_STUBBED

/*! DO NOT edit the code below */
#ifdef SPT_LOG_EVERYTHING

#define SPT_LOG_READ_FROM_DEVICE
#define SPT_LOG_PARSE_DATA
#define SPT_LOG_GET_NEXT_MESSAGE

#endif

#ifdef SPT_READ_FROM_DEVICE_MAX_SPEED

#define SPT_DISABLE_PARSE_DATA_AFTER_A_WHILE
#define SPT_DISABLE_GET_NEXT_MESSAGE
#define SPT_LOG_READ_FROM_DEVICE

#endif

#ifdef SPT_PARSE_DATA_MAX_SPEED

#define SPT_DISABLE_READ_FROM_DEVICE_AFTER_A_WHILE
#define SPT_DISABLE_GET_NEXT_MESSAGE
#define SPT_LOG_PARSE_DATA

#endif

#ifdef SPT_GET_NEXT_MESSAGE_MAX_SPEED

#define SPT_DISABLE_READ_FROM_DEVICE_AFTER_A_WHILE
#define SPT_DISABLE_PARSE_DATA_AFTER_A_WHILE
#define SPT_LOG_GET_NEXT_MESSAGE

#endif

#ifdef SPT_READ_FROM_DEVICE_STUBBED

#define SPT_DISABLE_READ_FROM_DEVICE_AFTER_A_WHILE
#define SPT_LOG_PARSE_DATA
#define SPT_LOG_GET_NEXT_MESSAGE

#endif

#ifdef SPT_PARSE_DATA_STUBBED

#define SPT_DISABLE_PARSE_DATA_AFTER_A_WHILE
#define SPT_LOG_READ_FROM_DEVICE
#define SPT_LOG_GET_NEXT_MESSAGE

#endif

#ifdef SPT_GET_NEXT_MESSAGE_STUBBED

#define SPT_DISABLE_GET_NEXT_MESSAGE
#define SPT_LOG_READ_FROM_DEVICE
#define SPT_LOG_PARSE_DATA

#endif

inline void speedTestLog(SpeedTestTypes_t type, int newData) {
    static std::vector <bool> initialTimeTook(3, false);
    static std::vector <std::chrono::steady_clock::time_point> initialTime(3);
    static std::vector <int64_t> accData(3, 0);
    if (!initialTimeTook[type]) {
        initialTime[type] = std::chrono::steady_clock::now();
        initialTimeTook[type] = true;
    }
    auto thisTime = std::chrono::steady_clock::now();
    auto t = std::chrono::duration_cast <std::chrono::milliseconds> (thisTime - initialTime[type]).count();
    auto tmin = t;
    for (int i = 0; i < SpeedTestTypesNum; i++) {
        auto newt = std::chrono::duration_cast <std::chrono::milliseconds> (thisTime - initialTime[i]).count();
        tmin = newt < tmin ? newt : tmin;
    }

    accData[type] += newData;

    if (t > 2000 && tmin > 300) {
        std::cout << typeName[type] << " " << accData[type] / t / 1000 << " MBps" << std::endl;
        initialTime[type] = thisTime;
        accData[type] = 0;
    }
}

#endif // SPEED_TEST_H
