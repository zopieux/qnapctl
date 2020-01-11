#include <QDBusConnection>
#include <QSerialPortInfo>
#include <QtCore>
#include <iostream>

#include "Config.h"
#include "Daemon.h"
#include "LCD.h"
#include "dbus_qnapctl_adaptor.h"

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  auto systemBus = QDBusConnection::systemBus();

  auto *ctrl = new Daemon;
  new QNAPCtlInterfaceAdaptor(ctrl);

  if (!systemBus.registerObject(DBUS_PATH, ctrl)) {
    std::cerr << "Could not register Daemon object: "
              << qPrintable(systemBus.lastError().message()) << "\n";
    exit(1);
  }

  if (!systemBus.registerService(DBUS_SERVICE)) {
    std::cerr << "Could not register service " << DBUS_SERVICE << ": "
              << qPrintable(systemBus.lastError().message()) << "\n";
    exit(1);
  }

  std::cerr << "Ready.\n"
            << "  Interface: " << DBUS_INTERFACE << "\n"
            << "    Service: " << DBUS_SERVICE << "\n"
            << "       Path: " << DBUS_PATH << "\n";

  return app.exec();
}
