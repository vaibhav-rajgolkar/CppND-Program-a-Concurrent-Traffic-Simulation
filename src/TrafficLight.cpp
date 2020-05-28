#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [&, this](){ return !_queue.empty(); });

    T message = _queue.front();
    _queue.pop_front();

    return message;
}

template <typename T>
void MessageQueue<T>::send(T &&message)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
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
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {
       TrafficLightPhase message = _queue.receive();
       if(message == TrafficLightPhase::_phaseGreen)
        {
            return;
        }
    }
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    
    std::chrono::time_point<std::chrono::system_clock> previousTimer;

    // init stop watch
    previousTimer = std::chrono::system_clock::now();
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        double timeDifference = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - previousTimer).count();
        // move toggled current phase to message queue
        if(timeDifference >= 4 && timeDifference <= 6)
        {
            if(_currentPhase == TrafficLightPhase::_phaseRed)
            {
                _currentPhase == TrafficLightPhase::_phaseGreen;
            }
            else
            {
                _currentPhase == TrafficLightPhase::_phaseRed;
            }
            _queue.send(std::move(_currentPhase));
        }
        // reset stop watch
        previousTimer = std::chrono::system_clock::now();
    }
}

