if (STACKTRACE)
    if ("${WINXXBITS}" STREQUAL "Win64")
        add_compile_options(-Zi)
    else ("${WINXXBITS}" STREQUAL "Win64")
        # i686 arch requires frame pointer preservation
        add_compile_options(-Oy-)
    endif ("${WINXXBITS}" STREQUAL "Win64")
    link_libraries(dbghelp.lib)
endif (STACKTRACE)

# Enable Wide characters
add_definitions(-DTORRENT_USE_WPATH)

include(MacroConfigureMSVCRuntime)
set(MSVC_RUNTIME "dynamic")
configure_msvc_runtime()
