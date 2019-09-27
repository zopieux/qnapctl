#include "LedBlinker.h"

#include <memory>

constexpr int kBlinkTimeMs = 100;

LedBlinker::LedBlinker(QNAPCtlInterface* ctrl, QString led, QObject* parent)
    : QObject(parent), ctrl_(ctrl), led_(std::move(led)), next_interval_(0) {
  timer_on_ = new QTimer(this);
  timer_on_->setSingleShot(false);
  connect(timer_on_, &QTimer::timeout, [this]() {
    if (next_interval_ != 0) {
      timer_on_->start(next_interval_);
      timer_off_->start();
      next_interval_ = 0;
    }
    ctrl_->setLED(led_, true);
  });

  timer_off_ = new QTimer(this);
  timer_off_->setSingleShot(false);
  timer_off_->setInterval(kBlinkTimeMs);
  connect(timer_off_, &QTimer::timeout,
          [this]() { ctrl_->setLED(led_, false); });
}

void LedBlinker::setInterval(int msec) {
  if (msec == 0) {
    timer_on_->stop();
    return;
  }
  if (timer_on_->isActive()) {
    next_interval_ = msec;
  } else {
    next_interval_ = 0;
    timer_on_->start(msec);
  }
}
