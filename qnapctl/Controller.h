#pragma once

#include <QDBusConnection>
#include <QTimer>

#include "LedBlinker.h"
#include "qnapctl_dbus.h"

class Controller : public QObject {
  Q_OBJECT

 public:
  explicit Controller(QObject* parent = nullptr);

 private slots:
  void goToSleep();
  void pollStatus();
  void onButtonEvent(const QString& button, bool pressed);

 private:
  enum class State { SLEEP, STATUS, NETWORK } state_;

  QNAPCtlInterface* ctrl_;
  QTimer* sleep_timer_;
  QTimer* poll_status_timer_;

  LedBlinker* led_green_;
  LedBlinker* led_red_;

  double load_average_;
  size_t services_running_, services_failed_;

  void resetSleepTimer();
};
