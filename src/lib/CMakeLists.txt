add_library(blueprint
#    Blueprint.cpp
#    Blueprint.h
    step.h
    step.cpp
    principal.h
    principal.cpp
    statusdispatcher.h
    loopactionstep.h
    composite_action_step.h
    )
target_include_directories(blueprint PUBLIC ${rxcpp_SOURCE_DIR}/Rx/v2/src INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(blueprint ${Boost_LIBRARIES})
