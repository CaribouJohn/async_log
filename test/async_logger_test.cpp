//
// simple test for the async logger
//

#include "async_logger.h"
#include <thread>
#include <iostream>
#include <string>
#include <random>

int main()
{
    double test = 3.14;
    async_logger::Logger logger;
    logger.start_logger();

    std::random_device rd; // Seed
    std::mt19937 gen(rd()); // Mersenne Twister generator
    std::uniform_int_distribution<> dist(0, 99); // Range [0, 99]
    std::uniform_real_distribution<> dist2(0, 10); // Range [0, 1]


    auto producerFunc1 = [&](int id)
    {
        for (int i = 0; i < 10; i++)
        {
            logger.log(21, "Hello from the logger(%d) %d, this is PI=%.3f random double = %.5f  and %s", id, i, test, dist2(gen),
                        "first");
            logger.log(21, "Hello from the logger(%d) %d, this is PI=%.3f random double = %.5f  and %s", id, i, test, dist2(gen),
                        "another");
        }
    };

    std::thread producer1(producerFunc1, dist(gen));
    std::thread producer2(producerFunc1, dist(gen));
    std::thread producer3(producerFunc1, dist(gen));

    producer1.join();
    producer2.join();
    producer3.join();

    logger.log(45, "Testing a very long message %s \n", std::string(4096, 'X').c_str());


    logger.log(23,"Finishing up");
    logger.stop_logger();

    return 0;
}