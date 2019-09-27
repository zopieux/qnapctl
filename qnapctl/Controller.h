#pragma once

#include <QDBusConnection>
#include <QTimer>

#include "qnapctl_dbus.h"

class Controller : public QObject {
  Q_OBJECT

 private:
  enum class Led { STATUS_GREEN, STATUS_RED };

 public:
  explicit Controller(QObject* parent = nullptr);

 private slots:
  void goToSleep();
  void pollStatus();
  void onButtonEvent(const QString& button, bool pressed);
  void blinkLed(Led led);

 private:
  enum class State { SLEEP, STATUS, NETWORK } state_;

  QNAPCtlInterface* ctrl_;
  QTimer* sleep_timer_;
  QTimer* poll_status_timer_;
  QTimer* blink_timer_green_;
  QTimer* blink_timer_red_;
  bool blink_green_;
  bool blink_red_;

  double load_average_;
  size_t services_running_, services_failed_;

  void resetSleepTimer();
};
