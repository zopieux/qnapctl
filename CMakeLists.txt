cmake_minimum_required(VERSION 3.10)
project(qnapctl-and-qnapd)

set(CMAKE_CXX_STANDARD 17)

set(dbus_path /eu/zopi/qnapctl)
set(dbus_service eu.zopi.QNAPCtl)
set(dbus_iface ${dbus_service}Interface)

set(config_file ${CMAKE_CURRENT_BINARY_DIR}/Config.h)
set(dbus_xml_file ${CMAKE_CURRENT_BINARY_DIR}/dbus_interface.xml)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Config.h.in
        ${config_file}
        @ONLY
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/dbus_interface.xml.in
        ${dbus_xml_file}
        @ONLY
)

add_subdirectory(qnapd)
add_subdirectory(qnapctl)
