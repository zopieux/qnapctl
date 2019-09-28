#include <iostream>

#include <QBitArray>
#include <QMetaEnum>
#include <QSet>

#include "Daemon.h"
#include "LCD.h"
#include "SIOPoller.h"

Daemon::Daemon(QObject *parent) : QObject(parent) {
  lcd_ = new LCD(this);
  if (!lcd_->open("/dev/ttyS1")) {
    qWarning() << "Failed to open LCD serial port";
  }
  connect(lcd_, &LCD::buttonEvent, this, &Daemon::emitButtonEvent);

  sio_helper_ = new SIOPoller(this);
  connect(sio_helper_, &SIOPoller::buttonEvent, this, &Daemon::emitButtonEvent);

  // Switch everything off at startup.
  auto &&meta = QMetaEnum::fromType<PanelLED>();
  for (int i = 0; i < meta.keyCount(); ++i) {
    sio_helper_->setLed(static_cast<PanelLED>(meta.value(i)), false);
  }

  sio_helper_->start();
}

void Daemon::writeLCD(const QString &text) {
  const auto &lines = text.split("\n").mid(0, 2);
  int index = 0;
  for (const auto &line : lines) {
    writeLCD(index++, line);
  }
}

void Daemon::writeLCD(int line, const QString &text) {
  if (!lcd_->write(line, text.toLatin1())) {
    qWarning() << "Failed to write to LCD";
  }
}

void Daemon::setLCDBacklight(bool on) {
  if (!lcd_->setBacklight(on)) {
    qWarning() << "Failed to switch LCD backlight";
  }
}

void Daemon::setLED(Daemon::PanelLED led, bool on) {
  sio_helper_->setLed(led, on);
}

void Daemon::setLED(const QString &led_name, bool on) {
  auto &&meta = QMetaEnum::fromType<PanelLED>();
  bool ok;
  const PanelLED led =
      static_cast<const PanelLED>(meta.keyToValue(qPrintable(led_name), &ok));
  if (!ok) return;
  setLED(led, on);
}

void Daemon::emitButtonEvent(Daemon::PanelButton button, bool pressed) {
  emit buttonEvent(QVariant::fromValue(button).toString(), pressed);
}
