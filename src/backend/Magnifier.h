#ifndef __MAGNIFIER_H
#define __MAGNIFIER_H

#include "Config.h"

class Magnifier {
  protected:
    bool is_tracking;
    bool is_magnified;
    double cur_mag_factor;

    double mag_factor;
    double mag_enlarge_factor;
    double mag_shrink_factor;

  public:
    Magnifier(const Config &config)
        : is_tracking(false), is_magnified(false), cur_mag_factor(1.0),
          mag_factor(config.mag_factor),
          mag_enlarge_factor(config.mag_enlarge_factor),
          mag_shrink_factor(config.mag_shrink_factor){};
    virtual ~Magnifier() = default;

    virtual void magnify() = 0;
    virtual void unmagnify() = 0;
    void toggle() {
        if (is_magnified) {
            unmagnify();
        } else {
            magnify();
        }
    }
    virtual void enlarge() = 0;
    virtual void shrink() = 0;

    virtual void update() const = 0;
};

#endif // __MAGNIFIER_H