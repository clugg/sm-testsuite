# sm-testsuite
![Build Status](https://github.com/clugg/sm-testsuite/workflows/Compile%20with%20SourceMod/badge.svg) [![Latest Release](https://img.shields.io/github/v/release/clugg/sm-testsuite?include_prereleases&sort=semver)](https://github.com/clugg/sm-testsuite/releases)

A pure SourcePawn implementation of basic testing and assertions.

Table of Contents
=================

* [Requirements](#requirements)
* [Installation](#installation)
* [Usage](#usage)
* [Contributing](#contributing)
* [License](#license)

## Requirements
* SourceMod 1.8 or later

## Installation
Download the source code for the [latest release](https://github.com/clugg/sm-testsuite/releases/latest) and move all files and directories from the [`addons/sourcemod/scripting/include`](addons/sourcemod/scripting/include) directory to your existing `addons/sourcemod/scripting/include` directory.

## Usage
First, you need to include the library:

```c
#include <testsuite>
```

### Sections
A section begins with the name of the section and ends by outputting the summary result of all tests executed within that section. If you are using SourceMod 1.10 or later, your tests will automatically be profiled and the time it took to execute the tests will be outputted after the test results.

```c
Test_StartSection("section name");
// your tests here
Test_EndSection();
```

### Tests
A test begins with the name of the test and ends by outputting the summary result of all assertions within that test. All test methods must conform to the `Test_Method` typedef, which is simply a function which takes no parameters and returns nothing (void). You can run a test with a custom name, or let SourceMod resolve the test based on the name. *Note: when using name-based resolving, if you don't call the method anywhere else, you will get an unused symbol warning.*

```c
void it_should_do_something()
{
    // your assertions here
}

Test_Run("it_should_do_something", it_should_do_something);
// or
Test_RunFromString("it_should_do_something");
```

### Assertions
An assertion is a statement that something should be a certain way. There are multiple types of assertions. Every assertion method accepts a name as the first parameter to make your test output more human readable. Every assertion you make will count towards the total assertions made in an individual test, and a single failed assertion means the entire test is considered failed. When an assertion fails, the value of the variable being asserted against will be outputted as information. *Note: when an assertion fails, the function will continue executing unless you explicitly stop it.*
```c
Test_AssertTrue("value", true);
Test_AssertFalse("value", false);
Test_AssertEqual("matching int", 1, 1);
Test_AssertNotEqual("non-matching int", 1, 2);
Test_AssertFloatsEqual("float", 0.1, 0.1);
Test_AssertStringsEqual("string", "hello", "hello");
```

`Test_AssertFloatsEqual` accepts an optional fourth parameter to set the threshold under which floats are considered equal. This is used to combat [floating point errors](https://en.wikipedia.org/wiki/Floating_point_error_mitigation) and has been set to a sensible default.

### Customising Output
Each section's tests and results are outputted within a box shape. The box has a default width of 64, but you can change it per-section if you'd like your the box to better fit your test output.

```c
Test_SetBoxWidth(32);
```

You might want to output some information not directly related to an assertion. However, if you simply use `PrintToServer`, the information will not align correctly with the rest of the test output. A special function is provided for this, which uses `VFormat` internally, meaning you can treat it in a similar way to `Format`.

```c
Test_Output("here is some information: %d", 1);
```

### Putting It All Together / Reading Output
```c
#include <testsuite>

void it_has_no_asserts()
{
    Test_Output("I have no strong feelings one way or the other.");
}

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
    Test_AssertTrue("value", false);
    Test_AssertFalse("value", true);
    Test_AssertEqual("matching int", 1, 2);
    Test_AssertNotEqual("non-matching int", 1, 1);
    Test_AssertFloatsEqual("float", 0.1, 0.2);
    Test_AssertStringsEqual("string", "hello", "world");
    Test_Output("here is some information: %d", 1);
}

void it_should_fail_some()
{
    Test_AssertTrue("value", true);
    Test_AssertFalse("value", true);
}

public void OnPluginStart()
{
    Test_StartSection("passing section");
    Test_Run("it_has_no_asserts", it_has_no_asserts);
    Test_Run("it_should_pass_all", it_should_pass_all);
    Test_EndSection();

    Test_SetBoxWidth(32);
    Test_StartSection("failing section");
    Test_Run("it_should_fail_all", it_should_fail_all);
    Test_EndSection();

    Test_SetBoxWidth(42);
    Test_StartSection("both section");
    Test_Run("it_should_pass_all", it_should_pass_all);
    Test_Run("it_should_fail_all", it_should_fail_all);
    Test_Run("it_should_fail_some", it_should_fail_some);
    Test_EndSection();

    Test_SetBoxWidth(22);
    Test_StartSection("empty section");
    Test_EndSection();
}
```

This plugin will produce the following output:
```
|--------------------------------------------------------------|
|                       passing section                        |
|--------------------------------------------------------------|
|                                                              |
| it_has_no_asserts                                            |
|   [=] I have no strong feelings one way or the other.        |
| PASS                                                         |
|                                                              |
|--------------------------------------------------------------|
|                                                              |
| it_should_pass_all                                           |
|   [ ] value == true                                          |
|   [ ] value == false                                         |
|   [ ] matching int == 1                                      |
|   [ ] non-matching int != 2                                  |
|   [ ] float == 0.100000                                      |
|   [ ] string == "hello"                                      |
|   [=] here is some information: 1                            |
| Assertions: 6 passed                                         |
| PASS                                                         |
|                                                              |
|--------------------------------------------------------------|
| Tests: 2 passed                                              |
| Time:  0.003803s                                             |
|--------------------------------------------------------------|

|------------------------------|
|       failing section        |
|------------------------------|
|                              |
| it_should_fail_all           |
|   [!] value == true          |
|   [=] value = false          |
|   [!] value == false         |
|   [=] value = true           |
|   [!] matching int == 2      |
|   [=] matching int = 1       |
|   [!] non-matching int != 1  |
|   [=] non-matching int = 1   |
|   [!] float == 0.200000      |
|   [=] float = 0.100000       |
|   [!] string == "world"      |
|   [=] string = "hello"       |
|   [=] here is some information: 1 |
| Assertions: 6 failed         |
| FAIL!                        |
|                              |
|------------------------------|
| Tests: 1 failed              |
| Time:  0.002643s             |
|------------------------------|

|----------------------------------------|
|              both section              |
|----------------------------------------|
|                                        |
| it_should_pass_all                     |
|   [ ] value == true                    |
|   [ ] value == false                   |
|   [ ] matching int == 1                |
|   [ ] non-matching int != 2            |
|   [ ] float == 0.100000                |
|   [ ] string == "hello"                |
|   [=] here is some information: 1      |
| Assertions: 6 passed                   |
| PASS                                   |
|                                        |
|----------------------------------------|
|                                        |
| it_should_fail_all                     |
|   [!] value == true                    |
|   [=] value = false                    |
|   [!] value == false                   |
|   [=] value = true                     |
|   [!] matching int == 2                |
|   [=] matching int = 1                 |
|   [!] non-matching int != 1            |
|   [=] non-matching int = 1             |
|   [!] float == 0.200000                |
|   [=] float = 0.100000                 |
|   [!] string == "world"                |
|   [=] string = "hello"                 |
|   [=] here is some information: 1      |
| Assertions: 6 failed                   |
| FAIL!                                  |
|                                        |
|----------------------------------------|
|                                        |
| it_should_fail_some                    |
|   [ ] value == true                    |
|   [!] value == false                   |
|   [=] value = true                     |
| Assertions: 1 passed / 1 failed        |
| FAIL!                                  |
|                                        |
|----------------------------------------|
| Tests: 1 passed / 2 failed             |
| Time:  0.002664s                       |
|----------------------------------------|

|--------------------|
|   empty section    |
|--------------------|
| No tests run!      |
| Time:  0.000000s   |
|--------------------|
```

The output is designed to be fairly simple and eye-catching for when assertions do fail.
* `[ ]` denotes that the assertion has passed
* `[!]` denotes that the assertion has failed
* `[=]` denotes information - provided either by `Test_Output` or automatically when an assertion fails

Note that if you are using a SourceMod version lower than 1.10, you will not see the time output.

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
[GNU General Public License v3.0](https://choosealicense.com/licenses/gpl-3.0/)
