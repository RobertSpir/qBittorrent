include(MacroConfigureMSVCRuntime)
set(MSVC_RUNTIME "dynamic")
configure_msvc_runtime()
link_libraries(iphlpapi)