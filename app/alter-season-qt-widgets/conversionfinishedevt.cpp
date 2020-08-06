#include "conversionfinishedevt.h"

const QEvent::Type ConversionFinishedEvent::ConversionFinishedEventType = static_cast<QEvent::Type>(QEvent::registerEventType(QEvent::User + 1));