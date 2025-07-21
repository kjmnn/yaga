#ifndef YAGA_CLAUSE_H
#define YAGA_CLAUSE_H

#include <cassert>
#include <vector>

#include "Literal.h"

namespace yaga {


using Literals = std::vector<Literal>;

// TODO: remove the old implementation
// /** Disjunction of literals
//  */
// using Clause = Literals;

using Clause_id = int;

/** Disjunction of literals
 * 
 * Also contains a unique id for more efficient tracking
 */
class Clause : public Literals {
public:
    // Clause() = default;

    using Literals::Literals;

    Clause(Literals&& literals) : Literals{std::move(literals)} {}

    Clause(Clause const&) = default;
    Clause(Clause&&) = default;

    Clause& operator=(Clause const&) = default;
    Clause& operator=(Clause&&) = default;

    Clause_id id() const { return id_; }

private:
    static Clause_id next_id;
    Clause_id id_ = next_id++;
};


} // namespace yaga

#endif // YAGA_CLAUSE_H