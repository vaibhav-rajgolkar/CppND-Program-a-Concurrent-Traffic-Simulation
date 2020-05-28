#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
    T receive();
    void send(T &&message);

private:
    std::deque<T> _queue; // queue to store TrafficLightPhases
    std::condition_variable _condition;
    std::mutex _mutex;
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    enum class TrafficLightPhase
    {
        _phaseRed,
        _phaseGreen
    };

    // getters / setters
    TrafficLightPhase getCurrentPhase();
    void setCurrentPhase(TrafficLightPhase newPhase);

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods

    void cycleThroughPhases();

    MessageQueue<TrafficLightPhase> _queue; // message queue to store traffic phases for traffic light
    TrafficLightPhase _currentPhase;        // current traffic phase
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif