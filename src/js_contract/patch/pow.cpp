
#include "src/base/ieee754.h"

#include <cmath>
#include <limits>

#include "src/base/build_config.h"
#include "src/base/macros.h"
#include "src/base/overflowing-math.h"

#define pow pow_old
#include "src/base/ieee754.cc"
#undef pow

namespace v8 {
namespace base {
    namespace ieee754 {
        double pow(double x, double y)
        {
            if (y == 0.0) {
                return 1.0;
            }
            if (std::isnan(y) || ((x == 1 || x == -1) && std::isinf(y))) {
                return std::numeric_limits<double>::quiet_NaN();
            }
            return std::pow(x, y);
        }
    } // namesapce ieee745
} // namespace base
} // namespace v8