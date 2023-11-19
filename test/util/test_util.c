#include "unity_fixture.h"

static void RunUTILTests(void)
{
    RUN_TEST_GROUP(BYTE);
    RUN_TEST_GROUP(CRC);
}

int main(int argc, const char * argv[])
{
    return UnityMain(argc, argv, RunUTILTests);
}
