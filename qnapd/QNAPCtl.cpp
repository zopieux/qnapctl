#include <iostream>

#include <QBitArray>
#include <QMap>
#include <QMetaEnum>
#include <QSet>
#include <QThread>
#include <QTimer>

#include "LCD.h"
#include "QNAPCtl.h"
#include "SIOPoller.h"

QNAPCtl::QNAPCtl(QObject *parent) : QObject(parent) {
  lcd_ = new LCD(this);
  if (!lcd_->open("/dev/ttyS1")) {
    qWarning() << "Failed to open LCD serial port";
  }
  connect(lcd_, &LCD::buttonEvent, this, &QNAPCtl::emitButtonEvent);

  sio_helper_ = new SIOPoller(this);
  connect(sio_helper_, &SIOPoller::buttonEvent, this,
          &QNAPCtl::emitButtonEvent);

  // Switch everything off at startup.
  auto &&meta = QMetaEnum::fromType<PanelLED>();
  for (int i = 0; i < meta.keyCount(); ++i) {
    sio_helper_->setLed(static_cast<PanelLED>(meta.value(i)), false);
  }

  sio_helper_->start();
}

void QNAPCtl::writeLCD(QString text) {
  const auto &lines = text.split("\n").mid(0, 2);
  int index = 0;
  for (const auto &line : lines) {
    writeLCD(index++, line);
  }
}

void QNAPCtl::writeLCD(int line, QString text) {
  if (!lcd_->write(line, text.toLatin1())) {
    qWarning() << "Failed to write to LCD";
  }
}

void QNAPCtl::setLCDBacklight(bool on) {
  if (!lcd_->setBacklight(on)) {
    qWarning() << "Failed to switch LCD backlight";
  }
}

void QNAPCtl::setLED(QNAPCtl::PanelLED led, bool on) {
  sio_helper_->setLed(led, on);
}

void QNAPCtl::setLED(const QString &led_name, bool on) {
  auto &&meta = QMetaEnum::fromType<PanelLED>();
  bool ok;
  const PanelLED led =
      static_cast<const PanelLED>(meta.keyToValue(qPrintable(led_name), &ok));
  if (!ok)
    return;
  setLED(led, on);
}

void QNAPCtl::emitButtonEvent(QNAPCtl::PanelButton button, bool pressed) {
  emit buttonEvent(QVariant::fromValue(button).toString(), pressed);
}
