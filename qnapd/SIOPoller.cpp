#include "SIOPoller.h"

#include <sys/io.h>

#include <QBitArray>
#include <QDebug>
#include <QTimer>

namespace {

constexpr int kPollIntervalMs = 50;

template <typename T, std::size_t N>
using Port =
    std::pair<unsigned char, std::array<std::pair<T, unsigned char>, N>>;

constexpr unsigned short kRegPort = 0xa05;
constexpr unsigned short kValuePort = kRegPort + 1;
constexpr int kPortCount = 2;

constexpr Port<QNAPCtrl::PanelLED, 2> portStatusLed = {
    0x91,
    {{
        {QNAPCtrl::PanelLED::STATUS_GREEN, 2},
        {QNAPCtrl::PanelLED::STATUS_RED, 3},
    }}};

constexpr Port<QNAPCtrl::PanelLED, 4> portDiskLeds = {
    0x81,
    {{
        {QNAPCtrl::PanelLED::DISK_1, 0},
        {QNAPCtrl::PanelLED::DISK_2, 1},
        {QNAPCtrl::PanelLED::DISK_3, 2},
        {QNAPCtrl::PanelLED::DISK_4, 3},
    }}};

constexpr Port<QNAPCtrl::PanelLED, 1> portUsbLed = {
    0xE1,
    {{
        {QNAPCtrl::PanelLED::USB, 7},
    }}};

constexpr Port<QNAPCtrl::PanelButton, 1> portUsbButton = {
    0xE2,
    {{
        {QNAPCtrl::PanelButton::USB_COPY, 2},
    }}};

// Reads the current bitmask for the port, applies any bit modifications from
// 'new_values' (if they are relevant to this port) and writes the new bitmask.
// Returns the read bitmask, decoded (before any edits).
template <typename T, std::size_t N>
QMap<T, bool> readModifyMaybeWrite(const Port<T, N> &ports,
                                   QMap<T, bool> new_values = {}) {
  QMap<T, bool> current_values;

  // Read current value.
  outb(ports.first, kRegPort);
  unsigned char current_uchar = inb(kValuePort);
  // Transform to a nifty QBitArray. Careful, a set bit means off, hence
  // reversed.
  char current_char[] = {static_cast<char>(current_uchar)};
  auto mask = ~QBitArray::fromBits(current_char, 8);

  for (auto &&[led, bit] : ports.second) {
    // Fill-in decoded return value.
    current_values[led] = mask.testBit(bit);

    // Apply bitmask edits from 'new_values'.
    if (new_values.contains(led)) {
      mask.setBit(bit, new_values.value(led));
    }
  }

  // Write new encoded bitmask.
  if (!new_values.empty()) {
    outb(static_cast<unsigned char>((~mask).bits()[0]), kValuePort);
  }

  return current_values;
}

} // namespace

void SIOPoller::run() {
  auto *timer = new QTimer;
  connect(timer, &QTimer::timeout, this, &SIOPoller::poll);
  timer->start(kPollIntervalMs);
  exec();
}

void SIOPoller::poll() {
  // Open port.
  ioperm(kRegPort, kPortCount, 1);

  // LED ports: read and modify state if needed.
  readModifyMaybeWrite(portStatusLed, intended_state_);
  readModifyMaybeWrite(portUsbLed, intended_state_);
  readModifyMaybeWrite(portDiskLeds, intended_state_);
  intended_state_.clear();

  // Special case for USB COPY button. This is a read-only port.
  // Emit the relevant signal when state toggles.
  const auto &portE2_values = readModifyMaybeWrite(portUsbButton);
  const bool usb_copy_pressed =
      portE2_values.value(QNAPCtrl::PanelButton::USB_COPY);
  if (usb_copy_pressed != usb_copy_pressed_) {
    emit buttonEvent(QNAPCtrl::PanelButton::USB_COPY, usb_copy_pressed);
  }
  usb_copy_pressed_ = usb_copy_pressed;

  // Close port.
  ioperm(kRegPort, kPortCount, 0);
}

void SIOPoller::setLed(QNAPCtrl::PanelLED led, bool on) {
  intended_state_[led] = on;
}
