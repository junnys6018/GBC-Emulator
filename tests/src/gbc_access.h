#pragma once

/*
    core classes such as GBC and CPU declare this class as a friend,
    define static functions in this class to gain access to private
    members of those classes
*/
class GBCTests
{
public:
    static bool test_instruction_timings();
};