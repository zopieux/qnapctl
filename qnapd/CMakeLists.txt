cmake_minimum_required(VERSION 3.10)
project(qnapd)

find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5SerialPort REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Generate an adaptor C++ class from the XML service definition.
set_source_files_properties(${dbus_xml_file} PROPERTIES CLASSNAME QNAPCtlAdaptor)
set_source_files_properties(${dbus_xml_file} PROPERTIES NO_NAMESPACE true)
qt5_add_dbus_adaptor(
        qnapd_adaptor_SRCS
        ${dbus_xml_file}
        Daemon.h
        Daemon
        dbus_qnapctl_adaptor
)

# Daemon daemon (qnapd)
set(
        qnapd_SRCS
        ${qnapd_adaptor_SRCS}
        ${config_file}
        Daemon.cpp
        Daemon.h
        LCD.cpp
        LCD.h
        SIOPoller.cpp
        SIOPoller.h
        main.cpp
)

add_executable(qnapd ${qnapd_SRCS})
target_include_directories(qnapd PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(qnapd Qt5::Core)
target_link_libraries(qnapd Qt5::DBus)
target_link_libraries(qnapd Qt5::SerialPort)

install(TARGETS qnapd RUNTIME)

# Policy file.
set(dbus_policy ${dbus_service}.conf)
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/dbus_policy.conf.in
        ${CMAKE_CURRENT_BINARY_DIR}/${dbus_policy}
        @ONLY
)
install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${dbus_policy}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/dbus-1/system.d
)
