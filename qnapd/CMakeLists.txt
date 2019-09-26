cmake_minimum_required(VERSION 3.10)
project(qnapd)

find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5SerialPort REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Generate an adaptor C++ class from the XML service definition.
set(interface_xml eu.zopi.qnapctl.xml)
set_source_files_properties(${interface_xml} PROPERTIES CLASSNAME QNAPCtlAdaptor)
set_source_files_properties(${interface_xml} PROPERTIES NO_NAMESPACE true)
qt5_add_dbus_adaptor(
  qnapd_adaptor_SRCS
  ${interface_xml}
  QNAPCtl.h
  QNAPCtl
)

# QNAPCtl daemon (qnapd)
set(
  qnapd_SRCS
  ${qnapd_adaptor_SRCS}
  QNAPCtl.cpp
  QNAPCtl.h
  LCD.cpp
  LCD.h
  SIOPoller.cpp
  SIOPoller.h
  main.cpp
)

add_executable(qnapd ${qnapd_SRCS})

target_link_libraries(qnapd Qt5::Core)
target_link_libraries(qnapd Qt5::DBus)
target_link_libraries(qnapd Qt5::SerialPort)

install(TARGETS qnapd)

# Policy file.
install(
  FILES eu.zopi.QNAPCtl.conf
  DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/dbus-1/system.d
)
