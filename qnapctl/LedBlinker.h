#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

#include "qnapctl_dbus.h"

class LedBlinker : public QObject {
  Q_OBJECT

 public:
  explicit LedBlinker(QNAPCtlInterface* ctrl, QString led,
                      QObject* parent = nullptr);

 public slots:
  void setInterval(int msec);

 private:
  QNAPCtlInterface* ctrl_;
  const QString led_;
  int next_interval_;

  // Used to switch the led ON.
  QTimer* timer_on_;
  // Used to switch the led OFF.
  QTimer* timer_off_;
};
