#ifndef YAGA_PROOF_NODE_H
#define YAGA_PROOF_NODE_H

#include "Clause.h"
#include "Conflict_explanation.h"
#include <memory>
#include <variant>

namespace yaga::proof {

// Forward declaration of node types
namespace node {
struct Conflict_intro;
struct Resolution;
} // namespace node

/** Proof tree node
 */
using Proof_node = std::variant<node::Conflict_intro, node::Resolution>;

// TODO: remove pointer recursion?
namespace node {
// Conflict clause introduction
struct Conflict_intro {
    Clause_id id;
    Conflict_explanation explanation; // Explanation of the introduction
};
// (Linear) resolution step
struct Resolution {
    std::unique_ptr<Proof_node> conflict;
    Clause_id other;
};
} // namespace node

// TODO move to .cpp
/** Collect resolvents from a linear resolution chain
 *
 * @param node last node in the chain
 * @return vector of resolved clause ids, ordered from last to first
 */
inline std::vector<Clause_id> collect_resolvents(Proof_node const& node)
{
    Proof_node const* current = &node;
    std::vector<Clause_id> resolvents;
    while (std::holds_alternative<node::Resolution>(*current))
    {
        auto const& res = std::get<node::Resolution>(*current);
        resolvents.push_back(res.other);
        current = res.conflict.get();
    }
    assert(std::holds_alternative<node::Conflict_intro>(*current));
    resolvents.push_back(std::get<node::Conflict_intro>(*current).id);
    return resolvents;
}

} // namespace yaga::proof

#endif