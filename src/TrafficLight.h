#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <thread>
#include <vector>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


// FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there should be an std::condition_variable as well as an std::mutex as private members. 

template <class T>
class MessageQueue {
public:
	T receive();
	void send(T&& msg);

private:
	std::mutex _mutex;
	std::condition_variable _condition;
	std::deque<T> _queue;
};


enum TrafficLightPhase {red, green};

class TrafficLight : public TrafficObject
{
public:
    void waitForGreen();
    void simulate() override;
    TrafficLight();
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.

    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;


    std::condition_variable _condition;
    std::mutex _mutex;
    std::vector<std::thread> _threads;
    MessageQueue<TrafficLightPhase> _message_queue;
};

#endif