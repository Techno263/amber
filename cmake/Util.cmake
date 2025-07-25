# Returns the last character of str. Returns result in the value of result_var
function(last_char str result_var)
    block(SCOPE_FOR VARIABLES)
        set(str_len 0)
        string(LENGTH ${str} str_len)
        set(last_char_index)
        math(EXPR last_char_index "${str_len} - 1" OUTPUT_FORMAT DECIMAL)
        string(SUBSTRING ${str} ${last_char_index} 1 ${result_var})
        return(PROPAGATE ${result_var})
    endblock()
endfunction()

# Prepends path_prefix to all elements in paths. Returns result in the value of result_var
function(prepend_paths paths path_prefix result_var)
    block(SCOPE_FOR VARIABLES)
        last_char(${path_prefix} prefix_last_char)
        if(NOT ${prefix_last_char} STREQUAL "/")
            string(APPEND path_prefix "/")
        endif()
        set(${result_var} "")
        foreach(path IN LISTS paths)
            string(STRIP ${path} path)
            string(PREPEND path ${path_prefix})
            list(APPEND ${result_var} ${path})
        endforeach()
        return(PROPAGATE ${result_var})
    endblock()
endfunction()
