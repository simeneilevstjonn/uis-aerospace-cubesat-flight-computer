target_sources(CubeSat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/accelerometer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gyroscope.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thermometer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/barometer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss.cpp
)