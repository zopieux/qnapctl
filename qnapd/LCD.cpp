#include "LCD.h"
#include "QNAPCtl.h"

namespace {

constexpr size_t MSG_SIZE = 4;

bool writeCheck(QSerialPort *port, const QByteArray &data) {
  return port->write(data) == data.size();
}

} // namespace

LCD::LCD(QObject *parent) : QObject(parent) {
  serial_port_ = new QSerialPort(this);
}

bool LCD::open(const QString &portName) {
  if (serial_port_)
    serial_port_->deleteLater();

  serial_port_ = new QSerialPort(this);
  serial_port_->setPortName(portName);
  serial_port_->setBaudRate(1200);
  serial_port_->setReadBufferSize(MSG_SIZE);

  connect(serial_port_, &QSerialPort::readyRead, this, &LCD::readyRead);

  bool ok = serial_port_->open(QIODevice::ReadWrite);
  if (!ok)
    return ok;

  constexpr char kSendTheButtonState[] = {0x4D, 0x06};
  return writeCheck(serial_port_, QByteArray(kSendTheButtonState, 2));
}

bool LCD::write(int line, const QByteArray &chars) {
  constexpr char kWriteText[] = {0x4D, 0x0C, 0x00, 0x10};
  constexpr size_t kLineLength = 16;

  if (!serial_port_)
    return false;

  if (!(0 <= line && line <= 1))
    return false;

  QByteArray buf(kWriteText, 4);
  buf[2] = line;
  buf.append(chars.left(kLineLength));
  return writeCheck(serial_port_, buf);
}

bool LCD::setBacklight(bool on) {
  constexpr char kBacklight[] = {0x4D, 0x5E, 0x00};

  if (!serial_port_)
    return false;

  QByteArray buf(kBacklight, 3);
  buf[2] = on ? 0x01 : 0x00;
  return writeCheck(serial_port_, buf);
}

void LCD::readyRead() {
  constexpr char kButtonPressHeader[] = {0x53, 0x05, 0x00};
  constexpr char kButtonEnter = 0x01;
  constexpr char kButtonSelect = 0x02;

  if (!serial_port_)
    return;

  if (serial_port_->bytesAvailable() < MSG_SIZE)
    return;

  char buf[MSG_SIZE];
  if (serial_port_->read(buf, MSG_SIZE) != MSG_SIZE)
    return;

  if (qstrncmp(buf, kButtonPressHeader, 3) != 0)
    return;

  const bool btn_enter_state = buf[3] & kButtonEnter;
  const bool btn_select_state = buf[3] & kButtonSelect;

  if (has_state_ && btn_enter_state != btn_enter_state_) {
    emit buttonEvent(QNAPCtl::PanelButton::ENTER, btn_enter_state);
  }

  if (has_state_ && btn_select_state != btn_select_state_) {
    emit buttonEvent(QNAPCtl::PanelButton::SELECT, btn_select_state);
  }

  has_state_ = true;
  btn_enter_state_ = btn_enter_state;
  btn_select_state_ = btn_select_state;
}
