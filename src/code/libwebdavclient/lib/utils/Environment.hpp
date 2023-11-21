#pragma once

#include <QString>

/**
    * @private
    */
class Environment {
public:
  static QString get(QString key);
};
