#ifndef __PID_H_
#define __PID_H_

typedef struct
{
	float kp;
	float ki;
	float kd;
	float err[3];	// 偏差结果，位置式使用err[0]存储上一次误差
	float out;		// 计算结果
	float Max;		// 输出最大值
	float Min;		// 输出最小值
	float integral; // 积分累积量
} PID;

// 初始化函数
void pid_Init(PID *pid, float kp, float ki, float kd, float max, float min);

// 增量式PID计算
void pid_incremental(PID *pid, float target, float sample);

// 位置式PID计算
void pid_positional(PID *pid, float target, float sample);

#endif
