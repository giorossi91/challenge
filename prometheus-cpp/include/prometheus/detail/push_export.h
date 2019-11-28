
#ifndef PROMETHEUS_CPP_PUSH_EXPORT_H
#define PROMETHEUS_CPP_PUSH_EXPORT_H

#ifdef PROMETHEUS_CPP_PUSH_STATIC_DEFINE
#  define PROMETHEUS_CPP_PUSH_EXPORT
#  define PROMETHEUS_CPP_PUSH_NO_EXPORT
#else
#  ifndef PROMETHEUS_CPP_PUSH_EXPORT
#    ifdef PROMETHEUS_CPP_PUSH_EXPORTS
        /* We are building this library */
#      define PROMETHEUS_CPP_PUSH_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define PROMETHEUS_CPP_PUSH_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef PROMETHEUS_CPP_PUSH_NO_EXPORT
#    define PROMETHEUS_CPP_PUSH_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef PROMETHEUS_CPP_PUSH_DEPRECATED
#  define PROMETHEUS_CPP_PUSH_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef PROMETHEUS_CPP_PUSH_DEPRECATED_EXPORT
#  define PROMETHEUS_CPP_PUSH_DEPRECATED_EXPORT PROMETHEUS_CPP_PUSH_EXPORT PROMETHEUS_CPP_PUSH_DEPRECATED
#endif

#ifndef PROMETHEUS_CPP_PUSH_DEPRECATED_NO_EXPORT
#  define PROMETHEUS_CPP_PUSH_DEPRECATED_NO_EXPORT PROMETHEUS_CPP_PUSH_NO_EXPORT PROMETHEUS_CPP_PUSH_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef PROMETHEUS_CPP_PUSH_NO_DEPRECATED
#    define PROMETHEUS_CPP_PUSH_NO_DEPRECATED
#  endif
#endif

#endif
