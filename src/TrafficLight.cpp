#include <iostream>
#include <random>
#include <future> 

#include "TrafficLight.h"


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [this] { return !_queue.empty(); }); 

    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg; 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _queue.push_back(std::move(msg));
    _condition.notify_one(); 
}


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true) {
        if (_message_queue.receive() == green) return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // example from https://stackoverflow.com/questions/7560114/random-number-c-in-some-range
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(4, 6); // define the range
    int duration = distr(eng);
    auto start = std::chrono::system_clock::now();

    while (true){
        auto end = std::chrono::system_clock::now();
        auto dt =  std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

        if (duration < dt){
            _currentPhase  = _currentPhase == red ? green : red;

            auto future = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send,
                                     &_message_queue, std::move(_currentPhase));
            future.wait();
            start = std::chrono::system_clock::now();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        duration = distr(eng);
    }
}
