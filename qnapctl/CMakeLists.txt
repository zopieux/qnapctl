cmake_minimum_required(VERSION 3.10)
project(qnapctl)

find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Network REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Generate an adaptor C++ class from the XML service definition.
set_source_files_properties(${dbus_xml_file} PROPERTIES CLASSNAME QNAPCtlInterface)
set_source_files_properties(${dbus_xml_file} PROPERTIES NO_NAMESPACE true)
qt5_add_dbus_interface(
        qnapctl_interface_SRCS
        ${dbus_xml_file}
        qnapctl_dbus
)

# Daemon daemon (qnapd)
set(
        qnapctl_SRCS
        ${qnapctl_interface_SRCS}
        ${config_file}
        main.cpp
        Controller.cpp
        Controller.h
        LedBlinker.cpp
        LedBlinker.h
)

add_executable(qnapctl ${qnapctl_SRCS})
target_include_directories(qnapctl PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(qnapctl Qt5::Core)
target_link_libraries(qnapctl Qt5::DBus)
target_link_libraries(qnapctl Qt5::Network)

install(TARGETS qnapctl RUNTIME DESTINATION bin)
