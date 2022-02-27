#ifndef __GNOME_MAGNIFIER_H
#define __GNOME_MAGNIFIER_H

#include "../Magnifier.h"

namespace gnome {
class Magnifier : public ::Magnifier {
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

#endif // __GNOME_MAGNIFIER_H