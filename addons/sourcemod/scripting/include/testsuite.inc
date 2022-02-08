/**
 * vim: set ts=4 :
 * =============================================================================
 * sm-testsuite
 * A pure SourcePawn implementation of basic testing and assertions.
 * https://github.com/clugg/sm-testsuite
 *
 * sm-testsuite (C)2020 James Dickens. (clug)
 * SourceMod (C)2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 */

#if defined _testsuite_included
 #endinput
#endif
#define _testsuite_included

#define _TEST_PROFILER_ENABLED SOURCEMOD_V_MAJOR >= 1 && SOURCEMOD_V_MINOR >= 10
#define _TEST_FUNCTION_WORKAROUND SOURCEMOD_V_MAJOR >= 1 && SOURCEMOD_V_MINOR >= 11

#if _TEST_PROFILER_ENABLED
#include <profiler>

static Profiler __test_profiler = null;
#endif

/**
 * @section Output
 */

#define TEST_PASS_SYMBOL "✓"
#define TEST_FAIL_SYMBOL "✗"

static int __test_boxWidth = 64;

/**
 * Sets the width of the box rendered for sections.
 *
 * @param width     Width of box.
 */
stock void Test_SetBoxWidth(int width)
{
    if (width <= 0) {
        return;
    }

    __test_boxWidth = width;
}

/**
 * Calculates the visible length of a string, taking into account
 * multibyte Unicode symbols.
 *
 * @param str       String to check.
 * @returns     The number of visible characters in the string.
 */
stock int Test_MBStrlen(const char[] str)
{
    int length = strlen(str);
    int visible_length = length;
    int size;
    for (int i = 0; i < length; i += 1) {
        if ((str[i] & 0x80) != 0) {
            if (length - i < 2) {
                continue;
            }

            if ((str[i + 1] & 0xC0) != 0x80) {
                continue;
            }

            if ((str[i] & 0xE0) == 0xE0) {
                if (length - i < 3) {
                    continue;
                }

                if ((str[i + 2] & 0xC0) != 0x80) {
                    continue;
                }

                if ((str[i] & 0xF0) == 0xF0) {
                    if (length - i < 4) {
                        continue;
                    }

                    if (
                        (str[i] & 0xF8) != 0xF0
                        || (str[i + 3] & 0xC0) != 0x80
                    ) {
                        continue;
                    }

                    size = 4;
                } else {
                    size = 3;
                }
            } else {
                size = 2;
            }
        } else {
            size = 1;
        }

        i += size - 1;
        visible_length -= size - 1;
    }

    return visible_length;
}

/**
 * Fits the message to a box and outputs to server.
 *
 * @param message   Message to output.
 * @param ...       Further arguments to pass to message formatter.
 */
stock void Test_Print(const char[] message, any ...)
{
    char output[1024];
    VFormat(output, sizeof(output), message, 2);

    // fit output to a box
    int length = strlen(output);
    int extra = length - Test_MBStrlen(output);
    int end = __test_boxWidth - 4 + extra;
    if (length < end) {
        int i;
        for (i = length; i < end; i += 1) {
            output[i] = ' ';
        }
        output[i] = '\0';
    }

    PrintToServer("| %s |", output);
}

/**
 * Aligns the message to the center of the box and outputs to server.
 *
 * @param message   Message to output.
 * @param ...       Further arguments to pass to message formatter.
 */
stock void Test_PrintCentered(const char[] message, any...)
{
    char output[1024];
    VFormat(output, sizeof(output), message, 2);

    // left pad output
    int len = Test_MBStrlen(output);
    int spaceAmount = RoundToFloor(((__test_boxWidth - 4) / 2.0) - (len / 2.0));
    if (spaceAmount > 0) {
        char[] spaces = new char[spaceAmount + 1];
        int i;
        for (i = 0; i < spaceAmount; i += 1) {
            spaces[i] = ' ';
        }
        spaces[i] = '\0';

        Format(output, sizeof(output), "%s%s", spaces, output);
    }

    Test_Print("%s", output);
}

/**
 * Prints a box-width separator.
 *
 * @param separator     Separator to use.
 */
stock void Test_PrintSeparator(int separator = '-')
{
    int max_size = __test_boxWidth + 1;
    char[] output = new char[max_size];

    int i;
    for (i = 0; i < __test_boxWidth - 2; i += 1) {
        output[i] = separator;
    }
    output[i] = '\0';

    PrintToServer("|%s|", output);
}


/**
 * @section Sections
 */

static int __test_testsTotal = 0;
static int __test_testsPassed = 0;

/**
 * Resets the test counters to 0 and outputs the section name to the server.
 * If profiling is enabled, starts the profiler.
 *
 * @param name  Name of section.
 */
stock void Test_StartSection(const char[] name)
{
    __test_testsTotal = 0;
    __test_testsPassed = 0;

    Test_PrintSeparator();
    Test_PrintCentered("%s", name);
    Test_PrintSeparator();

    #if _TEST_PROFILER_ENABLED
    if (__test_profiler == null) {
        __test_profiler = new Profiler();
    }

    __test_profiler.Start();
    #endif
}

/**
 * Outputs the amount of tests passed and failed to the server.
 * If profiling is enabled, stops the profiler and outputs the time taken.
 *
 * @param name  Name of section.
 */
stock void Test_EndSection()
{
    #if _TEST_PROFILER_ENABLED
    __test_profiler.Stop();
    #endif

    char output[1024];
    if (__test_testsTotal > 0) {
        strcopy(output, sizeof(output), "Tests: ");

        int testsFailed = __test_testsTotal - __test_testsPassed;

        if (__test_testsPassed > 0) {
            Format(
                output,
                sizeof(output),
                "%s%d passed",
                output,
                __test_testsPassed
            );

            if (testsFailed > 0) {
                StrCat(output, sizeof(output), " / ");
            }
        }

        if (testsFailed > 0) {
            Format(
                output,
                sizeof(output),
                "%s%d failed",
                output,
                testsFailed
            );
        }
    } else {
        strcopy(output, sizeof(output), "No tests run!");
    }

    Test_Print("%s", output);
    #if _TEST_PROFILER_ENABLED
    Test_Print("Time:  %fs", __test_profiler.Time);
    #endif
    Test_PrintSeparator();
    PrintToServer("");
}

/**
 * @section Running Tests
 */

typedef Test_Method = function void();

static int __test_assertionsTotal = 0;
static int __test_assertionsPassed = 0;

/**
 * Prepares the assert counts and prints the test name to server.
 *
 * @param name  Name of test.
 */
stock void Test_BeforeRun(const char[] name)
{
    __test_assertionsTotal = 0;
    __test_assertionsPassed = 0;

    Test_Print("");
    Test_Print("%s", name);
}

/**
 * Calculates and outputs the test result to server.
 *
 * @returns     True if the test passed, false otherwise.
 */
stock bool Test_AfterRun()
{
    bool passed = Test_IsPassing();
    __test_testsTotal += 1;
    if (passed) {
        __test_testsPassed += 1;
    }

    if (__test_assertionsTotal > 0) {
        char output[1024];
        strcopy(output, sizeof(output), "Assertions: ");

        int assertionsFailed = __test_assertionsTotal - __test_assertionsPassed;

        if (__test_assertionsPassed > 0) {
            Format(
                output,
                sizeof(output),
                "%s%d passed",
                output,
                __test_assertionsPassed
            );

            if (assertionsFailed > 0) {
                StrCat(output, sizeof(output), " / ");
            }
        }

        if (assertionsFailed > 0) {
            Format(
                output,
                sizeof(output),
                "%s%d failed",
                output,
                assertionsFailed
            );
        }

        Test_Print("%s", output);
    }

    Test_Print(
        "%s %s",
        passed ? TEST_PASS_SYMBOL : TEST_FAIL_SYMBOL,
        passed ? "PASS" : "FAIL"
    );
    Test_Print("");
    Test_PrintSeparator();

    return passed;
}

/**
 * Performs pre-test operations, executes the test then
 * performs post-test operations.
 *
 * @param name      Name of test.
 * @param method    Method to execute.
 * @returns         True if all test assertions passed, false otherwise.
 */
stock bool Test_Run(const char[] name, Test_Method method)
{
    Test_BeforeRun(name);

    Call_StartFunction(null, method);
    Call_Finish();

    return Test_AfterRun();
}

/**
 * Works the same as Test_Run but accepts a less strictly typed method,
 * which SM1.11 seems to require when using GetFunctionByName.
 *
 * @param name      Name of test.
 * @param method    Method to execute.
 * @returns         True if all test assertions passed, false otherwise.
 */
stock bool Test_RunUnsafe(const char[] name, Function method)
{
    Test_BeforeRun(name);

    Call_StartFunction(null, method);
    Call_Finish();

    return Test_AfterRun();
}

/**
 * Resolves the function based on the provided name and executes Test_Run.
 *
 * @param name  Name of test to run.
 * @returns     True if all test assertions passed, false otherwise.
 */
stock bool Test_RunFromString(const char[] name)
{
    Function method = GetFunctionByName(null, name);

    #if _TEST_FUNCTION_WORKAROUND
    return Test_RunUnsafe(name, method);
    #else
    return Test_Run(name, view_as<Test_Method>(method));
    #endif
}

/**
 * Determines whether or not the currently running test is passing.
 *
 * @returns     True if the test is passing, false otherwise.
 */
stock bool Test_IsPassing()
{
    return __test_assertionsPassed == __test_assertionsTotal;
}

/**
 * @section Assertions
 */

/**
 * Outputs the message to server.
 *
 * @param message   Message to output.
 * @param ...       Further arguments to pass to message formatter.
 */
stock void Test_Output(const char[] message, any ...)
{
    char output[1024];
    VFormat(output, sizeof(output), message, 2);
    Test_Print("  i %s", output);
}

/**
 * Adds to the assert counters and outputs the assertion result to server.
 *
 * @param bool      Result of the assertion.
 * @param message   Message to output.
 * @param ...       Further arguments to pass to message formatter.
 * @returns         The result passed in.
 */
stock bool Test_OutputAssertResult(bool result, const char[] message, any ...)
{
    __test_assertionsTotal += 1;
    if (result) {
        __test_assertionsPassed += 1;
    }

    char output[1024];
    VFormat(output, sizeof(output), message, 3);
    Test_Print(
        "  %s %s",
        result ? TEST_PASS_SYMBOL : TEST_FAIL_SYMBOL,
        output
    );

    return result;
}

/**
 * Asserts that the value is truthy.
 *
 * @param name  Name to output in assertion result.
 * @param x     Value to compare.
 * @returns     True if the value is truthy, false otherwise.
 */
stock bool Test_Assert(const char[] name, any x)
{
    return Test_OutputAssertResult(!!x, "%s", name);
}

/**
 * Asserts that the value is true.
 *
 * @param name  Name to output in assertion result.
 * @param x     Value to compare.
 * @returns     True if the value is true, false otherwise.
 */
stock bool Test_AssertTrue(const char[] name, any x)
{
    if (! Test_OutputAssertResult(x == true, "%s == true", name)) {
        Test_Output("%s = %s", name, x ? "true" : "false");

        return false;
    }

    return true;
}

/**
 * Asserts that the value is false.
 *
 * @param name  Name to output in assertion result.
 * @param x     Value to compare.
 * @returns     True if the value is false, false otherwise.
 */
stock bool Test_AssertFalse(const char[] name, any x)
{
    if (! Test_OutputAssertResult(x == false, "%s == false", name)) {
        Test_Output("%s = %s", name, x ? "true" : "false");

        return false;
    }

    return true;
}

/**
 * Asserts that the value is null.
 *
 * @param name  Name to output in assertion result.
 * @param x     Value to compare.
 * @returns     True if the value is null, false otherwise.
 */
stock bool Test_AssertNull(const char[] name, Handle x)
{
    if (! Test_OutputAssertResult(x == null, "%s == null", name)) {
        Test_Output("%s = %d", name, x);

        return false;
    }

    return true;
}

/**
 * Asserts that the value is not null.
 *
 * @param name  Name to output in assertion result.
 * @param x     Value to compare.
 * @returns     True if the value is not null, false otherwise.
 */
stock bool Test_AssertNotNull(const char[] name, Handle x)
{
    if (! Test_OutputAssertResult(x != null, "%s != null", name)) {
        Test_Output("%s = %d", name, x);

        return false;
    }

    return true;
}

/**
 * Asserts that two values are equal.
 *
 * @param name  Name to output in assertion result.
 * @param x     First value to compare.
 * @param y     Second value to compare.
 * @returns     True if the values are equal, false otherwise.
 */
stock bool Test_AssertEqual(const char[] name, any x, any y)
{
    if (! Test_OutputAssertResult(x == y, "%s == %d", name, y)) {
        Test_Output("%s = %d", name, x);

        return false;
    }

    return true;
}

/**
 * Asserts that two values are not equal.
 *
 * @param name  Name to output in assertion result.
 * @param x     First value to compare.
 * @param y     Second value to compare.
 * @returns     True if the values are not equal, false otherwise.
 */
stock bool Test_AssertNotEqual(const char[] name, any x, any y)
{
    if (! Test_OutputAssertResult(x != y, "%s != %d", name, y)) {
        Test_Output("%s = %d", name, x);

        return false;
    }

    return true;
}

/**
 * Helper function to check whether two floats are "equal" within a
 * threshold (to account for floating point errors).
 * @see https://en.wikipedia.org/wiki/Floating_point_error_mitigation
 * @param x             First value to compare.
 * @param y             Second value to compare.
 * @param threshold     Maximum allowed difference between floats.
 * @returns             True if the floats are equal within
 *                      the threshold, false otherwise.
 */
stock bool Test_FloatsEqual(float x, float y, float threshold = 0.0001)
{
    if (x == y) {
        return true;
    }

    return y > (x - threshold) && y < (x + threshold);
}

/**
 * Asserts that two floats are equal (@see Test_FloatsEqual).
 *
 * @param name          Name to output in assertion result.
 * @param x             First value to compare.
 * @param y             Second value to compare.
 * @param threshold     Maximum allowed difference between floats.
 * @returns             True if the floats are equal within
 *                      the threshold, false otherwise.
 */
stock bool Test_AssertFloatsEqual(
    const char[] name,
    float x,
    float y,
    float threshold = 0.0001
)
{
    if (! Test_OutputAssertResult(
        Test_FloatsEqual(x, y, threshold),
        "%s == %f",
        name,
        y
    )) {
        Test_Output("%s = %f", name, x);

        return false;
    }

    return true;
}

/**
 * Asserts that two floats are not equal (@see Test_FloatsEqual).
 *
 * @param name          Name to output in assertion result.
 * @param x             First value to compare.
 * @param y             Second value to compare.
 * @param threshold     Maximum allowed difference between floats.
 * @returns             True if the floats are not equal within
 *                      the threshold, false otherwise.
 */
stock bool Test_AssertFloatsNotEqual(
    const char[] name,
    float x,
    float y,
    float threshold = 0.0001
)
{
    if (! Test_OutputAssertResult(
        ! Test_FloatsEqual(x, y, threshold),
        "%s != %f",
        name,
        y
    )) {
        Test_Output("%s = %f", name, x);

        return false;
    }

    return true;
}

/**
 * Asserts that two strings are equal.
 *
 * @param name  Name to output in assertion result.
 * @param x     First string to compare.
 * @param y     Second string to compare.
 * @returns     True if the strings are equal, false otherwise.
 */
stock bool Test_AssertStringsEqual(
    const char[] name,
    const char[] x,
    const char[] y
)
{
    if (! Test_OutputAssertResult(StrEqual(x, y), "%s == \"%s\"", name, y)) {
        Test_Output("%s = \"%s\"", name, x);

        return false;
    }

    return true;
}

/**
 * Asserts that two strings are not equal.
 *
 * @param name  Name to output in assertion result.
 * @param x     First string to compare.
 * @param y     Second string to compare.
 * @returns     True if the strings are not equal, false otherwise.
 */
stock bool Test_AssertStringsNotEqual(
    const char[] name,
    const char[] x,
    const char[] y
)
{
    if (! Test_OutputAssertResult(! StrEqual(x, y), "%s != \"%s\"", name, y)) {
        Test_Output("%s = \"%s\"", name, x);

        return false;
    }

    return true;
}
