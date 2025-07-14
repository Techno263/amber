include(FindPackageHandleStandardArgs)

find_path(Catch2_INCLUDE_DIR
    NAMES catch2/catch_test_macros.hpp
)
mark_as_advanced(Catch2_INCLUDE_DIR)

set(_catch2_lib_name "Catch2")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(_catch2_lib_name "${_catch2_lib_name}d")
endif()
find_library(Catch2_Catch2_LIBRARY
    NAMES "${_catch2_lib_name}"
    NAMES_PER_DIR
)
unset(_catch2_lib_name)
mark_as_advanced(Catch2_Catch2_LIBRARY)

set(_catch2main_lib_name "Catch2Main")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(_catch2main_lib_name "${_catch2main_lib_name}d")
endif()
find_library(Catch2_Catch2Main_LIBRARY
    NAMES "${_catch2main_lib_name}"
    NAMES_PER_DIR
)
unset(_catch2main_lib_name)
mark_as_advanced(Catch2_Catch2Main_LIBRARY)

foreach(_comp IN LISTS Catch2_FIND_COMPONENTS)
    if(_comp STREQUAL "Catch2")
        if(Catch2_INCLUDE_DIR
            AND EXISTS "${Catch2_INCLUDE_DIR}"
            AND Catch2_Catch2_LIBRARY
            AND EXISTS "${Catch2_Catch2_LIBRARY}"
        )
            set("Catch2_${_comp}_FOUND" TRUE)
        else()
            set("Catch2_${_comp}_FOUND" FALSE)
        endif()
    elseif(_comp STREQUAL "Catch2Main")
        if(Catch2_INCLUDE_DIR
            AND EXISTS "${Catch2_INCLUDE_DIR}"
            AND Catch2_Catch2_LIBRARY
            AND EXISTS "${Catch2_Catch2_LIBRARY}"
            AND Catch2_Catch2Main_LIBRARY
            AND EXISTS "${Catch2_Catch2Main_LIBRARY}"
        )
            set("Catch2_${_comp}_FOUND" TRUE)
        else()
            set("Catch2_${_comp}_FOUND" FALSE)
        endif()
    else()
        message(WARNING "${_comp} is not a valid Catch2 component")
        set("Catch2_${_comp}_FOUND" FALSE)
    endif()
endforeach()
unset(_comp)

find_package_handle_standard_args(Catch2
    REQUIRED_VARS
        Catch2_INCLUDE_DIR
        Catch2_Catch2_LIBRARY
    HANDLE_COMPONENTS
)

if(Catch2_FOUND)
    set(Catch2_INCLUDE_DIRS "${Catch2_INCLUDE_DIR}")
    set(Catch2_Catch2_LIBRARIES "${Catch2_Catch2_LIBRARY}")
    set(Catch2_LIBRARIES "${Catch2_Catch2_LIBRARY}")
    if(NOT TARGET Catch2::Catch2)
        add_library(Catch2::Catch2 UNKNOWN IMPORTED)
        set_target_properties(Catch2::Catch2
            PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${Catch2_INCLUDE_DIR}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
            IMPORTED_LOCATION "${Catch2_Catch2_LIBRARY}"
        )
    endif()
    if(Catch2_Catch2Main_FOUND)
        set(Catch2_Catch2Main_LIBRARIES "${Catch2_Catch2Main_LIBRARY}")
        set(Catch2_LIBRARIES ${Catch2_LIBRARIES} "${Catch2_Catch2Main_LIBRARY}")
        if(NOT TARGET Catch2::Catch2Main)
            add_library(Catch2::Catch2Main UNKNOWN IMPORTED)
            set_target_properties(Catch2::Catch2Main
                PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${Catch2_INCLUDE_DIR}"
                IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
                IMPORTED_LOCATION "${Catch2_Catch2Main_LIBRARY}"
                INTERFACE_LINK_LIBRARIES Catch2::Catch2
            )
        endif()
    endif()
endif()
