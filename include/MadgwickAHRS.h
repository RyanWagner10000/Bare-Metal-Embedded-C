//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef MadgwickAHRS_h
#define MadgwickAHRS_h

#include <math.h>
#include <stdint.h>
#include <errno.h>

//----------------------------------------------------------------------------------------------------
// Variable declaration

// extern volatile float beta;           // algorithm gain
// extern volatile float q_w, q_x, q_y, q_z; // quaternion of sensor frame relative to auxiliary frame

#define PI (3.14159f)

//---------------------------------------------------------------------------------------------------
// Function declarations

typedef struct
{
    float q_w, q_x, q_y, q_z;
} Quaterntion;

typedef struct
{
    float x, y, z;
} Euler;

Quaterntion getQuaternion(void);
void setQuaternion(Quaterntion q_);
void eulerToQuaternion(Quaterntion *q, Euler e);
void quaternionToEuler(Quaterntion q, Euler *e);
// void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az);

#endif
//=====================================================================================================
// End of file
//=====================================================================================================
