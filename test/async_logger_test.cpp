//
// simple test for the async logger
//

#include "async_logger.h"
#include <thread>
#include <iostream>
#include <string>

int main()
{
    async_logger::Logger logger;
    logger.start_logger();
    auto producerFunc = [&logger]()
    {
        for (int i = 0; i < 100; i++)
        {
            logger.log("Hello from the logger(%d) %d", std::this_thread::get_id(), i);
        }
    };

    std::thread producer1(producerFunc);
    std::thread producer2(producerFunc);
    std::thread producer3(producerFunc);

    producer1.join();
    producer2.join();
    producer3.join();

    logger.log("Finishing up");
    logger.stop_logger();

    return 0;
}