add_library(activity_diagram
    node.h
    principal.h
    principal.cpp
    status_decision.h
    loop_action.h
    nested_activity_diagram.h
    )
target_include_directories(activity_diagram PUBLIC ${rxcpp_SOURCE_DIR}/Rx/v2/src INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(activity_diagram ${Boost_LIBRARIES})
# target_compile_definitions(activity_diagram PUBLIC BOOST_ALL_DYN_LINK)