#pragma once
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