//=====================================================================================================
// MadgwickAHRS.c
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
// 19/02/2012	SOH Madgwick	Magnetometer measurement is normalised
//
//=====================================================================================================

//---------------------------------------------------------------------------------------------------
// Header files

#include "MadgwickAHRS.h"

//---------------------------------------------------------------------------------------------------
// Definitions

#define sampleFreq 250.0f // sample frequency in Hz
#define betaDef 0.1f	  // 2 * proportional gain

//---------------------------------------------------------------------------------------------------
// Variable definitions

volatile float beta = betaDef;							   // 2 * proportional gain (Kp)
// volatile float q_w = 1.0f, q_x = 0.0f, q_y = 0.0f, q_z = 0.0f; // quaternion of sensor frame relative to auxiliary frame
volatile Quaterntion q = {1.0, 0, 0, 0};

//---------------------------------------------------------------------------------------------------
// Function declarations

float invSqrt(float x);

//====================================================================================================
// Functions

Quaterntion getQuaternion(void)
{
	return q;
}

void setQuaternion(Quaterntion q_)
{
	q.q_w = q_.q_w;
	q.q_x = q_.q_x;
	q.q_y = q_.q_y;
	q.q_z = q_.q_z;
}

/**
 * @brief Function to return error number for math.h library
 *
 * @param None
 *
 * @return Error number
 */
int *__errno(void)
{
	return &errno;
}

//---------------------------------------------------------------------------------------------------
// AHRS algorithm update
/**
 * @brief Function to calculate quaternion using 3-axis Accelerometer, Gyro, and Magnetometer
 *
 * @param gx X-axis gyro value
 * @param gy Y-axis gyro value
 * @param gz Z-axis gyro value
 * @param ax X-axis accelerometer value
 * @param ay Y-axis accelerometer value
 * @param az Z-axis accelerometer value
 * @param mx X-axis magnetometer value
 * @param my Y-axis magnetometer value
 * @param mz Z-axis magnetometer value
 *
 * @return None
 */
// void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz)
// {
// 	float recipNorm;
// 	float s0, s1, s2, s3;
// 	float qDot1, qDot2, qDot3, qDot4;
// 	float hx, hy;
// 	float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

// 	// Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
// 	if ((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f))
// 	{
// 		MadgwickAHRSupdateIMU(gx, gy, gz, ax, ay, az);
// 		return;
// 	}

// 	// Rate of change of quaternion from gyroscope
// 	qDot1 = 0.5f * (-q_x * gx - q_y * gy - q_z * gz);
// 	qDot2 = 0.5f * (q_w * gx + q_y * gz - q_z * gy);
// 	qDot3 = 0.5f * (q_w * gy - q_x * gz + q_z * gx);
// 	qDot4 = 0.5f * (q_w * gz + q_x * gy - q_y * gx);

// 	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
// 	if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
// 	{

// 		// Normalise accelerometer measurement
// 		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
// 		ax *= recipNorm;
// 		ay *= recipNorm;
// 		az *= recipNorm;

// 		// Normalise magnetometer measurement
// 		recipNorm = invSqrt(mx * mx + my * my + mz * mz);
// 		mx *= recipNorm;
// 		my *= recipNorm;
// 		mz *= recipNorm;

// 		// Auxiliary variables to avoid repeated arithmetic
// 		_2q0mx = 2.0f * q_w * mx;
// 		_2q0my = 2.0f * q_w * my;
// 		_2q0mz = 2.0f * q_w * mz;
// 		_2q1mx = 2.0f * q_x * mx;
// 		_2q0 = 2.0f * q_w;
// 		_2q1 = 2.0f * q_x;
// 		_2q2 = 2.0f * q_y;
// 		_2q3 = 2.0f * q_z;
// 		_2q0q2 = 2.0f * q_w * q_y;
// 		_2q2q3 = 2.0f * q_y * q_z;
// 		q0q0 = q_w * q_w;
// 		q0q1 = q_w * q_x;
// 		q0q2 = q_w * q_y;
// 		q0q3 = q_w * q_z;
// 		q1q1 = q_x * q_x;
// 		q1q2 = q_x * q_y;
// 		q1q3 = q_x * q_z;
// 		q2q2 = q_y * q_y;
// 		q2q3 = q_y * q_z;
// 		q3q3 = q_z * q_z;

// 		// Reference direction of Earth's magnetic field
// 		hx = mx * q0q0 - _2q0my * q_z + _2q0mz * q_y + mx * q1q1 + _2q1 * my * q_y + _2q1 * mz * q_z - mx * q2q2 - mx * q3q3;
// 		hy = _2q0mx * q_z + my * q0q0 - _2q0mz * q_x + _2q1mx * q_y - my * q1q1 + my * q2q2 + _2q2 * mz * q_z - my * q3q3;
// 		_2bx = sqrtf(hx * hx + hy * hy);
// 		_2bz = -_2q0mx * q_y + _2q0my * q_x + mz * q0q0 + _2q1mx * q_z - mz * q1q1 + _2q2 * my * q_z - mz * q2q2 + mz * q3q3;
// 		_4bx = 2.0f * _2bx;
// 		_4bz = 2.0f * _2bz;

// 		// Gradient decent algorithm corrective step
// 		s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * q_y * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q_z + _2bz * q_x) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q_y * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
// 		s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q_x * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * q_z * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q_y + _2bz * q_w) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q_z - _4bz * q_x) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
// 		s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q_y * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * q_y - _2bz * q_w) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q_x + _2bz * q_z) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q_w - _4bz * q_y) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
// 		s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * q_z + _2bz * q_x) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q_w + _2bz * q_y) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q_x * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
// 		recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
// 		s0 *= recipNorm;
// 		s1 *= recipNorm;
// 		s2 *= recipNorm;
// 		s3 *= recipNorm;

// 		// Apply feedback step
// 		qDot1 -= beta * s0;
// 		qDot2 -= beta * s1;
// 		qDot3 -= beta * s2;
// 		qDot4 -= beta * s3;
// 	}

// 	// Integrate rate of change of quaternion to yield quaternion
// 	q_w += qDot1 * (1.0f / sampleFreq);
// 	q_x += qDot2 * (1.0f / sampleFreq);
// 	q_y += qDot3 * (1.0f / sampleFreq);
// 	q_z += qDot4 * (1.0f / sampleFreq);

// 	// Normalise quaternion
// 	recipNorm = invSqrt(q_w * q_w + q_x * q_x + q_y * q_y + q_z * q_z);
// 	q_w *= recipNorm;
// 	q_x *= recipNorm;
// 	q_y *= recipNorm;
// 	q_z *= recipNorm;
// }

//---------------------------------------------------------------------------------------------------
// IMU algorithm update
/**
 * @brief Function to calculate quaternion using 3-axis Accelerometer, and Gyro
 *
 * @param gx X-axis gyro value
 * @param gy Y-axis gyro value
 * @param gz Z-axis gyro value
 * @param ax X-axis accelerometer value
 * @param ay Y-axis accelerometer value
 * @param az Z-axis accelerometer value
 *
 * @return None
 */
void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az)
{
	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5f * (-q.q_x * gx - q.q_y * gy - q.q_z * gz);
	qDot2 = 0.5f * (q.q_w * gx + q.q_y * gz - q.q_z * gy);
	qDot3 = 0.5f * (q.q_w * gy - q.q_x * gz + q.q_z * gx);
	qDot4 = 0.5f * (q.q_w * gz + q.q_x * gy - q.q_y * gx);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
	{

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Auxiliary variables to avoid repeated arithmetic
		_2q0 = 2.0f * q.q_w;
		_2q1 = 2.0f * q.q_x;
		_2q2 = 2.0f * q.q_y;
		_2q3 = 2.0f * q.q_z;
		_4q0 = 4.0f * q.q_w;
		_4q1 = 4.0f * q.q_x;
		_4q2 = 4.0f * q.q_y;
		_8q1 = 8.0f * q.q_x;
		_8q2 = 8.0f * q.q_y;
		q0q0 = q.q_w * q.q_w;
		q1q1 = q.q_x * q.q_x;
		q2q2 = q.q_y * q.q_y;
		q3q3 = q.q_z * q.q_z;

		// Gradient decent algorithm corrective step
		s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
		s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q.q_x - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
		s2 = 4.0f * q0q0 * q.q_y + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
		s3 = 4.0f * q1q1 * q.q_z - _2q1 * ax + 4.0f * q2q2 * q.q_z - _2q2 * ay;
		recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	q.q_w += qDot1 * (1.0f / sampleFreq);
	q.q_x += qDot2 * (1.0f / sampleFreq);
	q.q_y += qDot3 * (1.0f / sampleFreq);
	q.q_z += qDot4 * (1.0f / sampleFreq);

	// Normalise quaternion
	recipNorm = invSqrt(q.q_w * q.q_w + q.q_x * q.q_x + q.q_y * q.q_y + q.q_z * q.q_z);
	q.q_w *= recipNorm;
	q.q_x *= recipNorm;
	q.q_y *= recipNorm;
	q.q_z *= recipNorm;
}

//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root

float invSqrt(float x)
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

// Convert Euler angles to a quaternion
// See: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
void eulerToQuaternion(Quaterntion *q, Euler e)
{
	q->q_w = cosf(e.x * 0.5f) * cosf(e.y * 0.5f) * cosf(e.z * 0.5f) + sinf(e.x * 0.5f) * sinf(e.y * 0.5f) * sinf(e.z * 0.5f);
	q->q_x = sinf(e.x * 0.5f) * cosf(e.y * 0.5f) * cosf(e.z * 0.5f) - cosf(e.x * 0.5f) * sinf(e.y * 0.5f) * sinf(e.z * 0.5f);
	q->q_y = cosf(e.x * 0.5f) * sinf(e.y * 0.5f) * cosf(e.z * 0.5f) + sinf(e.x * 0.5f) * cosf(e.y * 0.5f) * sinf(e.z * 0.5f);
	q->q_z = cosf(e.x * 0.5f) * cosf(e.y * 0.5f) * sinf(e.z * 0.5f) - sinf(e.x * 0.5f) * sinf(e.y * 0.5f) * cosf(e.z * 0.5f);
}

// Convert a quaternion to Euler angles
// See: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
void quaternionToEuler(Quaterntion q, Euler *e)
{
	e->x = atan2f( 2.0f * (q.q_w * q.q_x + q.q_y * q.q_z), 1.0f - 2.0f * (q.q_x * q.q_x + q.q_y * q.q_y));
	e->x *= 57.295779513f;
	e->y = -0.5f * PI + 2.0f * atan2f( sqrtf(1.0f + 2.0f * (q.q_w * q.q_y + q.q_x * q.q_z)),sqrtf(1.0f - 2.0f * (q.q_w * q.q_y + q.q_x * q.q_z)));
	e->y *= 57.295779513f;
	e->z = atan2f( 2.0f * (q.q_w * q.q_z + q.q_x * q.q_y), 1.0f - 2.0f * (q.q_y * q.q_y + q.q_z * q.q_z));
	e->z *= 57.295779513f;
}

//====================================================================================================
// END OF CODE
//====================================================================================================
