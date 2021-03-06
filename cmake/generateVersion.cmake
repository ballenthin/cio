function(GenerateVersion VERSION_FILE)
    file(STRINGS ${VERSION_FILE} VERSION_STRING)
    string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)
    set(${PROJECT_NAME_UC}_VERSION ${VERSION_STRING} PARENT_SCOPE)

    string(REPLACE "." ";" VERSION_LIST ${VERSION_STRING})

    list(GET VERSION_LIST 0 VERSION_MAJOR)
    list(GET VERSION_LIST 1 VERSION_MINOR)
    list(GET VERSION_LIST 2 VERSION_PATCH)

    set(PROJECT_VERSION_MAJOR ${VERSION_MAJOR} PARENT_SCOPE)
    set(PROJECT_VERSION_MINOR ${VERSION_MINOR} PARENT_SCOPE)
    set(PROJECT_VERSION_PATCH ${VERSION_PATCH} PARENT_SCOPE)

    execute_process(
        COMMAND git diff --shortstat
        COMMAND tail -n1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_DIRTY
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    execute_process(
        COMMAND git describe --exact-match --tags HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE IS_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        OUTPUT_QUIET
    )

    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if((IS_TAG EQUAL 0) AND (GIT_DIRTY STREQUAL ""))
        set(${PROJECTNAME}_VERSION_TWEAK "")
    else()
        execute_process(
            COMMAND git rev-list HEAD --count
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_REV_COUNT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(GIT_REV_COUNT STREQUAL "")
            set(${PROJECTNAME}_VERSION_TWEAK "-unknown")
        else()
            set(${PROJECTNAME}_VERSION_TWEAK "-${GIT_REV_COUNT}")
        endif()
    endif()
    
    if(GIT_DIRTY STREQUAL "")
        set(${PROJECTNAME}_VERSION_DIRTY "")
    else()
        set(${PROJECTNAME}_VERSION_DIRTY ".dirty")
    endif()

    set(${PROJECTNAME}_BUILDINFO "+${GIT_HASH}${${PROJECTNAME}_VERSION_DIRTY}")
    set(${PROJECT_NAME_UC}_LAST ${${PROJECTNAME}_VERSION_TWEAK}${${PROJECTNAME}_BUILDINFO} PARENT_SCOPE)
endfunction(GenerateVersion)
