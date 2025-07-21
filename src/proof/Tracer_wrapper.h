#ifndef YAGA_TRACER_WRAPPER_H
#define YAGA_TRACER_WRAPPER_H

#include "Options.h"
#include "Tracer.h"
#include <memory>

namespace yaga::proof {

/** Wraps a shared pointer to a tracer and forwards its interface.
 *
 * It exists for 2 reasons:
 * 1. To avoid null checks on every tracer method call site.
 *    (If the tracer is missing, the calls do nothing.)
 * 2. To allow constructing a tracer from options in a member initializer list.
 */
class Tracer_wrapper : public Tracer {
public:
    Tracer_wrapper() = default;
    Tracer_wrapper(Options const& opts);
    Tracer_wrapper(Tracer_wrapper const& other) : tracer(other.tracer) {};
    ~Tracer_wrapper() override = default;
    Tracer_wrapper& operator=(Tracer_wrapper const&) = default;

    void trivial_proof() override
    {
        if (tracer)
        {
            tracer->trivial_proof();
        }
    }
    void begin_proof(Database const& db) override
    {
        if (tracer)
        {
            tracer->begin_proof(db);
        }
    }
    void init_conflict(Clause const& conflict, Conflict_explanation&& explanation) override
    {
        if (tracer)
        {
            tracer->init_conflict(conflict, std::move(explanation));
        }
    }
    void resolve_conflict(Clause_id conflict, Clause_id other) override
    {
        if (tracer)
        {
            tracer->resolve_conflict(conflict, other);
        }
    }

    void rename_conflict(Clause_id from, Clause_id to) override
    {
        if (tracer)
        {
            tracer->rename_conflict(from, to);
        }
    }

    void finish_conflicts() override
    {
        if (tracer)
        {
            tracer->finish_conflicts();
        }
    }
    void learn_clause(Clause const& learned) override
    {
        if (tracer)
        {
            tracer->learn_clause(learned);
        }
    }
    void delete_clause(Clause const& deleted) override
    {
        if (tracer)
        {
            tracer->delete_clause(deleted);
        }
    }
    void derive_final(Clause const& empty) override
    {
        if (tracer)
        {
            tracer->derive_final(empty);
        }
    }
    void end_proof(Database const& db) override
    {
        if (tracer)
        {
            tracer->end_proof(db);
        }
    }
    bool supports_lra() const override { return tracer ? tracer->supports_lra() : true; }
    bool supports_uf() const override { return tracer ? tracer->supports_uf() : true; }

    operator bool() const { return tracer != nullptr; }

private:
    std::shared_ptr<Tracer> tracer;
};

} // namespace yaga::proof

#endif