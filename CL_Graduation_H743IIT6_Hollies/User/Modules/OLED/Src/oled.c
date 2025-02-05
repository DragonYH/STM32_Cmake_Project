/********************************************************/
/*   2019/11/05
 *   连卡佛哈利
 *   STM32 7针 0.96 OLED HAL库显示驱动
 *********************************************************
 *   引脚定义：
 *   OLED_CS OLED_RES OLED_DC OUTPUT    Output push pull
 *********************************************************
 *   SPI定义：
 *   Mode:Transmit only Master
 *   Hardware Nss Signal:Disable
 *   Data Size : 8Bits
 *   First Bit : MSB First
 *   CPOL : Low
 *   CPHA : 1 Edge
 *********************************************************
 *   接线：
 *   GND ---> GND
 *   VCC ---> 3.3V
 *   DO  ---> SPI_SCK
 *   D1  ---> SPI_MOSI
 *   RES ---> OLED_RES
 *   DC  ---> OLED_DC
 *   CS  ---> OLED_CS
 *
 */
/********************************************************/
#include "oled.h"
#include "stdlib.h"
#include "stdio.h"
#include "oledfont.h"
#include "spi.h"
#include "math.h"
uint8_t OLED_GRAM[144][16];

/**
 * @brief 反显函数
 * @param i 0 正常显示 1 反色显示
 */
void OLED_ColorTurn(uint8_t i)
{
	if (i == 0)
	{
		OLED_WR_Byte(0xA6, OLED_CMD); // 正常显示
	}
	if (i == 1)
	{
		OLED_WR_Byte(0xA7, OLED_CMD); // 反色显示
	}
}

/**
 * @brief 屏幕旋转180度
 * @param i 0 正常显示 1 旋转180度
 */
void OLED_DisplayTurn(uint8_t i)
{
	if (i == 0)
	{
		OLED_WR_Byte(0xC8, OLED_CMD); // 正常显示
		OLED_WR_Byte(0xA1, OLED_CMD);
	}
	if (i == 1)
	{
		OLED_WR_Byte(0xC0, OLED_CMD); // 反转显示
		OLED_WR_Byte(0xA0, OLED_CMD);
	}
}

/**
 * @brief 写入数据
 * @param dat 要写入的数据
 * @param cmd 数据类型 0 写入命令 1 写入数据
 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	uint8_t *data = &dat;
	if (cmd)
		OLED_DC_Set();
	else
		OLED_DC_Clr();
	OLED_CS_Clr();

	HAL_SPI_Transmit(WHICH_SPI, (uint8_t *)data, 1, 200); // 需根据实际情况修改

	OLED_CS_Set();
	OLED_DC_Set();
}

/**
 * @brief 开启OLED显示
 */
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵使能
	OLED_WR_Byte(0x14, OLED_CMD); // 开启电荷泵
	OLED_WR_Byte(0xAF, OLED_CMD); // 点亮屏幕
}

/**
 * @brief 关闭OLED显示
 */
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵使能
	OLED_WR_Byte(0x10, OLED_CMD); // 关闭电荷泵
	OLED_WR_Byte(0xAF, OLED_CMD); // 关闭屏幕
}

/**
 * @brief 更新显存到OLED
 */
void OLED_Refresh(void)
{
	uint8_t i, n;
	for (i = 0; i < 16; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置行起始地址
		OLED_WR_Byte(0x00, OLED_CMD);	  // 设置低列起始地址
		OLED_WR_Byte(0x10, OLED_CMD);	  // 设置高列起始地址
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
	}
}

/**
 * @brief 清屏
 */
void OLED_Clear(void)
{
	uint8_t i, n;
	for (i = 0; i < 16; i++)
	{
		for (n = 0; n < 128; n++)
		{
			OLED_GRAM[n][i] = 0; // 清除所有数据
		}
	}
	OLED_Refresh(); // 更新显示
}

/**
 * @brief 画点
 * @param x x坐标
 * @param y y坐标
 */
void OLED_DrawPoint(uint8_t x, uint8_t y)
{
	uint8_t i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;
	OLED_GRAM[x][i] |= n;
}

/**
 * @brief 清除一个点
 * @param x x坐标
 * @param y y坐标
 */
void OLED_ClearPoint(uint8_t x, uint8_t y)
{
	uint8_t i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;
	OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
	OLED_GRAM[x][i] |= n;
	OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
}

/**
 * @brief 画线
 * @param x1 起点x坐标
 * @param y1 起点y坐标
 * @param x2 终点x坐标
 * @param y2 终点y坐标
 */
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	uint8_t i, k, k1, k2;
	if ((x2 > 128) || (y2 > 128) || (x1 > x2) || (y1 > y2))
		return;
	if (x1 == x2) // 画竖线
	{
		for (i = 0; i < (y2 - y1); i++)
		{
			OLED_DrawPoint(x1, y1 + i);
		}
	}
	else if (y1 == y2) // 画横线
	{
		for (i = 0; i < (x2 - x1); i++)
		{
			OLED_DrawPoint(x1 + i, y1);
		}
	}
	else // 画斜线
	{
		k1 = y2 - y1;
		k2 = x2 - x1;
		k = k1 * 10 / k2;
		for (i = 0; i < (x2 - x1); i++)
		{
			OLED_DrawPoint(x1 + i, y1 + i * k / 10);
		}
	}
}

/**
 * @brief 画矩形
 * @param x1 左上角x坐标
 * @param y1 左上角y坐标
 * @param x2 右下角x坐标
 * @param y2 右下角y坐标
 */
void OLED_DrawSquare(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	OLED_DrawLine(x1, y1, x2, y1);
	OLED_DrawLine(x1, y2, x2, y2);
	OLED_DrawLine(x1, y1, x1, y2);
	OLED_DrawLine(x2, y1, x2, y2);
}

/**
 * @brief 画圆
 * @param x 圆心x坐标
 * @param y 圆心y坐标
 * @param r 圆半径
 */
void OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t r)
{
	int a, b, num;
	a = 0;
	b = r;
	while (2 * b * b >= r * r)
	{
		OLED_DrawPoint(x + a, y - b);
		OLED_DrawPoint(x - a, y - b);
		OLED_DrawPoint(x - a, y + b);
		OLED_DrawPoint(x + a, y + b);

		OLED_DrawPoint(x + b, y + a);
		OLED_DrawPoint(x + b, y - a);
		OLED_DrawPoint(x - b, y - a);
		OLED_DrawPoint(x - b, y + a);

		a++;
		num = (a * a + b * b) - r * r; // 计算画的点离圆心的距离
		if (num > 0)
		{
			b--;
			a--;
		}
	}
}

/**
 * @brief 显示一个字符
 * @param x x坐标0-127
 * @param y y坐标0-63
 * @param chr 要显示的字符
 * @param size1 字体大小 12/16/24
 * @note 取模方式 逐列式
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size1)
{
	uint8_t i, m, temp, size2, chr1;
	uint8_t y0 = y;
	size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2); // 得到字体一个字符对应点阵集所占的字节数
	chr1 = chr - ' ';										   // 计算偏移后的值
	for (i = 0; i < size2; i++)
	{
		if (size1 == 12)
		{
			temp = asc2_1206[chr1][i];
		} // 调用12 06字体
		else if (size1 == 16)
		{
			temp = asc2_1608[chr1][i];
		} // 调用16 08字体
		else if (size1 == 24)
		{
			temp = asc2_2412[chr1][i];
		} // 调用24 12字体
		else
			return;
		for (m = 0; m < 8; m++) // 写入数据
		{
			if (temp & 0x80)
				OLED_DrawPoint(x, y);
			else
				OLED_ClearPoint(x, y);
			temp <<= 1;
			y++;
			if ((y - y0) == size1)
			{
				y = y0;
				x++;
				break;
			}
		}
	}
}

/**
 *@brief 显示字符串
 *@param x x坐标
 *@param y y坐标
 *@param chr 字符串起始地址
 *@param size1 字体大小
 */
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t size1)
{
	while ((*chr >= ' ') && (*chr <= '~')) // 判断是不是非法字符!
	{
		OLED_ShowChar(x, y, *chr, size1);
		x += size1 / 2;
		if (x > 128 - size1) // 换行
		{
			x = 2; // x=0
			y += size1;
		}
		chr++;
	}
}

/**
 * @brief 计算m的n次方
 * @param m 底数
 * @param n 幂
 */
uint32_t OLED_Pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
	while (n--)
	{
		result *= m;
	}
	return result;
}

/**
 * @brief 显示数字
 * @param x x坐标
 * @param y y坐标
 * @param num 要显示的数字
 * @param len 数字的位数
 * @param size1 字体大小
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size1)
{
	uint8_t t, temp;
	for (t = 0; t < len; t++)
	{
		temp = (num / OLED_Pow(10, len - t - 1)) % 10;
		if (temp == 0)
		{
			OLED_ShowChar(x + (size1 / 2) * t, y, '0', size1);
		}
		else
		{
			OLED_ShowChar(x + (size1 / 2) * t, y, temp + '0', size1);
		}
	}
	OLED_Refresh();
}

/**
 * @brief 显示汉字
 * @param x x坐标
 * @param y y坐标
 * @param num 汉字序号
 * @param size1 字体大小
 * @note 取模方式 列行式
 */
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t num, uint8_t size1)
{
	uint8_t i, m, n = 0, temp, chr1;
	uint8_t x0 = x, y0 = y;
	uint8_t size3 = size1 / 8;
	while (size3--)
	{
		chr1 = num * size1 / 8 + n;
		n++;
		for (i = 0; i < size1; i++)
		{
			if (size1 == 16)
			{
				temp = Hzk1[chr1][i];
			} // 调用16*16字体
			else if (size1 == 24)
			{
				temp = Hzk2[chr1][i];
			} // 调用24*24字体
			else if (size1 == 32)
			{
				temp = Hzk3[chr1][i];
			} // 调用32*32字体
			else if (size1 == 64)
			{
				temp = Hzk4[chr1][i];
			} // 调用64*64字体
			else
				return;

			for (m = 0; m < 8; m++)
			{
				if (temp & 0x01)
					OLED_DrawPoint(x, y);
				else
					OLED_ClearPoint(x, y);
				temp >>= 1;
				y++;
			}
			x++;
			if ((x - x0) == size1)
			{
				x = x0;
				y0 = y0 + 8;
			}
			y = y0;
		}
	}
}

/**
 * @brief 滚动显示汉字
 * @param num 显示汉字的个数
 * @param space 每一遍显示的间隔
 * @note 该函数为死循环
 */
void OLED_ScrollDisplay(uint8_t num, uint8_t space)
{
	uint8_t i, n, t = 0, m = 0, r;
	while (1)
	{
		if (m == 0)
		{
			OLED_ShowChinese(128, 24, t, 16); // 写入一个汉字保存在OLED_GRAM[][]数组中
			t++;
		}
		if (t == num)
		{
			for (r = 0; r < 16 * space; r++) // 显示间隔
			{
				for (i = 1; i < 144; i++)
				{
					for (n = 0; n < 8; n++)
					{
						OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
					}
				}
				OLED_Refresh();
			}
			t = 0;
		}
		m++;
		if (m == 16)
		{
			m = 0;
		}
		for (i = 1; i < 144; i++) // 实现左移
		{
			for (n = 0; n < 8; n++)
			{
				OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}

/**
 * @brief 设置写入数据的起始位置
 * @param x x坐标
 * @param y y坐标
 */
void OLED_WR_BP(uint8_t x, uint8_t y)
{
	OLED_WR_Byte(0xb0 + y, OLED_CMD); // 设置行起始地址
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f), OLED_CMD);
}

/**
 * @brief 显示浮点数
 * @param x x坐标
 * @param y y坐标
 * @param num 要显示的浮点数
 * @param z_len 整数部分长度
 * @param f_len 小数部分长度
 * @param size2 字体大小
 */
void OLED_Showdecimal(uint8_t x, uint8_t y, float num, uint8_t z_len, uint8_t f_len, uint8_t size2)
{
	uint8_t t, temp, i = 0; // i为负数标志位
	uint8_t enshow;
	int z_temp, f_temp;
	if (num < 0)
	{
		z_len += 1;
		i = 1;
		num = -num;
	}
	z_temp = (int)num;
	// 整数部分
	for (t = 0; t < z_len; t++)
	{
		temp = (z_temp / OLED_Pow(10, z_len - t - 1)) % 10;
		if (enshow == 0 && t < (z_len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (size2 / 2) * t, y, ' ', size2);
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2);
	}
	// 小数点
	OLED_ShowChar(x + (size2 / 2) * (z_len), y, '.', size2);

	f_temp = (int)((num - z_temp) * (OLED_Pow(10, f_len)));
	// 小数部分
	for (t = 0; t < f_len; t++)
	{
		temp = (f_temp / OLED_Pow(10, f_len - t - 1)) % 10;
		OLED_ShowChar(x + (size2 / 2) * (t + z_len) + 5, y, temp + '0', size2);
	}
	if (i == 1) // 如果为负，就将最前的一位赋值‘-’
	{
		OLED_ShowChar(x, y, '-', size2);
		i = 0;
	}
	OLED_Refresh();
}

/**
 * @brief 显示图片
 * @param x0 起点x坐标
 * @param y0 起点y坐标
 * @param x1 终点x坐标
 * @param y1 终点y坐标
 * @param BMP 图片数组
 */
void OLED_ShowPicture(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[])
{
	uint32_t j = 0;
	uint8_t x = 0, y = 0;
	if (y % 8 == 0)
		y = 0;
	else
		y += 1;
	for (y = y0; y < y1; y++)
	{
		OLED_WR_BP(x0, y);
		for (x = x0; x < x1; x++)
		{
			OLED_WR_Byte(BMP[j], OLED_DATA);
			j++;
		}
	}
}

/**
 * @brief 直接显示字符串
 * @param str 字符串
 */
void OLED_Printf(uint8_t str[])
{
	OLED_Clear();
	OLED_ShowString(2, 2, str, 12);
	OLED_Refresh();
}

/**
 * @brief OLED初始化
 */
void OLED_Init(void)
{

	OLED_RES_Clr();
	HAL_Delay(10);
	OLED_RES_Set();

	OLED_WR_Byte(0xAE, OLED_CMD); //--turn off oled panel

	OLED_WR_Byte(0x00, OLED_CMD); /*set lower column address*/
	OLED_WR_Byte(0x10, OLED_CMD); /*set higher column address*/

	OLED_WR_Byte(0xB0, OLED_CMD); /*set page address*/

	OLED_WR_Byte(0xdc, OLED_CMD); /*set display start line*/
	OLED_WR_Byte(0x00, OLED_CMD);

	OLED_WR_Byte(0x81, OLED_CMD); /*contract control*/
	OLED_WR_Byte(0x6f, OLED_CMD); /*128*/
	OLED_WR_Byte(0x20, OLED_CMD); /* Set Memory addressing mode (0x20/0x21) */

	OLED_WR_Byte(0xA4, OLED_CMD); /*Disable Entire Display On (0xA4/0xA5)*/

	OLED_WR_Byte(0xA6, OLED_CMD); /*normal / reverse*/
	OLED_WR_Byte(0xA8, OLED_CMD); /*multiplex ratio*/
	OLED_WR_Byte(0x7F, OLED_CMD); /*duty = 1/64*/

	OLED_WR_Byte(0xD3, OLED_CMD); /*set display offset*/
	OLED_WR_Byte(0x00, OLED_CMD);

	OLED_WR_Byte(0xC0, OLED_CMD);
	OLED_WR_Byte(0xA0, OLED_CMD);

	OLED_WR_Byte(0xD5, OLED_CMD); /*set osc division*/
	OLED_WR_Byte(0x80, OLED_CMD);

	OLED_WR_Byte(0xD9, OLED_CMD); /*set pre-charge period*/
	OLED_WR_Byte(0x1D, OLED_CMD);

	OLED_WR_Byte(0xdb, OLED_CMD); /*set vcomh*/
	OLED_WR_Byte(0x35, OLED_CMD);

	OLED_WR_Byte(0xad, OLED_CMD); /*set charge pump enable*/
	OLED_WR_Byte(0x80, OLED_CMD); /*Set DC-DC enable (a=0:disable; a=1:enable) */
	OLED_Clear();
	OLED_WR_Byte(0xAF, OLED_CMD);
}
