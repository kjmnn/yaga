#ifndef YAGA_BOOL_PHASE_H
#define YAGA_BOOL_PHASE_H

namespace yaga {

enum class Phase {
    /** Always decide true for boolean variables.
     */
    positive,

    /** Always decide false for boolean variables.
     */
    negative,

    /** Cache values of boolean variables.
     */
    cache,
};

}

#endif