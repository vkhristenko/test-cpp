function(testcpp_print msg)
    message(STATUS "${msg}")
endfunction()

function(testcpp_debug_var var) 
    testcpp_print("${var} = ${${var}}")
endfunction()

function(testcpp_fail_here)
    message(FATAL_ERROR "Stop execution!")
endfunction()

function(testcpp_fail_with_error msg)
    message(FATAL_ERROR ${msg})
endfunction()

function(testcpp_fail_if_undefined var)
    if (NOT DEFINED ${var})
        testcpp_fail_with_error("variable ${var} is not defined")
    endif()
endfunction()
