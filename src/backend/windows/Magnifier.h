#ifndef __WINDOWS_MAGNIFIER_H
#define __WINDOWS_MAGNIFIER_H

#include <functional>

#include "../Magnifier.h"

namespace windows {
class Magnifier : public ::Magnifier {
  private:
    static const float dead_zone_factor;

    void update_mag_factor();

    int transform_x(int) const;
    int transform_y(int) const;

  public:
    Magnifier(const Config &config);
    virtual ~Magnifier();

    virtual void magnify() override;
    virtual void unmagnify() override;
    virtual void enlarge() override;
    virtual void shrink() override;

    virtual void update() const override;
};
} // namespace gnome

#endif // __WINDOWS_MAGNIFIER_H