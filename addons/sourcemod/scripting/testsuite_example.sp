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

#pragma semicolon 1
#pragma newdecls required

#include <sourcemod>
#include <testsuite>

public Plugin myinfo = {
    name = "Test Suite Example",
    author = "clug",
    description = "Demonstrates basic usage of sm-testsuite.",
    version = "1.2.2",
    url = "https://github.com/clugg/sm-testsuite"
};

void it_has_no_asserts()
{
    Test_Output("I have no strong feelings one way or the other.");
}

void it_should_pass_all()
{
    Test_AssertTrue("value", true);
    Test_AssertFalse("value", false);
    Test_AssertNull("value", null);
    Test_AssertNotNull("value", view_as<Handle>(1));
    Test_AssertEqual("value", 1, 1);
    Test_AssertNotEqual("value", 1, 2);
    Test_AssertFloatsEqual("value", 0.1, 0.1);
    Test_AssertFloatsNotEqual("value", 0.1, 0.2);
    Test_AssertStringsEqual("value", "hello", "hello");
    Test_AssertStringsNotEqual("value", "hello", "world");
    Test_Output("here is some information: %d", 1);
}

void it_should_fail_all()
{
    Test_AssertTrue("value", false);
    Test_AssertFalse("value", true);
    Test_AssertNull("value", view_as<Handle>(1));
    Test_AssertNotNull("value", null);
    Test_AssertEqual("value", 1, 2);
    Test_AssertNotEqual("value", 1, 1);
    Test_AssertFloatsEqual("value", 0.1, 0.2);
    Test_AssertFloatsNotEqual("value", 0.1, 0.1);
    Test_AssertStringsEqual("value", "hello", "world");
    Test_AssertStringsNotEqual("value", "hello", "hello");
    Test_Output("here is some information: %d", 1);
}

void it_should_fail_some()
{
    Test_AssertTrue("value", true);
    Test_AssertFalse("value", true);
}

public void OnPluginStart()
{
    Test_SetBoxWidth(56);
    Test_StartSection("passing section");
    Test_Run("it_has_no_asserts", it_has_no_asserts);
    Test_Run("it_should_pass_all", it_should_pass_all);
    Test_EndSection();

    Test_SetBoxWidth(36);
    Test_StartSection("failing section");
    Test_Run("it_should_fail_all", it_should_fail_all);
    Test_EndSection();

    Test_StartSection("both section");
    Test_Run("it_should_pass_all", it_should_pass_all);
    Test_Run("it_should_fail_all", it_should_fail_all);
    Test_Run("it_should_fail_some", it_should_fail_some);
    Test_EndSection();

    Test_SetBoxWidth(20);
    Test_StartSection("empty section");
    Test_EndSection();
}
