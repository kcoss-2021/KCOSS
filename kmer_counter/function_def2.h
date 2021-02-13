#pragma once
inline uint_32** mularr_uint_32(int m, int n) //使用一位数组模拟二维数组，m行n列，储存空间连续
{
	uint_32** arr = new uint_32 * [m]; //开辟行
	arr[0] = new uint_32[m * n]; //开辟列
	memset(arr[0], 0, sizeof(arr[0]));
	for (int i = 1; i < m; i++) //将一位数组模拟成二维数组
	{
		arr[i] = arr[i - 1] + n;
	}
	return arr;
}