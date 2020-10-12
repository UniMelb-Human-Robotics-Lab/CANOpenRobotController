#include "X2DemoMachine.h"

#define OWNER ((X2DemoMachine *)owner)

X2DemoMachine::X2DemoMachine() {
    robot = new X2Robot();
    // Create PRE-DESIGNED State Machine events and state objects.
    startExo = new StartExo(this);
    /**f
     * \brief add a tranisition object to the arch list of the first state in the NewTransition MACRO.
     * Effectively creating a statemachine transition from State A to B in the event of event c.
     * NewTranstion(State A,Event c, State B)
     *
     */
    idleState = new IdleState(this, robot);
    x2DemoState = new X2DemoState(this, robot);

    NewTransition(idleState, startExo, x2DemoState);
    //Initialize the state machine with first state of the designed state machine, using baseclass function.
    StateMachine::initialize(idleState);

    // Create ros object
    x2DemoMachineRos_ = new X2DemoMachineROS(robot);
}
/**
 * \brief start function for running any designed statemachine specific functions
 * for example initialising robot objects.
 *
 */
void X2DemoMachine::init(int argc, char *argv[]) {
    DEBUG_OUT("X2DemoMachine::init()")
    initialised = robot->initialise();
    DEBUG_OUT("X2DemoMachineROS::init()")
    x2DemoMachineRos_->initialize(argc, argv);
    running = true;
    time0 = std::chrono::steady_clock::now();

    logHelper.initLogger("test_logger", "logs/helperTrial.csv", LogFormat::CSV, true);
    logHelper.add(time, "time");
    logHelper.add(robot->getPosition(), "JointPositions");
    logHelper.add(robot->getTorque(), "JointTorques");
    logHelper.startLogger();
}

void X2DemoMachine::end() {
    if(initialised) {
        logHelper.endLog();
        currentState->exit();
        robot->disable();
        delete x2DemoMachineRos_;
        delete robot;
    }
}

////////////////////////////////////////////////////////////////
// Events ------------------------------------------------------
///////////////////////////////////////////////////////////////
bool X2DemoMachine::StartExo::check(void) {
    if (OWNER->robot->keyboard.getS() == true) {
        std::cout << "Pressed S!" << std::endl;
        return true;
    }
    return false;
}
/**
 * \brief Statemachine to hardware interface method. Run any hardware update methods
 * that need to run every program loop update cycle.
 *
 */
void X2DemoMachine::hwStateUpdate(void) {
    robot->updateRobot();
}

void X2DemoMachine::update() {
    time = (std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - time0).count()) / 1e6;

    StateMachine::update();
    x2DemoMachineRos_->update();
    logHelper.recordLogData();
}
