// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#include <cstddef>
#else
#include <stdlib.h>
#include <stddef.h>
#endif

#include "macro_utils/macro_utils.h"
#include "testrunnerswitcher.h"

static TEST_MUTEX_HANDLE g_testByTest;

#include "c_pal/gballoc_hl.h"

BEGIN_TEST_SUITE(TEST_SUITE_NAME_FROM_CMAKE)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    ASSERT_ARE_EQUAL(int, 0, gballoc_hl_init(NULL, NULL));
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    gballoc_hl_deinit();

    TEST_MUTEX_DESTROY(g_testByTest);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (TEST_MUTEX_ACQUIRE(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}


TEST_FUNCTION(gballoc_hl_init_works)
{
    ///arrange
    gballoc_hl_deinit();

    int result;

    ///act
    result = gballoc_hl_init(NULL, NULL);

    ///assert - doesn't crash
    ASSERT_ARE_EQUAL(int, 0, result);
}

TEST_FUNCTION(gballoc_hl_deinit_works)
{
    ///act
    gballoc_hl_deinit();

    ///assert - doesn't crash

    ///clean
    ASSERT_ARE_EQUAL(int, 0, gballoc_hl_init(NULL, NULL));
}

TEST_FUNCTION(gballoc_hl_malloc_works)
{
    ///act (1)
    unsigned char* ptr = (unsigned char*)gballoc_hl_malloc(1);

    ///assert (1)
    ASSERT_IS_NOT_NULL(ptr);

    ///act(2)
    ptr[0] = '3'; /*can be written*/

    ///assert (2) - doesn't crash

    ///clean
    gballoc_hl_free(ptr);
}

TEST_FUNCTION(gballoc_hl_malloc_1MB_works)
{
    ///act (1)
    unsigned char* ptr = (unsigned char*)gballoc_hl_malloc(1024 * 1024);

    ///assert (1)
    ASSERT_IS_NOT_NULL(ptr);

    ///act(2)
    ptr[0] = '3'; /*can be written*/

    ///assert (2) - doesn't crash

    ///clean
    gballoc_hl_free(ptr);
}

TEST_FUNCTION(gballoc_hl_free_works)
{
    ///arrange
    unsigned char* ptr = (unsigned char*)gballoc_hl_malloc(1);
    ASSERT_IS_NOT_NULL(ptr);

    ///act 
    gballoc_hl_free(ptr);

    ///assert - doesn't crash
}

TEST_FUNCTION(gballoc_hl_realloc_works)
{
    ///arrange
    unsigned char* ptr1 = (unsigned char*)gballoc_hl_malloc(1);
    ASSERT_IS_NOT_NULL(ptr1);
    unsigned char* ptr2;

    ///act 
    ptr2 = (unsigned char*)gballoc_hl_realloc(ptr1, 2);

    ///assert - doesn't crash
    ASSERT_IS_NOT_NULL(ptr2);

    ///clean
    gballoc_hl_free(ptr2);
}

TEST_FUNCTION(gballoc_hl_calloc_works)
{
    ///arrange
    unsigned char* ptr;

    ///act 
    ptr = (unsigned char*)gballoc_hl_calloc(1, 1);

    ///assert - doesn't crash
    ASSERT_IS_NOT_NULL(ptr);
    ASSERT_IS_TRUE(0 == ptr[0]);

    ///clean
    gballoc_hl_free(ptr);
}

END_TEST_SUITE(TEST_SUITE_NAME_FROM_CMAKE)
