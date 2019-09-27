#pragma once

#include <QtSerialPort>

#include "Daemon.h"

class LCD : public QObject {
  Q_OBJECT

 public:
  explicit LCD(QObject *parent = nullptr);

 signals:
  void buttonEvent(Daemon::PanelButton button, bool pressed);

 public slots:
  bool open(const QString &portName);

  bool write(int line, const QByteArray &chars);

  bool setBacklight(bool on);

 private slots:
  void readyRead();

 private:
  QSerialPort *serial_port_;
  bool has_state_ = false;
  bool btn_enter_state_ = false;
  bool btn_select_state_ = false;
};
