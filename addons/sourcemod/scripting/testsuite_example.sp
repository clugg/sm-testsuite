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
    version = "1.0.0",
    url = "https://github.com/clugg/sm-testsuite"
};

void it_should_pass_all()
{
    Test_AssertTrue("value", true);
    Test_AssertFalse("value", false);
    Test_AssertEqual("matching int", 1, 1);
    Test_AssertNotEqual("non-matching int", 1, 2);
    Test_AssertFloatsEqual("float", 0.1, 0.1);
    Test_AssertStringsEqual("string", "hello", "hello");
    Test_Output("here is some information: %d", 1);
}

void it_should_fail_all()
{
    Test_AssertEqual("matching int", 1, 2);
}

void it_should_fail_some()
{
    Test_AssertEqual("matching int", 1, 2);
    Test_AssertNotEqual("non-matching int", 1, 2);
}

public void OnPluginStart()
{
    Test_StartSection("passing section");
    Test_Run("it_should_pass_all", it_should_pass_all);
    Test_EndSection();

    PrintToServer("");
    Test_StartSection("failing section");
    Test_Run("it_should_fail_all", it_should_fail_all);
    Test_EndSection();

    PrintToServer("");
    Test_StartSection("both section");
    Test_Run("it_should_pass_all", it_should_pass_all);
    Test_Run("it_should_fail_all", it_should_fail_all);
    Test_Run("it_should_fail_some", it_should_fail_some);
    Test_EndSection();
}