cmake_minimum_required(VERSION 3.10)
project(qnapctl)

find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Generate an adaptor C++ class from the XML service definition.
set(interface_xml ${qnapd_SOURCE_DIR}/eu.zopi.qnapctrl.xml)
set_source_files_properties(${interface_xml} PROPERTIES CLASSNAME QNAPCtrlInterface)
set_source_files_properties(${interface_xml} PROPERTIES NO_NAMESPACE true)
qt5_add_dbus_interface(
  qnapctl_interface_SRCS
  ${interface_xml}
  qnapctl_dbus
)

# QNAPCtrl daemon (qnapd)
set(
  qnapctl_SRCS
  ${qnapctl_interface_SRCS}
  main.cpp
)

add_executable(qnapctl ${qnapctl_SRCS})

target_link_libraries(qnapctl Qt5::Core)
target_link_libraries(qnapctl Qt5::DBus)

install(TARGETS qnapctl)
