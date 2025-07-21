#include "Frat_tracer.h"
#include <cassert>
#include <format>
#include <unordered_set>

namespace yaga::proof {

Frat_tracer::Frat_tracer(std::string const& output_path, bool binary_mode)
    : binary_mode(binary_mode)
{
    output.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    if (binary_mode)
    {
        output.open(output_path, std::ios::binary);
    }
    else
    {
        output.open(output_path);
    }
}

void Frat_tracer::trivial_proof()
{
    Clause empty;
    write_comment("False asserted, proof trivial");
    original_clause(empty);
    final_clause(empty);
}

void Frat_tracer::begin_proof(Database const& db)
{
    // TODO: disallow rebeginning?

    for (auto const& clause : db.asserted())
    {
        original_clause(clause);
    }
}

void Frat_tracer::init_conflict(Clause const& conflict, Conflict_explanation&& explanation)
{
    if (!std::holds_alternative<conflict::Boolean>(explanation))
    {
        // Theory conflict
        assert(!clause_definitions.contains(conflict.id()));
        if (!binary_mode)
        {
            theory_comment(explanation);
        }
        original_clause(conflict);
        open_theory_conflicts.emplace(clause_definitions[conflict.id()], conflict);
    }
    assert(clause_definitions.contains(conflict.id()));
    open_conflicts[conflict.id()] =
        Proof_node{node::Conflict_intro{conflict.id(), std::move(explanation)}};
}

void Frat_tracer::resolve_conflict(Clause_id conflict, Clause_id other)
{
    assert(open_conflicts.contains(conflict));
    assert(clause_definitions.contains(other));
    open_conflicts[conflict] =
        node::Resolution{std::make_unique<Proof_node>(std::move(open_conflicts[conflict])), other};
}

void Frat_tracer::rename_conflict(Clause_id from, Clause_id to)
{
    assert(open_conflicts.contains(from));
    if (from != to)
    {
        assert(!open_conflicts.contains(to));
        open_conflicts[to] = std::move(open_conflicts[from]);
        open_conflicts.erase(from);
    }
}

void Frat_tracer::finish_conflicts()
{
    // These clauses have served their purpose
    // They will not be in the database at the end of the proof, so we have to delete them now
    for (auto& [_, theory_conflict_clause] : open_theory_conflicts)
    {
        delete_clause(theory_conflict_clause);
    }
    open_theory_conflicts.clear();
    open_conflicts.clear();
}

void Frat_tracer::learn_clause(Clause const& learned)
{
    assert(open_conflicts.contains(learned.id()));
    // TODO: move theory conflict introduction here?
    auto proof = collect_resolvents(open_conflicts[learned.id()]);
    if (proof.size() == 1 && open_theory_conflicts.contains(clause_definitions[proof.front()]))
    {
        // Trivial conflict analysis of theory conflict with no resolution steps
        write_comment(
            std::format("Theory clause {} learned as-is", clause_definitions[proof.front()]));
        clause_definitions[learned.id()] = clause_definitions[proof.front()];
        // The original explanation clause is added to the database, so we don't need to clean it up
        open_theory_conflicts.erase(clause_definitions[proof.front()]);
        return;
    }
    auto proof_id = next_step_id++;
    assert(!clause_definitions.contains(learned.id()));
    clause_definitions[learned.id()] = proof_id;
    write_command('a');
    write_unsigned(proof_id);
    write_clause(learned);
    if (!proof.empty())
    {
        write_zero();
        write_command('l');
        for (auto const& clause : proof)
        {
            // Signed because negative values are used in RAT steps
            write_signed(clause_definitions[clause]);
        }
    }
    end_command();
}

void Frat_tracer::delete_clause(Clause const& deleted)
{
    assert(clause_definitions.contains(deleted.id()));
    auto proof_id = clause_definitions.at(deleted.id());
    clause_definitions.erase(deleted.id()); // this should be safe?
    write_command('d');
    write_unsigned(proof_id);
    write_clause(deleted);
    end_command();
}

void Frat_tracer::derive_final(Clause const& empty)
{
    learn_clause(empty);
    final_clause(empty);
}

void Frat_tracer::end_proof(Database const& db)
{
    for (auto& [_, theory_conflict_clause] : open_theory_conflicts)
    {
        final_clause(theory_conflict_clause);
    }
    for (auto it = db.learned().rbegin(); it != db.learned().rend(); ++it)
    {
        final_clause(*it);
    }
    for (auto it = db.asserted().rbegin(); it != db.asserted().rend(); ++it)
    {
        final_clause(*it);
    }
}

void Frat_tracer::original_clause(Clause const& clause)
{
    auto proof_id = next_step_id++;
    assert(!clause_definitions.contains(clause.id()));
    clause_definitions[clause.id()] = proof_id;
    write_command('o');
    write_unsigned(proof_id);
    write_clause(clause);
    end_command();
}

void Frat_tracer::final_clause(Clause const& clause)
{
    assert(clause_definitions.contains(clause.id()));
    auto proof_id = clause_definitions.at(clause.id());
    write_command('f');
    write_unsigned(proof_id);
    write_clause(clause);
    end_command();
}

void Frat_tracer::theory_comment(Conflict_explanation const& /*explanation*/)
{
    // TODO: better explanation
    write_comment("Theory conflict");
}

void Frat_tracer::write_command(char cmd)
{
    assert(!binary_mode); // TODO: test binary mode support
    assert(cmd >= 0);
    if (binary_mode)
    {
        output << cmd;
    }
    else
    {
        output << cmd << ' ';
    }
}

void Frat_tracer::write_unsigned(std::size_t value)
{
    if (binary_mode)
    {
        while (true)
        {
            unsigned char byte = value & 0x7F; // take last 7 bits
            value >>= 7;
            if (value)
            {
                output << (byte | 0x80); // continuation bit
            }
            else
            {
                output << byte;
                break;
            }
        }
    }
    else
    {
        output << value << ' ';
    }
}

void Frat_tracer::write_signed(int value)
{
    if (binary_mode)
    {
        if (value >= 0)
        {
            // n => 2n
            write_unsigned(2 * static_cast<std::size_t>(value));
        }
        else
        {
            // -n => 2n + 1
            write_unsigned(2 * static_cast<std::size_t>(-value) + 1);
        }
    }
    else
    {
        output << value << ' ';
    }
}

void Frat_tracer::write_zero()
{
    if (binary_mode)
    {
        output << '\0';
    }
    else
    {
        output << "0 ";
    }
}

void Frat_tracer::end_command()
{
    if (binary_mode)
    {
        output << '\0';
    }
    else
    {
        output << "0\n";
    }
}

void Frat_tracer::write_clause(Literals const& args)
{
    for (auto const& arg : args)
    {
        auto lit = arg.var().ord() + 1;
        if (arg.is_negation())
        {
            lit *= -1;
        }
        write_signed(lit);
    }
}

void Frat_tracer::write_comment(std::string const& comment)
{
    if (!binary_mode)
    {
        output << "c " << comment << " .\n";
    }
}

} // namespace yaga::proof