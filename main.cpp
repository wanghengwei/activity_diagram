// #include <rxcpp/rx.hpp>
#include "Robot.h"
#include "Blueprint.h"
#include <boost/log/trivial.hpp>
#include <fruit/fruit.h>

using namespace std::literals::chrono_literals;

int main() {
    BOOST_LOG_TRIVIAL(info) << "main";

    fruit::Injector<Blueprint> injector{getBlueprint};

    Robot robot{injector.get<Blueprint*>(), 1};
    robot.start();

    Robot robot2{injector.get<Blueprint*>(), 2};
    robot2.start();


    BOOST_LOG_TRIVIAL(info) << "end";

    while (true) {
        // loop.dispatch();
        std::this_thread::sleep_for(1s);
    }

    // std::this_thread::sleep_for(std::chrono::minutes{2});
    return 0;
}