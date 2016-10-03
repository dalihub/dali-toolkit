#ifndef CSHARP_COMMON
#define CSHARP_COMMON
#endif

#define SWIG_DIRECTORS

#include <stdexcept>

#define SWIGSTDCALL

/* attribute recognised by some compilers to avoid 'unused' warnings */
#ifndef SWIGUNUSED
# if defined(__GNUC__)
#   if !(defined(__cplusplus)) || (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#     define SWIGUNUSED __attribute__ ((__unused__))
#   else
#     define SWIGUNUSED
#   endif
# elif defined(__ICC)
#   define SWIGUNUSED __attribute__ ((__unused__))
# else
#   define SWIGUNUSED
# endif
#endif

/* internal SWIG method */
#ifndef SWIGINTERN
# define SWIGINTERN static SWIGUNUSED
#endif

#ifndef SWIGEXPORT
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   if defined(STATIC_LINKED)
#     define SWIGEXPORT
#   else
#     define SWIGEXPORT __declspec(dllexport)
#   endif
# else
#   if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#     define SWIGEXPORT __attribute__ ((visibility("default")))
#   else
#     define SWIGEXPORT
#   endif
# endif
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*  Errors in SWIG */
#define  SWIG_UnknownError         -1
#define  SWIG_IOError            -2
#define  SWIG_RuntimeError       -3
#define  SWIG_IndexError         -4
#define  SWIG_TypeError          -5
#define  SWIG_DivisionByZero     -6
#define  SWIG_OverflowError      -7
#define  SWIG_SyntaxError        -8
#define  SWIG_ValueError         -9
#define  SWIG_SystemError        -10
#define  SWIG_AttributeError     -11
#define  SWIG_MemoryError        -12
#define  SWIG_NullReferenceError   -13

/* Support for throwing C# exceptions from C/C++. There are two types:
 * Exceptions that take a message and ArgumentExceptions that take a message and a parameter name. */
typedef enum {
  SWIG_CSharpApplicationException,
  SWIG_CSharpArithmeticException,
  SWIG_CSharpDivideByZeroException,
  SWIG_CSharpIndexOutOfRangeException,
  SWIG_CSharpInvalidCastException,
  SWIG_CSharpInvalidOperationException,
  SWIG_CSharpIOException,
  SWIG_CSharpNullReferenceException,
  SWIG_CSharpOutOfMemoryException,
  SWIG_CSharpOverflowException,
  SWIG_CSharpSystemException
} SWIG_CSharpExceptionCodes;

typedef enum {
  SWIG_CSharpArgumentException,
  SWIG_CSharpArgumentNullException,
  SWIG_CSharpArgumentOutOfRangeException
} SWIG_CSharpExceptionArgumentCodes;

typedef void (SWIGSTDCALL* SWIG_CSharpExceptionCallback_t)(const char *);
typedef void (SWIGSTDCALL* SWIG_CSharpExceptionArgumentCallback_t)(const char *, const char *);

typedef struct {
  SWIG_CSharpExceptionCodes code;
  SWIG_CSharpExceptionCallback_t callback;
} SWIG_CSharpException_t;

typedef struct {
  SWIG_CSharpExceptionArgumentCodes code;
  SWIG_CSharpExceptionArgumentCallback_t callback;
} SWIG_CSharpExceptionArgument_t;

static SWIG_CSharpException_t SWIG_csharp_exceptions[] = {
  { SWIG_CSharpApplicationException, NULL },
  { SWIG_CSharpArithmeticException, NULL },
  { SWIG_CSharpDivideByZeroException, NULL },
  { SWIG_CSharpIndexOutOfRangeException, NULL },
  { SWIG_CSharpInvalidCastException, NULL },
  { SWIG_CSharpInvalidOperationException, NULL },
  { SWIG_CSharpIOException, NULL },
  { SWIG_CSharpNullReferenceException, NULL },
  { SWIG_CSharpOutOfMemoryException, NULL },
  { SWIG_CSharpOverflowException, NULL },
  { SWIG_CSharpSystemException, NULL }
};

static SWIG_CSharpExceptionArgument_t SWIG_csharp_exceptions_argument[] = {
  { SWIG_CSharpArgumentException, NULL },
  { SWIG_CSharpArgumentNullException, NULL },
  { SWIG_CSharpArgumentOutOfRangeException, NULL }
};

static void SWIGUNUSED SWIG_CSharpSetPendingException(SWIG_CSharpExceptionCodes code, const char *msg) {
  SWIG_CSharpExceptionCallback_t callback = SWIG_csharp_exceptions[SWIG_CSharpApplicationException].callback;
  if ((size_t)code < sizeof(SWIG_csharp_exceptions)/sizeof(SWIG_CSharpException_t)) {
    callback = SWIG_csharp_exceptions[code].callback;
  }
  callback(msg);
}

static void SWIGUNUSED SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpExceptionArgumentCodes code, const char *msg, const char *param_name) {
  SWIG_CSharpExceptionArgumentCallback_t callback = SWIG_csharp_exceptions_argument[SWIG_CSharpArgumentException].callback;
  if ((size_t)code < sizeof(SWIG_csharp_exceptions_argument)/sizeof(SWIG_CSharpExceptionArgument_t)) {
    callback = SWIG_csharp_exceptions_argument[code].callback;
  }
  callback(msg, param_name);
}

SWIGINTERN void SWIG_CSharpException(int code, const char *msg) {
  if (code == SWIG_ValueError) {
    SWIG_CSharpExceptionArgumentCodes exception_code = SWIG_CSharpArgumentOutOfRangeException;
    SWIG_CSharpSetPendingExceptionArgument(exception_code, msg, 0);
  } else {
    SWIG_CSharpExceptionCodes exception_code = SWIG_CSharpApplicationException;
    switch(code) {
    case SWIG_MemoryError:
      exception_code = SWIG_CSharpOutOfMemoryException;
      break;
    case SWIG_IndexError:
      exception_code = SWIG_CSharpIndexOutOfRangeException;
      break;
    case SWIG_DivisionByZero:
      exception_code = SWIG_CSharpDivideByZeroException;
      break;
    case SWIG_IOError:
      exception_code = SWIG_CSharpIOException;
      break;
    case SWIG_OverflowError:
      exception_code = SWIG_CSharpOverflowException;
      break;
    case SWIG_RuntimeError:
    case SWIG_TypeError:
    case SWIG_SyntaxError:
    case SWIG_SystemError:
    case SWIG_UnknownError:
    default:
      exception_code = SWIG_CSharpApplicationException;
      break;
    }
    SWIG_CSharpSetPendingException(exception_code, msg);
  }
}


#include <stdexcept>


#define SWIGSTDCALL


#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;


