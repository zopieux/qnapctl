#pragma once

#include <QDBusArgument>
#include <QDBusMetaType>
#include <QFlags>
#include <QObject>

class LCD;
class SIOPoller;

class QNAPCtrl : public QObject {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "eu.zopi.QNAPCtrlInterface")

public:
  enum class PanelButton {
    ENTER,
    SELECT,
    USB_COPY,
  };
  Q_ENUM(PanelButton)

  enum class PanelLED {
    STATUS_GREEN,
    STATUS_RED,
    USB,
    DISK_1,
    DISK_2,
    DISK_3,
    DISK_4,
  };
  Q_ENUM(PanelLED)

  using PanelButtons = QSet<PanelButton>;

  explicit QNAPCtrl(QObject *parent = nullptr);

signals:
  Q_SCRIPTABLE void buttonEvent(QString button, bool pressed);

public slots:
  Q_SCRIPTABLE void writeLCD(QString text);

  Q_SCRIPTABLE void writeLCD(int line, QString text);

  Q_SCRIPTABLE void setLCDBacklight(bool on);

  void setLED(PanelLED led, bool on);
  Q_SCRIPTABLE void setLED(const QString &led, bool on);

  void emitButtonEvent(QNAPCtrl::PanelButton button, bool pressed);

private:
  LCD *lcd_;
  SIOPoller *sio_helper_;
};

inline uint qHash(const QNAPCtrl::PanelButton &key, uint seed) {
  return ::qHash(static_cast<uint>(key), seed);
}

inline uint qHash(const QNAPCtrl::PanelLED &key, uint seed) {
  return ::qHash(static_cast<uint>(key), seed);
}
