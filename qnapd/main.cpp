#include <iostream>

#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>
#include <QSerialPortInfo>
#include <QtCore>

#include "Daemon.h"
#include "LCD.h"
#include "qnapctladaptor.h"

constexpr char kServiceName[] = "eu.zopi.Daemon";
constexpr char kServicePath[] = "/eu/zopi/qnapctrl";

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  auto systemBus = QDBusConnection::systemBus();

  auto *ctrl = new Daemon;
  new QNAPCtlInterfaceAdaptor(ctrl);

  if (!systemBus.registerObject(kServicePath, ctrl)) {
    std::cerr << "Could not register Daemon object: "
              << qPrintable(systemBus.lastError().message()) << "\n";
    exit(1);
  }

  if (!systemBus.registerService(kServiceName)) {
    std::cerr << "Could not register service " << kServiceName << ": "
              << qPrintable(systemBus.lastError().message()) << "\n";
    exit(1);
  }

  std::cerr << "Ready.\n"
               "  Interface: eu.zopi.QNAPCtlInterface\n    Service: "
            << kServiceName << "\n       Path: " << kServicePath << "\n";

  return app.exec();
}
