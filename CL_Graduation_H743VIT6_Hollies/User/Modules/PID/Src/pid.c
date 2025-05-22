#include "pid.h"

/**
 * @brief pid参数初始化
 * @param pid pid指针
 * @param kp 比例系数
 * @param ki 积分系数
 * @param kd 微分系数
 * @param max 输出最大值
 * @param min 输出最小值
 */
void pid_Init(PID *pid, float kp, float ki, float kd, float max, float min)
{
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->Max = max;
	pid->Min = min;
	pid->err[0] = 0;
	pid->err[1] = 0;
	pid->err[2] = 0;
	pid->integral = 0; // 初始化积分项
	pid->out = 0;
}
/**
 * @brief 增量式PID控制器
 * @param pid pid指针
 * @param target 目标值
 * @param sample 测量值
 * @note 根据增量式离散PID公式
 * @note dac+=kp[e(0)-e(1)]+ki*e(0)+kd[e(0)-2e(1)+e(2)]
 * @note e(0)代表本次偏差
 * @note e(1)代表上一次的偏差  以此类推
 * @note result代表增量输出
 */
void pid_incremental(PID *pid, float target, float sample)
{
	pid->err[0] = target - sample;
	pid->out += pid->kp * (pid->err[0] - pid->err[1]) +
				pid->ki * pid->err[0] +
				pid->kd * (pid->err[0] - 2 * pid->err[1] + pid->err[2]);

	// 限幅处理
	if (pid->out > pid->Max)
		pid->out = pid->Max;
	else if (pid->out < pid->Min)
		pid->out = pid->Min;

	// 更新误差历史
	pid->err[2] = pid->err[1];
	pid->err[1] = pid->err[0];
}

void pid_positional(PID *pid, float target, float sample)
{
	float error = target - sample;

	// 积分项累积（带抗饱和处理）
	pid->integral += error;

	// 计算各分量
	float p_term = pid->kp * error;
	float i_term = pid->ki * pid->integral;
	float d_term = pid->kd * (error - pid->err[0]);

	// 计算输出
	float output = p_term + i_term + d_term;

	// 输出限幅与积分抗饱和
	if (output > pid->Max)
	{
		output = pid->Max;
		pid->integral -= error; // 回退积分
	}
	else if (output < pid->Min)
	{
		output = pid->Min;
		pid->integral -= error; // 回退积分
	}

	pid->out = output;
	pid->err[0] = error; // 保存当前误差供下次微分计算
}
