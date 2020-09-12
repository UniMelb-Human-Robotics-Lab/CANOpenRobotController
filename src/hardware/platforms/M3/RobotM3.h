/**
 *
 * \file RobotM3.h
 * \author Vincent Crocher
 * \version 0.2
 * \date 2020-07-27
 * \copyright Copyright (c) 2020
 *
 * \brief  The<code> RobotM3</ code> class represents an M3 Robot.
 *
 */

#ifndef RobotM3_H_INCLUDED
#define RobotM3_H_INCLUDED

#include <map>
#include <Eigen/Dense>

#include "JointM3.h"
#include "Keyboard.h"
#include "Joystick.h"
#include "Robot.h"


typedef Eigen::Vector3d V3; //! Convenience alias for double  Vector of length 3

/**
     * \todo Load in paramaters and dictionary entries from JSON file.
     *
     */

typedef struct M3Tool
{
    M3Tool(Eigen::Matrix4d t, double m, std::string name="tool"):T(t),Mass(m),Name(name) {};
    const Eigen::Matrix4d T; //Transformation matrix (in m)
    const double Mass; //In kg
    const std::string Name;
} M3Tool;

//Classic tools attached to M3
static M3Tool M3Handle(Eigen::Matrix4d::Identity(), 0.95, "Handle"); //! Default handle with 3 rotational DoFs
static M3Tool M3MachiningTool(Eigen::Matrix4d::Identity(), 0.5, "Machining tool"); //!

/**
 * \brief Implementation of the M3 robot class, representing an M3 using 3 JointM3 (and so Kinco drives).
 * model reference:
 *             2
 *      3       \
 *      /\       \(L2)
 * (L3)/  \       \
 *    /    \       \
 *   4      \      .\0
 *           \   .
 *          1\.  (L1)
 *
 */
class RobotM3 : public Robot {
   private:
    float LinkLengths[5] = {0.056, 0.15-0.015, 0.5, 0.465, 0.465+0.15-0.015};   /*!< Link lengths used for kniematic models (in m)*/
    float LinkMasses[5] = {0, 0.450, 0.700, 0.200, .0};                        /*!< Link masses used for gravity compensation (in kg)*/

    M3Tool *endEffTool; /*!< End-effector representation (transformation and mass) */

    Eigen::Vector3d qCalibration = {38*M_PI/180., 70*M_PI/180., 95*M_PI/180.};  /*!< Calibration configuration: posture in which the robot is when using the calibration procedure */

    bool calibrated;
    double maxEndEffVel; /*!< Maximal end-effector allowable velocity. Used in checkSafety when robot is calibrated.*/
    double maxEndEffForce; /*!< Maximal end-effector allowable force. Used in checkSafety when robot is calibrated. */

   public:
    /**
      * \brief Default <code>RobotM3</code> constructor.
      * Initialize memory for the Exoskelton <code>Joint</code> + sensors.
      * Load in exoskeleton paramaters to  <code>TrajectoryGenerator.</code>.
      */
    RobotM3();
    ~RobotM3();

    Keyboard *keyboard;
    Joystick *joystick;

    /**
       * \brief Initialises all joints to position control mode.
       *
       * \return true If all joints are successfully configured
       * \return false  If some or all joints fail the configuration
       */
    bool initPositionControl();

    /**
       * \brief Initialises all joints to velocity control mode.
       *
       * \return true If all joints are successfully configured
       * \return false  If some or all joints fail the configuration
       */
    bool initVelocityControl();

    /**
       * \brief Initialises all joints to torque control mode.
       *
       * \return true If all joints are successfully configured
       * \return false  If some or all joints fail the configuration
       */
    bool initTorqueControl();

    /**
       * \brief Send a stop command to all joint drives.
       *
       * \return true If all joints are stopped
       * \return false  Otherwise
       */
    bool stop();

    /**
    * \brief Set the target positions for each of the joints
    *
    * \param positions a vector of target positions - applicable for each of the actauted joints
    * \return MovementCode representing success or failure of the application
    */
    setMovementReturnCode_t applyPosition(std::vector<double> positions);

    /**
    * \brief Set the target velocities for each of the joints
    *
    * \param velocities a vector of target velocities - applicable for each of the actuated joints
    * \return MovementCode representing success or failure of the application
    */
    setMovementReturnCode_t applyVelocity(std::vector<double> velocities);

    /**
    * \brief Set the target torque for each of the joints
    *
    * \param torques a vector of target torques - applicable for each of the actuated joints
    * \return MovementCode representing success or failure of the application
    */
    setMovementReturnCode_t applyTorque(std::vector<double> torques);

    /**
    * \brief Apply current configuration as calibration configuration using qcalibration such that:
    *  q=qcalibration in current configuration.
    */
    void applyCalibration();

    bool isCalibrated() {return calibrated;}
    void decalibrate() {calibrated = false;}


    /**
       * \brief Implementation of Pure Virtual function from <code>Robot</code> Base class.
       * Create designed <code>Joint</code> and <code>Driver</code> objects and load into
       * Robot joint vector.
       */
    bool initialiseJoints();
    /**
       * \brief Implementation of Pure Virtual function from <code>Robot</code> Base class.
       * Initialize each <code>Drive</code> Objects underlying CANOpen Networking.

      */
    bool initialiseNetwork();
    /**
       * \brief Implementation of Pure Virtual function from <code>Robot</code> Base class.
       * Initialize each <code>Input</code> Object.

      */
    bool initialiseInputs();
    /**
       * \brief update current state of the robot, including input and output devices.
       * Overloaded Method from the Robot Class.
       * Example. for a keyboard input this would poll the keyboard for any button presses at this moment in time.
       */
    void updateRobot();

    /**
     * \brief Check if current end effector force and velocities are within limits (if calibrated, otherwise
     *  check that joints velocity and torque are within limits).
     *
     * \return OUTSIDE_LIMITS if outside the limits (!), SUCCESS otherwise
     */
    setMovementReturnCode_t safetyCheck();

    void printStatus();
    void printJointStatus();


    Eigen::Matrix3d J();
    Eigen::Vector3d directKinematic(Eigen::Vector3d q);
    Eigen::Vector3d inverseKinematic(Eigen::Vector3d X);
    Eigen::Vector3d calculateGravityTorques();

    Eigen::Vector3d getJointPosition();
    Eigen::Vector3d getJointVelocity();
    Eigen::Vector3d getJointTorque();
    Eigen::Vector3d getEndEffPosition();
    Eigen::Vector3d getEndEffVelocity();
    Eigen::Vector3d getEndEffForce();

    setMovementReturnCode_t setJointPosition(Eigen::Vector3d q);
    setMovementReturnCode_t setJointVelocity(Eigen::Vector3d q);
    setMovementReturnCode_t setJointTorque(Eigen::Vector3d tau);
    setMovementReturnCode_t setEndEffPosition(Eigen::Vector3d X);
    setMovementReturnCode_t setEndEffVelocity(Eigen::Vector3d dX);
    setMovementReturnCode_t setEndEffForce(Eigen::Vector3d F);
    setMovementReturnCode_t setEndEffForceWithCompensation(Eigen::Vector3d F, bool friction_comp=true);


    void changeTool(M3Tool *new_tool) {endEffTool=new_tool; std::cout << "RobotM3::changeTool: new tool: " << endEffTool->Name << std::endl;}
};
#endif /*RobotM3_H*/
