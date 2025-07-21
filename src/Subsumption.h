#ifndef YAGA_SUBSUMPTION_H
#define YAGA_SUBSUMPTION_H

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <deque>
#include <ranges>
#include <vector>

#include "Clause.h"
#include "Database.h"
#include "Event_listener.h"
#include "Literal.h"
#include "Literal_map.h"
#include "Model.h"
#include "Tracer_wrapper.h"
#include "Trail.h"
#include "Variable.h"

namespace yaga {

/** Periodically (on restart) removes subsumed clauses.
 *
 * It also minimizes learned clauses using self-subsumption
 */
class Subsumption final : public Event_listener {
public:
    Subsumption(proof::Tracer_wrapper tracer = {}) : tracer(tracer) {}

    /** Allocate memory for internal structures.
     *
     * @param type type of variables
     * @param num_vars new number of variables of type @p type
     */
    void on_variable_resize(Variable::Type type, int num_vars) override;

    /** Find and remove subsumed learned clauses from db
     *
     * @param db clause database
     * @param trail current solver trail
     */
    void on_restart(Database& db, Trail& trail) override;

    /** Minimize @p clause using self-subsuming resolution.
     *
     * @param trail current solver trail
     * @param clause clause to minimize
     */
    void minimize(Trail const& trail, Clause& clause);

private:
    // Clause pointer proxy which also stores signature of the clause.
    // Signature is a 64-bit mask of the clause such that if a clause A is a
    // subset of a clause B, then A.sig() is a subset of B.sig() (but not
    // necessarily vice versa)
    class Clause_ptr {
    public:
        inline Clause_ptr() {}
        inline Clause_ptr(Clause* ptr, std::uint64_t sig) : clause_ptr(ptr), clause_sig(sig) {}
        inline Clause_ptr(Clause_ptr const&) = default;
        inline Clause_ptr& operator=(Clause_ptr const&) = default;
        inline Clause* operator->() { return clause_ptr; }
        inline Clause& operator*() { return *clause_ptr; }
        inline std::uint64_t sig() const { return clause_sig; }
        inline bool operator==(Clause_ptr const& other) const
        {
            return clause_ptr == other.clause_ptr;
        }
        inline bool operator!=(Clause_ptr const& other) const { return !operator==(other); }

    private:
        // pointer to the clause
        Clause* clause_ptr;
        // clause signature
        std::uint64_t clause_sig;
    };

    using Clause_iterator = std::deque<Clause>::iterator;

    // map literal -> clauses in which it occurs (set by `index()`)
    Literal_map<std::vector<Clause_ptr>> occur;
    // auxiliary bitset for subset tests in subsumes() and selfsubsumes()
    Literal_map<bool> lit_bitset;
    // number of learned clauses in previous restart
    std::size_t old_size = 0;
    // Tracer for proof production (optional)
    proof::Tracer_wrapper tracer;

    // compute signature of a clause and create a proxy object which includes
    // this signature
    inline Clause_ptr make_proxy(Clause* clause) const
    {
        Literal_hash hash;

        constexpr std::uint64_t MOD64 = (1 << 6) - 1; // bitmask for mod 64
        std::uint64_t sig = 0;
        for (auto lit : *clause)
        {
            sig |= 1UL << (hash(lit) & MOD64);
        }
        return {clause, sig};
    }

    /** Check if @p first is a proper subset of @p second
     *
     * @param first pointer to the first clause with its signature
     * @param second pointer to the second clause with its signature
     * @return true iff @p first is a proper subset of @p second
     */
    bool subsumes(Clause_ptr first, Clause_ptr second);

    /** Check if `resolve(first, second, lit)` is a proper subset of @p second
     *
     * @param first first clause
     * @param second second clause
     * @param lit literal for resolution in @p first ; negation of a literal in
     * @p second
     * @return true iff `resolve(first, second, lit)` is a proper subset of @p
     * second
     */
    bool selfsubsumes(Clause const& first, Clause const& second, Literal lit);

    /** Construct `occur` from learned clauses in @p db
     *
     * @param begin begin iterator of a range of clauses to index
     * @param end end iterator of a range of clauses to index
     */
    void index(Clause_iterator being, Clause_iterator end);

    /** Mark clauses subsumed by @p clause (by making them empty)
     *
     * @param clause
     */
    void remove_subsumed(Clause_ptr clause);

    // removed subsumed learned clauses
    void remove_subsumed(Database& db);
};

} // namespace yaga

#endif // YAGA_SUBSUMPTION_H