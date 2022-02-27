#ifndef __GNOME_MAGNIFIER_H
#define __GNOME_MAGNIFIER_H

#include "../Magnifier.h"

namespace gnome {
class Magnifier : public ::Magnifier {
  private:
    void update_mag_factor();

  public:
    Magnifier(const Config &config);
    virtual ~Magnifier();

    virtual void magnify() override;
    virtual void unmagnify() override;
    virtual void enlarge() override;
    virtual void shrink() override;
};
} // namespace gnome

#endif // __GNOME_MAGNIFIER_H