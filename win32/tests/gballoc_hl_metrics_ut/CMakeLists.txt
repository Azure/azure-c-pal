#Copyright (c) Microsoft. All rights reserved.

compileAsC11()
set(theseTestsName gballoc_hl_metrics_ut)

set(${theseTestsName}_test_files
    ${theseTestsName}.c
)

set(${theseTestsName}_c_files
    ../../src/gballoc_hl_metrics.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} "tests/c_pal/common" ADDITIONAL_LIBS pal_interfaces c_pal_reals)

if("${building}" STREQUAL "exe")
    set_target_properties(${theseTestsName}_exe PROPERTIES LINK_FLAGS "/ignore:4217")
endif()

if("${building}" STREQUAL "dll")
    set_target_properties(${theseTestsName}_dll PROPERTIES LINK_FLAGS "/ignore:4217")
endif()