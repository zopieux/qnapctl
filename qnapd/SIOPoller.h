#pragma once

#include <QObject>
#include <QThread>

#include "QNAPCtrl.h"

class SIOPoller : public QThread {
  Q_OBJECT

public:
  explicit SIOPoller(QObject *parent = nullptr) : QThread(parent) {}

signals:
  void buttonEvent(QNAPCtrl::PanelButton button, bool pressed);

public slots:
  void setLed(QNAPCtrl::PanelLED led, bool on);

protected:
  void run() override;

protected slots:
  void poll();

private:
  bool usb_copy_pressed_ = false;
  QMap<QNAPCtrl::PanelLED, bool> intended_state_;
};
