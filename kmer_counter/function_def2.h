#pragma once
inline uint_32** mularr_uint_32(int m, int n) //ʹ��һλ����ģ���ά���飬m��n�У�����ռ�����
{
	uint_32** arr = new uint_32 * [m]; //������
	arr[0] = new uint_32[m * n]; //������
	memset(arr[0], 0, sizeof(arr[0]));
	for (int i = 1; i < m; i++) //��һλ����ģ��ɶ�ά����
	{
		arr[i] = arr[i - 1] + n;
	}
	return arr;
}