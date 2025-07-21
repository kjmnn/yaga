#ifndef YAGA_CONFLICT_EXPLANATION_H
#define YAGA_CONFLICT_EXPLANATION_H

#include "Rational.h"
#include <variant>
#include <vector>

namespace yaga::proof {

namespace conflict {
// Learned / asserted clause unsatisfiable
struct Boolean {};
// Lower and upper bounds in conflict
struct Lra_bounds {
    // Farkas coefficients
    std::vector<Rational> coefficients;
};
// Non-strict bounds determine a value prohibited by a disequality
struct Lra_disequality {
    // Farkas coefficients
    std::vector<Rational> coefficients;
};
// x... == y... but f(x...) != f(y...)
struct Uf_congruence {};
} // namespace conflict

/** Explains conflict clause origin (along with a proof / witness if necessary.)
 */
using Conflict_explanation = std::variant<conflict::Boolean, conflict::Lra_bounds,
                                          conflict::Lra_disequality, conflict::Uf_congruence>;

} // namespace yaga::proof

#endif