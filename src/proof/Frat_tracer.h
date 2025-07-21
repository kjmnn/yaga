#ifndef YAGA_FRAT_TRACER_H
#define YAGA_FRAT_TRACER_H

#include "Proof_node.h"
#include "Tracer.h"
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <unordered_map>

namespace yaga::proof {

/** Tracer that produces (ascii / binary) FRAT proofs
 *
 * As FRAT was made for SAT solvers, theory conflicts are modelled as assertions
 * and their correctness is not checked.
 */
class Frat_tracer : public Tracer {

public:
    Frat_tracer(std::string const& output_path, bool binary_mode = false);
    ~Frat_tracer() override = default;

    void trivial_proof() override;
    void begin_proof(Database const&) override;
    void init_conflict(Clause const&, Conflict_explanation&&) override;
    void resolve_conflict(Clause_id conflict, Clause_id other) override;
    void rename_conflict(Clause_id from, Clause_id to) override;
    void finish_conflicts() override;
    void learn_clause(Clause const&) override;
    void delete_clause(Clause const& deleted) override;
    void derive_final(Clause const& empty) override;
    void end_proof(Database const&) override;

    bool supports_lra() const override { return true; }
    bool supports_uf() const override { return true; }

private:
    // Conflict clause id -> proof node
    std::map<Clause_id, Proof_node> open_conflicts;
    // keeps track of theory conflicts for cleanup if not learned
    std::map<std::size_t, Clause> open_theory_conflicts;
    // Clause id -> proof step id
    std::unordered_map<Clause_id, std::size_t> clause_definitions;
    bool binary_mode = false;
    std::ofstream output;
    std::size_t next_step_id = 1;

    /** Add an original (asserted / theory) clause
     *
     * @param clause original clause
     */
    void original_clause(Clause const& clause);
    /** Add a final (empty) clause
     *
     * @param clause final clause
     */
    void final_clause(Clause const& clause);
    /** Write a comment explaining a theory conflict
     *
     * @param explanation theory conflict explanation
     */
    void theory_comment(Conflict_explanation const& explanation);

    /** Write a command to the output
     *
     * @param cmd command character (e.g. 'o' for original clause)
     */
    void write_command(char);
    /** Write an unsigned integer to the output
     *
     * @param value value to write
     */
    void write_unsigned(std::size_t);
    /** Write a signed integer to the output (encoded if in binary mode)
     *
     * @param value value to write
     */
    void write_signed(int);
    /** Separate command parts with a zero
     */
    void write_zero();
    /** End a command (write zero + newline if not in binary mode)
     */
    void end_command();
    /** Write a clause to the output
     *
     * @param args clause literals
     */
    void write_clause(Literals const& args);
    /** Write a comment to the output
     *
     * @param comment comment to write
     */
    void write_comment(std::string const& comment);
};

} // namespace yaga::proof

#endif