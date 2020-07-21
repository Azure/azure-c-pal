// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

static void* stdlib_malloc(size_t size)
{
    return malloc(size);
}

static void* stdlib_calloc(size_t n, size_t s)
{
    return calloc(n,s);
}

static void* stdlib_realloc(void* p, size_t size)
{
    return realloc(p, size);
}

static void stdlib_free(void* ptr)
{
    free(ptr);
}


#include "azure_macro_utils/macro_utils.h"
#include "testrunnerswitcher.h"

static TEST_MUTEX_HANDLE g_testByTest;

#include "umock_c/umock_c.h"

#define ENABLE_MOCKS
#include "azure_c_pal/gballoc_ll.h"
#undef ENABLE_MOCKS

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

#include "azure_c_pal/gballoc_hl.h"

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error :%" PRI_MU_ENUM "", MU_ENUM_VALUE(UMOCK_C_ERROR_CODE, error_code));
}

BEGIN_TEST_SUITE(gballoc_hl_passthrough_ut)

TEST_SUITE_INITIALIZE(init_suite)
{
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    umock_c_init(on_umock_c_error);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_ll_malloc, stdlib_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_ll_realloc, stdlib_realloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_ll_calloc, stdlib_calloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_ll_free, stdlib_free);
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    umock_c_deinit();

    TEST_MUTEX_DESTROY(g_testByTest);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (TEST_MUTEX_ACQUIRE(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }

    umock_c_reset_all_calls();
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}

/*Tests_SRS_GBALLOC_HL_PASSTHROUGH_02_001: [ gballoc_hl_init shall call gballoc_ll_init(gballoc_ll_init_params). ]*/
/*Tests_SRS_GBALLOC_HL_PASSTHROUGH_02_002: [ gballoc_hl_init shall succeed and return 0. ]*/
TEST_FUNCTION(gballoc_hl_init_happy_path)
{
    ///arrange
    int result;

    STRICT_EXPECTED_CALL(gballoc_ll_init((void*)0x33))
        .SetReturn(0);

    ///act
    result = gballoc_hl_init(NULL, (void*)0x33);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///clean
    gballoc_hl_deinit();
}

/*Tests_SRS_GBALLOC_HL_PASSTHROUGH_02_003: [ If there are any failures then gballoc_hl_init shall fail and return a non-zero value. ]*/
TEST_FUNCTION(gballoc_hl_init_unhappy_path)
{
    ///arrange
    int result;

    STRICT_EXPECTED_CALL(gballoc_ll_init((void*)0x33))
        .SetReturn(1);

    ///act
    result = gballoc_hl_init(NULL, (void*)0x33);

    ///assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_GBALLOC_HL_PASSTHROUGH_02_004: [ gballoc_hl_deinit shall call gballoc_ll_deinit. ]*/
TEST_FUNCTION(gballoc_hl_deinit_succeds)
{
    ///arrange
    int result;
    STRICT_EXPECTED_CALL(gballoc_ll_init((void*)0x33))
        .SetReturn(1);
    result = gballoc_hl_init(NULL, (void*)0x33);
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(gballoc_ll_deinit());

    ///act
    gballoc_hl_deinit();

    ///assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_GBALLOC_HL_PASSTHROUGH_02_005: [ gballoc_hl_malloc shall call gballoc_ll_malloc(size) and return what gballoc_ll_malloc returned. ]*/
TEST_FUNCTION(gballoc_hl_malloc_succeeds)
{
    ///arrange
    void* result;
    STRICT_EXPECTED_CALL(gballoc_ll_malloc(3));

    ///act
    result = gballoc_hl_malloc(3);

    ///assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///clean
    gballoc_hl_free(result);
}

/*Tests_SRS_GBALLOC_HL_PASSTHROUGH_02_005: [ gballoc_hl_malloc shall call gballoc_ll_malloc(size) and return what gballoc_ll_malloc returned. ]*/
TEST_FUNCTION(gballoc_hl_malloc_unhappy_path)
{
    ///arrange
    void* result;
    STRICT_EXPECTED_CALL(gballoc_ll_malloc(3))
        .SetReturn(NULL);

    ///act
    result = gballoc_hl_malloc(3);

    ///assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///clean
}

/*Tests_SRS_GBALLOC_HL_PASSTHROUGH_02_006: [ gballoc_hl_free shall call gballoc_hl_free(ptr). ]*/
TEST_FUNCTION(gballoc_hl_free_with_NULL_succeeds)
{
    ///arrange
    STRICT_EXPECTED_CALL(gballoc_ll_free(NULL));

    ///act
    gballoc_hl_free(NULL);

    ///assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///clean
}

/*Tests_SRS_GBALLOC_HL_PASSTHROUGH_02_006: [ gballoc_hl_free shall call gballoc_hl_free(ptr). ]*/
TEST_FUNCTION(gballoc_hl_free_with_non_NULL_succeeds)
{
    ///arrange
    void* ptr = gballoc_hl_malloc(3);
    ASSERT_IS_NOT_NULL(ptr);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(gballoc_ll_free(ptr));

    ///act
    gballoc_hl_free(ptr);

    ///assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///clean
}

/*Tests_SRS_GBALLOC_HL_PASSTHROUGH_02_007: [ gballoc_hl_calloc shall call gballoc_ll_calloc(nmemb, size) and return what gballoc_ll_calloc returned. ]*/
TEST_FUNCTION(gballoc_ll_calloc_succeeds)
{
    ///arrange
    void* result;
    STRICT_EXPECTED_CALL(gballoc_ll_calloc(1, 2));

    ///act
    result = gballoc_hl_calloc(1, 2);

    ///assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///clean
    gballoc_hl_free(result);
}

/*Tests_SRS_GBALLOC_HL_PASSTHROUGH_02_007: [ gballoc_hl_calloc shall call gballoc_ll_calloc(nmemb, size) and return what gballoc_ll_calloc returned. ]*/
TEST_FUNCTION(gballoc_ll_calloc_when_ll_fails_it_fails)
{
    ///arrange
    void* result;
    STRICT_EXPECTED_CALL(gballoc_ll_calloc(1, 2))
        .SetReturn(NULL);

    ///act
    result = gballoc_hl_calloc(1, 2);

    ///assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///clean
}

/*Tests_SRS_GBALLOC_HL_PASSTHROUGH_02_008: [ gballoc_hl_realloc shall call gballoc_ll_realloc(ptr, size) and return what gballoc_ll_realloc returned. ]*/
TEST_FUNCTION(gballoc_hl_realloc_succeeds)
{
    ///arrange
    void* ptr = gballoc_hl_malloc(3);
    ASSERT_IS_NOT_NULL(ptr);
    umock_c_reset_all_calls();
    void* result;

    STRICT_EXPECTED_CALL(gballoc_ll_realloc(ptr, 10));

    ///act
    result = gballoc_hl_realloc(ptr, 10);

    ///assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///clean
    gballoc_hl_free(result);
}

/*Tests_SRS_GBALLOC_HL_PASSTHROUGH_02_008: [ gballoc_hl_realloc shall call gballoc_ll_realloc(ptr, size) and return what gballoc_ll_realloc returned. ]*/
TEST_FUNCTION(gballoc_hl_realloc_when_ll_fails)
{
    ///arrange
    void* ptr = gballoc_hl_malloc(3);
    ASSERT_IS_NOT_NULL(ptr);
    umock_c_reset_all_calls();
    void* result;

    STRICT_EXPECTED_CALL(gballoc_ll_realloc(ptr, 10))
        .SetReturn(NULL);

    ///act
    result = gballoc_hl_realloc(ptr, 10);

    ///assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///clean
    gballoc_hl_free(ptr);
}

END_TEST_SUITE(gballoc_hl_passthrough_ut)