#pragma once
#include <cstring>

inline char** mularr_char(int m, int n) //ʹ��һλ����ģ���ά���飬m��n�У�����ռ�����
{
	char** arr = new char* [m]; //������
	arr[0] = new char[m * n]; //������
	memset(arr[0], 0, sizeof(char)*m*n);
	for (int i = 1; i < m; i++) //��һλ����ģ��ɶ�ά����
	{
		arr[i] = arr[i - 1] + n;
	}
	return arr;
}

inline uint64_t find_table_size(unsigned long long int size)
{
    uint64_t mid, left = 0, right = 64;
    while (left <= right)
    {
        mid = (left + right) >> 1;
        //cout << mid << endl;
        //cout << "  " << right << endl;
        //cout << "  " << left << endl;
        if ((1ull << mid) >= size)
        {
            right = mid - 1;
        }
        else
        {
            left = mid + 1;
        }
    }
    return left;
}

inline void rtrim(char *_src) {
    size_t _r_pos = strlen(_src) - 1;
    for (size_t i = _r_pos; i <= _r_pos && isspace(_src[i]); --i) {
        _src[i] = 0;
    }
}