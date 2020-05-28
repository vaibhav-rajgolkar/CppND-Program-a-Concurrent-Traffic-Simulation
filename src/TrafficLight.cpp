#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    // wait till data becomes available in message queue
    _condition.wait(uLock, [&, this]() { return !_queue.empty(); });

    T message = _queue.back();
    _queue.pop_back();

    return message;
}

template <typename T>
void MessageQueue<T>::send(T &&message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _queue.push_back(std::move(message));
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
    : _currentPhase(TrafficLightPhase::_phaseRed)
{
}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        TrafficLightPhase message = _queue.receive();

        if (message == TrafficLightPhase::_phaseGreen)
        {
            setCurrentPhase(message);
            return;
        }
    }
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()
{
    std::lock_guard<std::mutex> lock(_mutex);

    return _currentPhase;
}

void TrafficLight::setCurrentPhase(TrafficLight::TrafficLightPhase newPhase)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _currentPhase = newPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // random number generator for random cycle durations between 4 to 6
    std::random_device randomDevice;
    std::mt19937 mersenneTwisterEngine(randomDevice());
    std::uniform_int_distribution<int> distribution(4, 6);
    int cycleDuration = distribution(mersenneTwisterEngine);

    std::chrono::time_point<std::chrono::system_clock> previousTimer;

    // init stop watch
    previousTimer = std::chrono::system_clock::now();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        int timeDifference = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - previousTimer).count();

        // move toggled current phase to message queue
        if (timeDifference == cycleDuration)
        {
            if (getCurrentPhase() == TrafficLightPhase::_phaseRed)
            {
                setCurrentPhase(TrafficLightPhase::_phaseGreen);
            }
            else
            {
                setCurrentPhase(TrafficLightPhase::_phaseRed);
            }

            _queue.send(std::move(_currentPhase));

            // reset stop watch
            previousTimer = std::chrono::system_clock::now();
            cycleDuration = distribution(mersenneTwisterEngine);
        }
    }
}
