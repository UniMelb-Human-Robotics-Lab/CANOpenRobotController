/**
 * \file State.h
 * \author William Campbell
 * \version 0.1
 * \date 2019-09-24
 * For more detail on the architecture and mechanics of the state machine class see: https://embeded.readthedocs.io/en/latest/StaeMachines/.
 * \copyright Copyright (c) 2019
 *
 */

#ifndef EXO_STATE_H
#define EXO_STATE_H

#include <cstddef>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <Eigen/Dense>


#include "StateMachine.h" //TODO remove?
class State;


typedef std::function<bool()> TransitionCallback_t; //TODO: make w/ template and pointer on specialised stateMachine?
typedef std::pair<const std::shared_ptr<State> &, TransitionCallback_t> Transition_t; //TODO: rename?



#define MAXARCS 10 /*<!Define the max number of arcs (transitions) any state can have*/
/**
 *  @ingroup stateMachine
 * \brief Abstract class representing a state in a StateMachine
 *
 */
class State {
    //friend class StateMachine; //TODO remove
    // A State machine class can access the private and protected members of a state class  */
   public:
     EIGEN_MAKE_ALIGNED_OPERATOR_NEW // Required to use eigen fixed size vectors/objects in states. See first section of http://eigen.tuxfamily.org/dox-devel/group__TopicUnalignedArrayAssert.html.

    /**
     * \brief Pointer to the owning state machine
     *
     */
    //StateMachine *owner;

    /**
     * \brief Construct a new State object
     *
     * \param n Name of the state machine
     */
    State(const char n[] = NULL) {
        if(n==NULL)
            name = "";
        else
            name = n;
    };
    ~State();

    void allowTransitionTo(const std::shared_ptr<State> &s, TransitionCallback_t f) {
        transitions.push_back(Transition_t(s, f));
    }

    /**
     * \brief Called once when the state is entered. Pure virtual function, must be overwritten by each state
     *
     */
    virtual void entry(void) = 0;

    /**
     * \brief Called continuously whilst in that state. Pure virtual function, must be overwritten by each state
     *
     */
    virtual void during(void) = 0;

    /**
     * \brief Called once when the state exits. Pure virtual function, must be overwritten by each state
     *
     */
    virtual void exit(void) = 0;

    /**
     * \brief Returns the name of the state
     *
     * \return The name of the state
     */
    const std::string &getName(void);

    /**
     * \brief Prints the name of the state
     *
     */
    void printName(void);

    const std::shared_ptr<State> & getActiveArc(void);

   private:
    /**
    * \brief List of possible transitions
    *
    */
    std::string name;                       /*<!Name of this State*/
    std::vector<Transition_t> transitions; //Vector of std::pair
};

#endif  //EXO_STATE_H
