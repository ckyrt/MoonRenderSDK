#pragma once

#if defined(_WIN32)
#    if defined(MOONRENDER_BUILD_DLL)
#        define MOONRENDER_API __declspec(dllexport)
#    else
#        define MOONRENDER_API __declspec(dllimport)
#    endif
#else
#    define MOONRENDER_API
#endif
