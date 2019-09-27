cmake_minimum_required(VERSION 3.10)
project(qnapctl)

find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Network REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Generate an adaptor C++ class from the XML service definition.
set(interface_xml ${qnapd_SOURCE_DIR}/eu.zopi.qnapctl.xml)
set_source_files_properties(${interface_xml} PROPERTIES CLASSNAME QNAPCtlInterface)
set_source_files_properties(${interface_xml} PROPERTIES NO_NAMESPACE true)
qt5_add_dbus_interface(
        qnapctl_interface_SRCS
        ${interface_xml}
        qnapctl_dbus
)

# Daemon daemon (qnapd)
set(
        qnapctl_SRCS
        ${qnapctl_interface_SRCS}
        main.cpp
        Controller.cpp
        Controller.h
        LedBlinker.cpp
        LedBlinker.h
)

add_executable(qnapctl ${qnapctl_SRCS})

target_link_libraries(qnapctl Qt5::Core)
target_link_libraries(qnapctl Qt5::DBus)
target_link_libraries(qnapctl Qt5::Network)

install(TARGETS qnapctl)
