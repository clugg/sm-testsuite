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

#if _TEST_PROFILER_ENABLED
#include <profiler>
#endif

#if _TEST_PROFILER_ENABLED
static Profiler __test_profiler = null;
#endif

/**
 * @section Test Sections
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

    PrintToServer("| %s", name);

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

    bool hasTests = __test_testsTotal > 0;
    bool hasPassedTests = __test_testsPassed > 0;
    bool hasFailedTests = __test_testsPassed != __test_testsTotal;

    char output[1024];
    strcopy(output, sizeof(output), "\nTests: ");
    if (hasTests) {
        if (hasPassedTests) {
            Format(
                output,
                sizeof(output),
                "%s%d passed",
                output,
                __test_testsPassed
            );
        }

        if (hasFailedTests) {
            if (hasPassedTests) {
                StrCat(output, sizeof(output), " / ");
            }

            Format(
                output,
                sizeof(output),
                "%s%d failed",
                output,
                __test_testsTotal - __test_testsPassed
            );
        }
    }

    PrintToServer(output);
    #if _TEST_PROFILER_ENABLED
    PrintToServer("Time:  %fs", __test_profiler.Time);
    #endif
}

/**
 * @section Test Calling
 */

typedef Test_Method = function void();

static int __test_assertsTotal = 0;
static int __test_assertsPassed = 0;

/**
 * Prepares the assert counts and prints the test name to server.
 *
 * @param name  Name of test.
 */
stock void Test_BeforeRun(const char[] name)
{
    __test_assertsTotal = 0;
    __test_assertsPassed = 0;

    PrintToServer("\n%s", name);
}

/**
 * Calculates and outputs the test result to server.
 */
stock bool Test_AfterRun()
{
    bool passed = Test_IsPassing();
    __test_testsTotal += 1;
    if (passed) {
        __test_testsPassed += 1;
    }

    bool hasAsserts = __test_assertsTotal > 0;
    bool hasPassedAsserts = __test_assertsPassed > 0;
    bool hasFailedAsserts = __test_assertsPassed != __test_assertsTotal;

    char output[1024];
    strcopy(output, sizeof(output), "Asserts: ");
    if (hasAsserts) {
        if (hasPassedAsserts) {
            Format(
                output,
                sizeof(output),
                "%s%d passed",
                output,
                __test_assertsPassed
            );
        }

        if (hasFailedAsserts) {
            if (hasPassedAsserts) {
                StrCat(output, sizeof(output), " / ");
            }

            Format(
                output,
                sizeof(output),
                "%s%d failed",
                output,
                __test_assertsTotal - __test_assertsPassed
            );
        }
    }

    PrintToServer(output);
    PrintToServer("%s", passed ? "PASS" : "FAIL!");

    return passed;
}

/**
 * Performs pre-test oprations, executes the test then
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
 * Resolves the function based on the provided name and executes Test_Run.
 *
 * @param name      Name of test to run.
 * @returns         True if all test assertions passed, false otherwise.
 */
stock bool Test_RunFromString(const char[] name)
{
    return Test_Run(name, view_as<Test_Method>(GetFunctionByName(null, name)));
}

/**
 * Determines whether or not the currently running test is passing.
 *
 * @returns     True if the test is passing, false otherwise.
 */
stock bool Test_IsPassing()
{
    return __test_assertsPassed == __test_assertsTotal;
}

/**
 * @section Asserts
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
    PrintToServer("  [=] %s", output);
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
    __test_assertsTotal += 1;
    if (result) {
        __test_assertsPassed += 1;
    }

    char output[1024];
    VFormat(output, sizeof(output), message, 3);
    PrintToServer("  [%s] %s", result ? " " : "!", output);

    return result;
}

/**
 * Asserts that the value is true.
 *
 * @param name          Name to output in assertion result.
 * @param x             Value to compare.
 * @returns             True if the value is true, false otherwise.
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
 * @param name          Name to output in assertion result.
 * @param x             Value to compare.
 * @returns             True if the value is false, false otherwise.
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
 * Asserts that two values are equal.
 *
 * @param name          Name to output in assertion result.
 * @param x             First value to compare.
 * @param y             Second value to compare.
 * @returns             True if the values are equal, false otherwise.
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
 * @param name          Name to output in assertion result.
 * @param x             First value to compare.
 * @param y             Second value to compare.
 * @returns             True if the values are not equal, false otherwise.
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
 * Asserts that two floats are "equal" within a
 * threshold to account for floating point errors.
 * @see https://en.wikipedia.org/wiki/Floating_point_error_mitigation
 *
 * @param name          Name to output in assertion result.
 * @param x             First value to compare.
 * @param y             Second value to compare.
 * @param tolerance     Maximum allowed difference between floats.
 * @returns True if the floats are equal within the threshold, false otherwise.
 */
stock bool Test_AssertFloatsEqual(
    const char[] name,
    float x,
    float y,
    float tolerance = 0.0005
)
{
    bool result;
    if (y == 0.0) {
        result = x == 0.0;
    } else {
        float difference = x / y;
        result = difference > (1 - tolerance) && difference < (1 + tolerance);
    }

    if (! Test_OutputAssertResult(result, "%s == %f", name, y)) {
        Test_Output("%s = %f", name, x);

        return false;
    }

    return true;
}

/**
 * Asserts that two strings are equal.
 *
 * @param name          Name to output in assertion result.
 * @param x             First string to compare.
 * @param y             Second string to compare.
 * @returns True if the strings are equal, false otherwise.
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
