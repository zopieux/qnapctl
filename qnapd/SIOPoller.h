#pragma once

#include <QMap>
#include <QObject>
#include <QThread>

#include "Daemon.h"

class SIOPoller : public QThread {
  Q_OBJECT

 public:
  explicit SIOPoller(QObject *parent = nullptr) : QThread(parent) {}

 signals:
  void buttonEvent(Daemon::PanelButton button, bool pressed);

 public slots:
  void setLed(Daemon::PanelLED led, bool on);

 protected:
  void run() override;

 protected slots:
  void poll();

 private:
  bool usb_copy_pressed_ = false;
  QMap<Daemon::PanelLED, bool> intended_state_;
};
