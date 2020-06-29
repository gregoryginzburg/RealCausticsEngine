#ifndef DRAW_CIRCLES_H
#define DRAW_CIRCLES_H
#include <vector>
#include <mutex>
#include "Color.h"
std::mutex mtx;
inline void draw_circle1(std::vector<std::vector<color>>& c, int pixel_x, int pixel_y, int width, int height)
{
	

	if (pixel_x < 3 || pixel_x >(width - 3) or pixel_y < 3 or pixel_y >(height - 3))
	{
		c[pixel_y][pixel_x] += color(20);
	}
	else
	{
		//0
		c[pixel_y][pixel_x] += color(20);

		//1
		c[pixel_y + 1][pixel_x] += color(20);
		//2
		
		c[pixel_y][pixel_x + 1] += color(20);
		//3
		
		c[pixel_y - 1][pixel_x] += color(20);
		//4
		
		c[pixel_y][pixel_x - 1] += color(20);


		//5
		c[pixel_y + 1][pixel_x + 1] += color(17);
		
		//6
		c[pixel_y - 1][pixel_x + 1] += color(17);
		
		//7
		c[pixel_y - 1][pixel_x - 1] += color(17);
		
		//8
		c[pixel_y + 1][pixel_x - 1] += color(17);
		

		//9
		c[pixel_y + 2][pixel_x] += color(15);
		
		//10
		c[pixel_y][pixel_x + 2] += color(15);
		
		//11
		c[pixel_y - 2][pixel_x] += color(15);
		
		//12
		c[pixel_y][pixel_x - 2] += color(15);
		

		//13
		c[pixel_y + 2][pixel_x + 1] += color(13);
		
		//14
		c[pixel_y + 1][pixel_x + 2] += color(13);
		
		//15
		c[pixel_y - 1][pixel_x + 2] += color(13);
		
		//16
		c[pixel_y - 2][pixel_x + 1] += color(13);
		
		//17
		c[pixel_y - 2][pixel_x - 1] += color(13);
		
		//18
		c[pixel_y - 1][pixel_x - 2] += color(13);
		
		//19
		c[pixel_y + 1][pixel_x - 2] += color(13);
		
		//20
		c[pixel_y + 2][pixel_x - 1] += color(13);
		
		
		//if (temp_max > max)
		//{
		//	max = temp_max;
		//}
	}

}

inline void draw_circle2(std::vector<std::vector<color>>& c, int pixel_x, int pixel_y, int width, int height)
{

	if (pixel_x < 3 || pixel_x >(width - 3) or pixel_y < 3 or pixel_y >(height - 3))
	{
		c[pixel_y][pixel_x] += color(50);
	}
	else
	{
		//0
		c[pixel_y][pixel_x] += color(50);

		//1
		c[pixel_y + 1][pixel_x] += color(45);
		//2
		
		c[pixel_y][pixel_x + 1] += color(45);
		//3
		
		c[pixel_y - 1][pixel_x] += color(45);
		//4
		
		c[pixel_y][pixel_x - 1] += color(45);
		

		//5
		c[pixel_y + 1][pixel_x + 1] += color(40);
		
		//6
		c[pixel_y - 1][pixel_x + 1] += color(40);
		
		//7
		c[pixel_y - 1][pixel_x - 1] += color(40);
		
		//8
		c[pixel_y + 1][pixel_x - 1] += color(40);
		
		
	}
}
inline void draw_square(std::vector<std::vector<color>>& c, int pixel_x, int pixel_y, int width, int height)
{
	if (pixel_x < 3 || pixel_x >(width - 3) or pixel_y < 3 or pixel_y >(height - 3))
	{
		c[pixel_y][pixel_x] += color(50);
	}
	else
	{

		c[pixel_y][pixel_x] += color(50);
		c[pixel_y][pixel_x - 1] += color(50);
		c[pixel_y - 1][pixel_x - 1] += color(50);
		c[pixel_y - 1][pixel_x] += color(50);
	}
}





#endif