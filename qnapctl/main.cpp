#include <QCoreApplication>

#include "Controller.h"

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);
  Controller ctl;
  return app.exec();
}
