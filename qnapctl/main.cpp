#include "qnapctl_dbus.h"
#include <QCoreApplication>
#include <QDBusConnection>

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  QNAPCtlInterface *ctrl = new QNAPCtlInterface(
      "eu.zopi.QNAPCtl", "/eu/zopi/qnapctrl", QDBusConnection::systemBus());

  QCoreApplication::connect(ctrl, &QNAPCtlInterface::buttonEvent,
                            [ctrl](const QString &button, bool pressed) {
                              if (button == "USB_COPY") {
                                ctrl->setLED("USB", pressed);
                                ctrl->setLED("STATUS_GREEN", !pressed);
                              }
                            });

  return app.exec();
}
