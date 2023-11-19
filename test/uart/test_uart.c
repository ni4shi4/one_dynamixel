#include "unity_fixture.h"

static void RunUARTTests(void)
{
    RUN_TEST_GROUP(COMMUNICATION);
}

int main(int argc, const char * argv[])
{
    return UnityMain(argc, argv, RunUARTTests);
}
