#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

int main(int argv, char* argv[]) {
    doctest::Context context = doctest::Context{argc, argv};
    auto resultCode = 0;

    //context.setOption("order-by", "name");
    //context.setOption("success", "true");
    resultCode = context.run();

    if (context.shouldExit() || resultCode != 0) {
        exit(-1);
    }
}


