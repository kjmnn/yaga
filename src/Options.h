#ifndef YAGA_OPTIONS_H
#define YAGA_OPTIONS_H

#include "Bool_phase.h"
#include <string>

namespace yaga {

/** Solver options parsed from the command line.
 */
struct Options {
    enum class Proof_format {
        frat_ascii, // ASCII FRAT
        frat_binary, // Binary FRAT
        alethe_stream, // Alethe streamed into a file (like FRAT)
        alethe_memory, // Alethe built and pruned in memory
    };

    /** If true, the LRA plugin will decide rational variables with only one allowed value first.
     *
     * For example, if 0 <= x and x <= 0 are on the trail, we will decide x before any other
     * variable.
     */
    bool prop_rational = false;

    /** If true, the LRA plugin will derive new bounds using Fourier-Motzkin elimination.
     */
    bool deduce_bounds = false;

    /** If true, the program will print solver counters like the number of conflicts.
     */
    bool print_stats = false;

    /** Value selection strategy for boolean variables.
     */
    Phase phase = Phase::positive;

    /** Input file path.
     */
    std::string input_path;

    /** If true, enable proof production.
     */
    bool produce_proofs = false;

    /** Produced proof format. 
     */
    Proof_format proof_format = Proof_format::frat_ascii;

    /** Proof file path (for tracers that output to a file)
     * 
     * If not set, the proof path will be derived from the input path by appending a format-specific suffix.
     */
    std::string proof_path;
};

} // namespace yaga

#endif // YAGA_OPTIONS_H