#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Smt2_parser.h"
#include "Options.h"

using namespace yaga;

void print_help()
{
    std::cerr << "Usage: ./smt [options] [input-path.smt2]" << std::endl;
    std::cerr << "Options:\n";
    std::cerr << "   --print-stats: print solver counters like the number of conflicts.\n";
    std::cerr << "   --prop-rational: decide rational variables with only one allowed value first.\n";
    std::cerr << "   --deduce-bounds: derive new bounds in LRA using Fourier-Motzkin elimination.\n";
    std::cerr << "   --phase [positive|negative|cache]: value selection strategy for Boolean variables.\n";
    // TODO: add help for proof production options
}

int main(int argc, char** argv)
{
    Options options;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg{argv[i]};
        if (arg == "--prop-rational")
        {
            options.prop_rational = true;
        }
        else if (arg == "--deduce-bounds")
        {
            options.deduce_bounds = true;
        }
        else if (arg == "--print-stats")
        {
            options.print_stats = true;
        }
        else if (arg == "--phase")
        {
            if (i + 1 < argc)
            {
                std::string value{argv[++i]};
                if (value == "positive")
                {
                    options.phase = Phase::positive;
                }
                else if (value == "negative")
                {
                    options.phase = Phase::negative;
                }
                else if (value == "cache")
                {
                    options.phase = Phase::cache;
                }
            }
        }
        else if (arg == "--frat")
        {
            std::cerr << "Producing FRAT" << std::endl; // TODO remove
            options.produce_proofs = true;
            options.proof_format = Options::Proof_format::frat_ascii;
        }
        else if (arg.starts_with("-"))
        {
            std::cerr << "Unrecognized option: '" << arg << "'\n";
            print_help();
            return -1;
        } 
        else
        {
            options.input_path = arg;
        }
    }

    if (options.input_path.empty())
    {
        print_help();
        return -1;
    }

    try
    {
        parser::Smt2_parser parser;
        parser.set_options(options);
        parser.parse_file(options.input_path);
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "Error: failed to open the input file '" << options.input_path << "'\n";
    }
    catch (std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << "\n";
    }
}