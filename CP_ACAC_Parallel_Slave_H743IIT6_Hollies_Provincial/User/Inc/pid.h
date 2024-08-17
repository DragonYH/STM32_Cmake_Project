#ifndef PID_H
#define PID_H

// PID初始化参数，每次一个PID都要重新定义一个新名称。
typedef struct PID_t
{
	float kp;
	float ki;
	float kd;
	float err[3]; // 偏差结果
	float out;	  // 计算结果
	float Max;	  // 输出的最大值
	float Min;	  // 输出的最小值

} PID_t;

void pid_Init(PID_t *pid, float kp, float ki, float kd, float max, float min); // 初始化结构体PID
void pid(PID_t *pid, float target, float sample);							   // PID计算

#endif
