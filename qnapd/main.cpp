#include <iostream>

#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>
#include <QSerialPortInfo>
#include <QtCore>

#include "LCD.h"
#include "QNAPCtrl.h"
#include "qnapctrladaptor.h"

constexpr char kServiceName[] = "eu.zopi.QNAPCtrl";
constexpr char kServicePath[] = "/eu/zopi/qnapctrl";

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  auto systemBus = QDBusConnection::systemBus();

  auto *ctrl = new QNAPCtrl;
  new QNAPCtrlInterfaceAdaptor(ctrl);

  if (!systemBus.registerObject(kServicePath, ctrl)) {
    std::cerr << "Could not register QNAPCtrl object: "
              << qPrintable(systemBus.lastError().message()) << "\n";
    exit(1);
  }

  if (!systemBus.registerService(kServiceName)) {
    std::cerr << "Could not register service " << kServiceName << ": "
              << qPrintable(systemBus.lastError().message()) << "\n";
    exit(1);
  }

  std::cerr << "Ready.\n"
               "  Interface: eu.zopi.QNAPCtrlInterface\n    Service: "
            << kServiceName << "\n       Path: " << kServicePath << "\n";

  return app.exec();
}
