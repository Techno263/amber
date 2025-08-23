include(FindPackageHandleStandardArgs)

find_path(mica_INCLUDE_DIR
    NAMES mica/mica.hpp
)
mark_as_advanced(mica_INCLUDE_DIR)

find_package_handle_standard_args(mica
    REQUIRED_VARS
        mica_INCLUDE_DIR
)

if(mica_FOUND)
    set(mica_INCLUDE_DIRS "${mica_INCLUDE_DIR}")
    if(NOT TARGET mica::mica)
        add_library(mica::mica INTERFACE IMPORTED)
        set_target_properties(mica::mica
            PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${mica_INCLUDE_DIR}"
            INTERFACE_COMPILE_FEATURES "cxx_std_23"
        )
    endif()
endif()
