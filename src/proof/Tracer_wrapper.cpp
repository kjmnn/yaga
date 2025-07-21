#include "Tracer_wrapper.h"
#include "Frat_tracer.h"

yaga::proof::Tracer_wrapper::Tracer_wrapper(Options const& opts)
{
    if (opts.produce_proofs)
    {
        auto path = opts.proof_path;
        switch (opts.proof_format)
        {
        case Options::Proof_format::frat_ascii:
            if (path.empty())
            {
                path = opts.input_path + ".frat";
            }
            tracer = std::make_unique<Frat_tracer>(path);
            break;
        case Options::Proof_format::frat_binary:
            if (path.empty())
            {
                path = opts.input_path + ".bfrat";
            }
            tracer = std::make_unique<Frat_tracer>(path, true);
            break;
        case Options::Proof_format::alethe_stream:
        case Options::Proof_format::alethe_memory:
            throw std::runtime_error("Alethe proofs TBI");
        default:
            throw std::runtime_error("Unknown proof format");
        }
    }
}