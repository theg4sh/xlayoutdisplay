#include "Mode.h"

#include "xrandrrutil.h"

using namespace std;

Mode *Mode::fromXRR(const RRMode id, const XRRScreenResources *resources) {
    if (resources == nullptr)
        throw invalid_argument("cannot construct Mode: NULL XRRScreenResources");

    XRRModeInfo *modeInfo = nullptr;
    for (int i = 0; i < resources->nmode; i++) {
        if (id == resources->modes[i].id) {
            modeInfo = &(resources->modes[i]);
            break;
        }
    }

    if (modeInfo == nullptr)
        throw invalid_argument("cannot construct Mode: cannot retrieve RRMode '" + to_string(id) + "'");

    return new Mode(id, modeInfo->width, modeInfo->height, refreshFromModeInfo(*modeInfo));
}

const bool Mode::operator<(const Mode &o) const {
    if (width == o.width)
        if (height == o.height)
            return refresh < o.refresh;
        else
            return height < o.height;
    else
        return width < o.width;
}
