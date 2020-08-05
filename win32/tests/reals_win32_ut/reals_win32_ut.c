// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "testrunnerswitcher.h"

#include "azure_c_pal/threadapi.h"
#include "azure_c_pal/srw_lock.h"
#include "azure_c_pal/string_utils.h"
#include "azure_c_pal/timer.h"
#include "azure_c_pal/interlocked_hl.h"
#include "azure_c_pal/interlocked.h"
#include "azure_c_pal/gballoc_ll.h"
#include "azure_c_pal/gballoc_hl.h"
#include "azure_c_pal/call_once.h"

#include "azure_macro_utils/macro_utils.h"

#ifdef REGISTER_GLOBAL_MOCK_HOOK
#undef REGISTER_GLOBAL_MOCK_HOOK
#endif 

#define REGISTER_GLOBAL_MOCK_HOOK(original, real) \
    (original == real) ? (void)0 : (void)1;

#include "real_threadapi.h"
#include "real_srw_lock.h"
#include "real_string_utils.h"
#include "real_timer.h"
#include "real_interlocked_hl.h"
#include "real_interlocked.h"
#include "real_gballoc_ll.h"
#include "real_gballoc_hl.h"
#include "real_call_once.h"

BEGIN_TEST_SUITE(reals_win32_ut)

// this test makes sure that the mappings work
// (there is a real_ function corresponding to the original)
TEST_FUNCTION(check_all_c_pal_reals)
{
    // arrange

    // act
    REGISTER_THREADAPI_GLOBAL_MOCK_HOOK();
    REGISTER_SRW_LOCK_GLOBAL_MOCK_HOOK();
    REGISTER_STRING_UTILS_GLOBAL_MOCK_HOOK();
    REGISTER_TIMER_GLOBAL_MOCK_HOOK();
    REGISTER_INTERLOCKED_HL_GLOBAL_MOCK_HOOK();
    REGISTER_INTERLOCKED_GLOBAL_MOCK_HOOK();
    REGISTER_GBALLOC_LL_GLOBAL_MOCK_HOOK();
    REGISTER_GBALLOC_HL_GLOBAL_MOCK_HOOK();
    REGISTER_CALLONCE_GLOBAL_MOCK_HOOK();

    // assert
    // no explicit assert, if it builds it works
}

END_TEST_SUITE(reals_win32_ut)
