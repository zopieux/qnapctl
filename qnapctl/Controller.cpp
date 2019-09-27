#include "Controller.h"

#include <QHostInfo>
#include <QNetworkInterface>

namespace {

void maybeStart(QTimer* timer, int msec) {
  if (timer->isActive()) {
    timer->setInterval(msec);
  } else {
    timer->start(msec);
  }
}

}  // namespace

Controller::Controller(QObject* parent)
    : state_(State::SLEEP),
      blink_green_(false),
      blink_red_(false),
      load_average_(0),
      services_running_(0),
      services_failed_(0) {
  sleep_timer_ = new QTimer(this);
  sleep_timer_->setInterval(4 * 1000);
  sleep_timer_->setSingleShot(true);
  connect(sleep_timer_, &QTimer::timeout, this, &Controller::goToSleep);

  poll_status_timer_ = new QTimer(this);
  poll_status_timer_->setInterval(2 * 1000);
  poll_status_timer_->setSingleShot(false);
  connect(poll_status_timer_, &QTimer::timeout, this, &Controller::pollStatus);

  blink_timer_green_ = new QTimer(this);
  blink_timer_green_->setSingleShot(false);
  connect(blink_timer_green_, &QTimer::timeout,
          [this]() { blinkLed(Led::STATUS_GREEN); });

  blink_timer_red_ = new QTimer(this);
  blink_timer_red_->setSingleShot(false);
  connect(blink_timer_red_, &QTimer::timeout,
          [this]() { blinkLed(Led::STATUS_RED); });

  ctrl_ = new QNAPCtlInterface("eu.zopi.Daemon", "/eu/zopi/qnapctrl",
                               QDBusConnection::systemBus());
  connect(ctrl_, &QNAPCtlInterface::buttonEvent, this,
          &Controller::onButtonEvent);

  goToSleep();
  poll_status_timer_->start();
}

void Controller::goToSleep() {
  ctrl_->setLCDBacklight(false);
  state_ = State::SLEEP;
}

void Controller::pollStatus() {
  QFile f("/proc/loadavg");
  f.open(QIODevice::ReadOnly);
  const QString load_average_str(f.readAll());
  f.close();
  load_average_ = load_average_str.split(' ').at(0).toDouble();

  auto listUnits = QDBusMessage::createMethodCall(
      "org.freedesktop.systemd1", "/org/freedesktop/systemd1",
      "org.freedesktop.systemd1.Manager", "ListUnits");
  auto reply = QDBusConnection::systemBus().call(listUnits);

  size_t service_count = 0, services_running = 0, services_failed = 0;
  const auto& arg = reply.arguments().at(0).value<QDBusArgument>();
  // a(ssssssouso)
  arg.beginArray();
  while (!arg.atEnd()) {
    QString unit, desc, load_state, active_state, sub_state, followed, type;
    QDBusObjectPath unit_path, job_path;
    uint queued = -1;

    arg.beginStructure();
    arg >> unit >> desc >> load_state >> active_state >> sub_state >>
        followed >> unit_path >> queued >> type >> job_path;
    arg.endStructure();

    if (!unit.endsWith(".service")) continue;

    service_count++;
    services_running +=
        (active_state == "active" && sub_state == "running") ? 1 : 0;
    services_failed += active_state == "failed" ? 1 : 0;
  }
  arg.endArray();

  maybeStart(blink_timer_green_, static_cast<int>(1000 / load_average_));

  if (services_failed == 0) {
    blink_timer_red_->stop();
  } else {
    maybeStart(blink_timer_red_, static_cast<int>(1000 / services_failed));
  }

  services_running_ = services_running;
  services_failed_ = services_failed;
}

void Controller::resetSleepTimer() {
  ctrl_->setLCDBacklight(true);
  sleep_timer_->start();
}

void Controller::onButtonEvent(const QString& button, bool pressed) {
  if (button == "USB_COPY") {
    ctrl_->setLED("USB", pressed);
    return;
  }

  if (!pressed) return;

  const bool isSelect = button == "SELECT";
  const bool isEnter = button == "ENTER";

  auto showStatus = [this]() {
    resetSleepTimer();
    state_ = State::STATUS;

    const auto& hostname = QHostInfo::localHostName();
    ctrl_->writeLCD(0, QString("H: %1").arg(hostname));
    ctrl_->writeLCD(1, QString("L: %1  S: %2")
                           .arg(load_average_, 0, 'f', 1)
                           .arg(services_running_, 2));
  };

  auto showNetwork = [this]() {
    resetSleepTimer();
    state_ = State::NETWORK;

    QList<QString> addrs;
    for (const auto& addr : QNetworkInterface::allAddresses()) {
      addrs.append(addr.toString());
    }
    if (addrs.size() >= 1) ctrl_->writeLCD(0, "IP: " + addrs.at(0));
    if (addrs.size() >= 2) ctrl_->writeLCD(1, "IP: " + addrs.at(1));
  };

  switch (state_) {
    case State::SLEEP:
      if (isSelect) showStatus();
      break;

    case State::STATUS:
      if (isSelect) showNetwork();
      break;

    case State::NETWORK:
      if (isSelect) showStatus();
      break;
  }
}

void Controller::blinkLed(Controller::Led led) {
  switch (led) {
    case Led::STATUS_GREEN:
      blink_green_ = !blink_green_;
      ctrl_->setLED("STATUS_GREEN", blink_green_);
      break;
    case Led::STATUS_RED:
      blink_red_ = !blink_red_;
      ctrl_->setLED("STATUS_RED", blink_red_);
      break;
  }
}
