/****************************************************************************/
/*	File   :             F:\Smoker\WiFiSmoke\trunk\src\include\qep\qassert.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/5/16,16:20:24                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#ifndef qassert_h
#define qassert_h

/** \ingroup qep qf qk qs
* \file qassert.h
* \brief Customizable assertions.
*
* Defines customizable and memory-efficient assertions applicable to
* embedded systems. This header file can be used in C, C++, and mixed C/C++
* programs.
*
* \note The preprocessor switch Q_NASSERT disables checking assertions.
* In particular macros \ref Q_ASSERT, \ref Q_REQUIRE, \ref Q_ENSURE,
* \ref Q_INVARIANT, and \ref Q_ERROR do NOT evaluate the test condition
* passed as the argument to these macros. One notable exception is the
* macro \ref Q_ALLEGE, that still evaluates the test condition, but does
* not report assertion failures when the switch Q_NASSERT is defined.
*/
#define Q_NASSERT

#ifdef Q_NASSERT          /* Q_NASSERT defined--assertion checking disabled */

    #define Q_DEFINE_THIS_FILE
    #define Q_DEFINE_THIS_MODULE(name_)
    #define Q_ASSERT(ignore_)  ((void)0)
    #define Q_ALLEGE(test_)    ((void)(test_))
    #define Q_ERROR()          ((void)0)

#else                  /* Q_NASSERT not defined--assertion checking enabled */

    #ifdef __cplusplus
        extern "C" {
    #endif

    /** callback invoked in case the condition passed to \ref Q_ASSERT,
    * \ref Q_REQUIRE, \ref Q_ENSURE, \ref Q_ERROR, or \ref Q_ALLEGE
    * evaluates to FALSE.
    *
    * \param file file name where the assertion failed
    * \param line line number at which the assertion failed
    */
    /*lint -sem(Q_assert_handler, r_no)    Q_assert_handler() never returns */
    void Q_assert_handler(char const Q_ROM *file, int line);

    #ifdef __cplusplus
        }
    #endif

    /** Place this macro at the top of each C/C++ module to define the file
    * name string using __FILE__ (NOTE: __FILE__ might contain lengthy path
    * name). This file name will be used in reporting assertions in this file.
    */
    #define Q_DEFINE_THIS_FILE \
        static char const Q_ROM l_this_file[] = __FILE__;

    /** Place this macro at the top of each C/C++ module to define the module
    * name as the argument \a name_. This file name will be used in reporting
    * assertions in this file.
    */
    #define Q_DEFINE_THIS_MODULE(name_) \
        static char const Q_ROM l_this_file[] = #name_;

    /** General purpose assertion that makes sure the \a test_ argument is
    * TRUE. Calls the Q_assert_handler() callback if the \a test_ evaluates
    * to FALSE.
    * \note the \a test_ is NOT evaluated if assertions are
    * disabled with the Q_NASSERT switch.
    */
    #define Q_ASSERT(test_) \
        if (test_) { \
        } \
        else (Q_assert_handler(l_this_file, __LINE__))

    /** General purpose assertion that ALWAYS evaluates the \a test_
    * argument and calls the Q_assert_handler() callback if the \a test_
    * evaluates to FALSE.
    * \note the \a test_ argument IS always evaluated even when assertions are
    * disabled with the Q_NASSERT macro. When the Q_NASSERT macro is
    * defined, the Q_assert_handler() callback is NOT called, even if the
    * \a test_ evaluates to FALSE.
    */
    #define Q_ALLEGE(test_)    Q_ASSERT(test_)

    /** Assertion that always calls the Q_assert_handler() callback if
    * ever executed.
    * \note can be disabled with the Q_NASSERT switch.
    */
    #define Q_ERROR() \
        (Q_assert_handler(l_this_file, __LINE__))

#endif                                                           /* NASSERT */

/** Assertion that checks for a precondition. This macro is equivalent to
* \ref Q_ASSERT, except the name provides a better documentation of the
* intention of this assertion.
*/
#define Q_REQUIRE(test_)   Q_ASSERT(test_)

/** Assertion that checks for a postcondition. This macro is equivalent to
* \ref Q_ASSERT, except the name provides a better documentation of the
* intention of this assertion.
*/
#define Q_ENSURE(test_)    Q_ASSERT(test_)

/** Assertion that checks for an invariant. This macro is equivalent to
* \ref Q_ASSERT, except the name provides a better documentation of the
* intention of this assertion.
*/
#define Q_INVARIANT(test_) Q_ASSERT(test_)

/** Compile-time assertion exploits the fact that in C/C++ a dimension of
 * an array must be non-zero. The following declaration causes a compilation
 * error if the compile-time expression (\a test_) is not TRUE. The assertion
 * has no runtime side effects.
 */
#define Q_ASSERT_COMPILE(test_) \
    extern char Q_assert_compile[(test_)]

#endif                                                         /* qassert_h */
