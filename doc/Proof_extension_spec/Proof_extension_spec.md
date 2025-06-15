# Specifiation: Yaga UNSAT proof production extension
### Author: Kryštof Jungmann
### Document version: 1.0.0
*For revision history see the repository commit history.*

## 1. Basic information
### 1.1 Project description & focus
Yaga is a SMT solver based on the [MCSat](mcsat) paradigm. It is desirable for SMT solvers to be able to
pair an UNSAT answer with a proof that certifies the correctness and can be checked in a dedicated checker
program (which is algorithmically simpler, and thus easier to formally verify.)  
This project will extend Yaga with the capability to produce ([partial](8.1-preprocessing)) proofs 
in some specified format (most likely [Alethe](alethe), possibly also [FRAT](frat) or similar clausal format) 
in a way that allows support for other formats to be added in the future.
### 1.2 Technologies used
Inherited from Yaga - C++ and Catch2 testing framework.  
Outputs are to be validated using external checkers not shipped with the project.
### 1.3 References
<a name="mcsat">**MCSat**</a>:
Leonardo De Moura and Dejan Jovanovic. A model-constructing satisfiability calculus. In Verification, Model Checking, and Abstract Interpretation: 14th International Conference, VMCAI 2013, Rome, Italy, January 20-22, 2013. Proceedings 14, pages 1–12. Springer, 2013.  
<a name="alethe">**Alethe**</a>:
https://verit.loria.fr/documentation/alethe-spec.pdf  
<a name="frat">**FRAT**</a>:
S. Baek, M. Carneiro, M. Heule. "A Flexible Proof Format for SAT Solver-Elaborator Communication," in Logical Methods in Computer Science, vol. Volume 18, Issue 2, 2022.
### 1.4 Document conventions
The template has been (roughly) translated to English and rewritten in GitHub Flavored Markdown. 
Points not relevant to the project will be omitted (including the headings) without changing the numbering.

## 2. Brief description of the project
### 2.1 Project purpose, basic elements and goals
See [1.1](1.1-project-description-&-focus). The project will *extend* Yaga, modifying the current source code
to enable *optional* UNSAT proof generation.
### 2.2 Main functionality
Production of UNSAT proofs in an established format.
### 2.3 Usage example
The user runs Yaga on an unsatisfiable SMT instance, having also enabled proof production, set the output file path and specified the desired format.  
Yaga then produces the usual UNSAT response, while also producing a proof, which can then be checked using an external proof checker.
### 2.4 Runtime environment
Same as Yaga. 
No guarantees are made with respect to the external checkers required to interpret the produced proofs.
### 2.5 Project restrictions
Same as Yaga.
Formatting and naming should follow the established conventions from Yaga.

## 3. External interface
### 3.1 User interface, I/O
The implementation will extend Yaga to be able to output partial proofs to a file, possibly also to program output.  
Using command-line options, possibly also using relevant SMT-LIB commands, the user will be able to configure:
1. Whether to produce a proof
2. Output file path
3. Output proof format
### 3.3 Software interface
It will be necessary to modify `Clause` representation to include a unique ID, but other than that,
modification of existing code should be kept to a minimum to limit impact on performance when not generating proofs.  

The proof tracer(s) will communicate with the rest of Yaga through a `Tracer` interface
which will have methods corresponding to relevant events in the solver (e.g. conflict resolution, clause deletion...).
Conditional calls to those methods will be added to the appropriate spots in the core solver and plugins.  

It should be possible to implement both tracers that build up a proof tree in memory for later post-processing
and ones that simply stream a solver trace into a file.

## 4. Detailed functionality description
The tracer module will need to handle - and the rest of the solver will need to report - the following events:
1. **Solver start** - to take note of asserted clauses
2. **Boolean conflict** - reference a learned or asserted clause
3. **Theory conflict** - introduction of theory conflict clauses (which are tautologies) with correctness witnesses
   (Farkas certificate for LRA, none needed for UF)
4. **Conflict resolution** - including modelling the self-subsumption used in learned clause minimisation
   (This will likely require making the procedure slightly less efficient when producing proofs)
5. **Learned clause addition** - to finalise tracing the subproof
6. **Learned clause deletion**
7. **Proof finalisation** - to finish the proof and clean up

Adding unique IDs to clauses will be needed tie them to their representation in the proof for boolean conflict initialisation
and resolution, as well as to distinguish between active confilct clauses if the plugins are configured to produce more than one.

## 6. Extra-functional requirements
### 6.1 Performance
As touched upon in section [3.3](#3.3-software-interface), special care is to be taken to avoid / minimize performance
regressions when not producing proofs. Naturally, the proof production itself should also be reasonably performant.  
Benchmark results should be included in the follow-up thesis. 
If more than one proof format is supported, their performance impact, size and checking time should be compared.
### 6.4 Extensibility & Interoperability
The `Tracer` interface must be general enough to allow both for implementing new tracers to produce different formats
and for adding new types of events in the case of the solver's extension with new functionality.

## 8. Scope limitations
### 8.1 Preprocessing
The proofs will (most likely) not include preprocessing steps, instead starting by asserting the resulting CNF formula.  
### 8.2 Custom checking
There will be no custom checking or extensions of proof formats. Steps not supported by available checkers 
(e.g. all theory introductions for DRAT-like proofs) will remain unchecked.  
