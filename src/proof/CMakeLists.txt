target_include_directories(yaga PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(yaga PRIVATE
    Frat_tracer.cpp
    Tracer_wrapper.cpp
)
