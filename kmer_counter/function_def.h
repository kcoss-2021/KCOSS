#pragma once
inline char** mularr_char(int m, int n) //使用一位数组模拟二维数组，m行n列，储存空间连续
{
	char** arr = new char* [m]; //开辟行
	arr[0] = new char[m * n]; //开辟列
	memset(arr[0], 0, sizeof(char)*m*n);
	for (int i = 1; i < m; i++) //将一位数组模拟成二维数组
	{
		arr[i] = arr[i - 1] + n;
	}
	return arr;
}

inline int find_table_size(unsigned long long int size)
{
    int mid, left = 0, right = 64;
    while (left <= right)
    {
        mid = (left + right) >> 1;
        //cout << mid << endl;
        //cout << "  " << right << endl;
        //cout << "  " << left << endl;
        if (pow(2, mid) >= size)
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