#include "qnapctl_dbus.h"
#include <QCoreApplication>
#include <QDBusConnection>

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  QNAPCtrlInterface *ctrl = new QNAPCtrlInterface(
      "eu.zopi.QNAPCtrl", "/eu/zopi/qnapctrl", QDBusConnection::systemBus());

  QCoreApplication::connect(ctrl, &QNAPCtrlInterface::buttonEvent,
                            [ctrl](const QString &button, bool pressed) {
                              if (button == "USB_COPY") {
                                ctrl->setLED("USB", pressed);
                                ctrl->setLED("STATUS_GREEN", !pressed);
                              }
                            });

  return app.exec();
}