#ifndef YAGA_TRACER_H
#define YAGA_TRACER_H

#include "Clause.h"
#include "Conflict_explanation.h"
#include "Database.h"

namespace yaga::proof {

/** Tracer interface for proof generation.
 *
 * Tracers trace the execution of the solver's algorithm to produce a proof of unsatisfiability
 * (if the input formula is unsatisfiable)
 */
class Tracer {
public:
    Tracer() = default;
    Tracer(Tracer const&) = delete;
    Tracer(Tracer&&) = delete;
    virtual ~Tracer() = default;
    Tracer& operator=(Tracer const&) = delete;
    Tracer& operator=(Tracer&&) = delete;

    /** Trivial proof
     * Called when "false" is asserted - or something the parser simplifies to "false".
     */
    virtual void trivial_proof() = 0;
    /** Begin writing a proof
     *
     * @param db clause database containing asserted clauses
     */
    virtual void begin_proof(Database const& db) = 0;
    /** Initialize a conflict
     *
     * @param conflict conflict clause
     * @param explanation conflict type tag and witness
     */
    virtual void init_conflict(Clause const& conflict, Conflict_explanation&& explanation) = 0;
    /** Resolve a conflict clause
     *
     * @param conflict id of the active conflict clause
     * @param other id of the clause to resolve with
     */
    virtual void resolve_conflict(Clause_id conflict, Clause_id other) = 0;
    /** Change the id of an active conflict
     * Used when a new clause (with a new id) is created to represent an existing conflict.
     *
     * @param from id of the old conflict clause
     * @param to id of the new conflict clause
     */
    virtual void rename_conflict(Clause_id from, Clause_id to) = 0;
    /** End a conflict resolution phase
     * (signals the tracer to clean up conflicts that did not result in a learned clause)
     */
    virtual void finish_conflicts() = 0;
    /** Learn a new clause (and close the corresponding conflict)
     *
     * @param learned clause to learn
     */
    virtual void learn_clause(Clause const& learned) = 0;
    /** Delete a clause
     *
     * @param deleted clause to delete
     */
    virtual void delete_clause(Clause const& deleted) = 0;
    /** Derive the final empty clause (called instead of learn_clause)
     *
     * @param empty empty clause
     */
    virtual void derive_final(Clause const& empty) = 0;
    /** End the proof (clean up, finalize, prune the proof...)
     *
     * @param db clause database containing asserted and learned clauses
     */
    virtual void end_proof(Database const& db) = 0;

    virtual bool supports_lra() const = 0;
    virtual bool supports_uf() const = 0;
};

} // namespace yaga::proof

#endif