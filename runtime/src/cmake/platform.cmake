if(UNIX)
    add_definitions(-D "RTCLI_TARGET_PLATFORM_UNIX")
    if(APPLE)
        add_definitions(-D "RTCLI_TARGET_PLATFORM_MACOS")
        set(RTCLI_PLATFORM "mac")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES ANDROID)
        add_definitions(-D "RTCLI_TARGET_PLATFORM_ANDROID")
        set(RTCLI_PLATFORM "android")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES Emscripten)
        add_definitions(-D "RTCLI_TARGET_PLATFORM_EMSCRIPTEN")
        set(RTCLI_PLATFORM "web")
    else(APPLE)
        add_definitions(-D "RTCLI_TARGET_PLATFORM_LINUX")
        set(RTCLI_PLATFORM "linux")
    endif(APPLE)
elseif(WIN32)
    add_definitions(-D "RTCLI_TARGET_PLATFORM_WIN")
    set(RTCLI_PLATFORM "windows")
elseif(__COMPILER_PS5)
    set(RTCLI_PLATFORM "prospero")
    add_definitions(-D "RTCLI_TARGET_PLATFORM_PLAYSTATION")
endif(UNIX)

