#include "counter_less_than_14.h"

void counter_less_than_14::count(char *reads_address) {
    //uint_32 kmers_bit_tmp;
    int arr_num = 0;

    string kmer, reads;

    //int k_flag; //用于判断kmer是否完整
    //uint_32 kmer_tmp; //存放拼接好的kmer
    //uint_32 new_bit; //用于存放刚拿到的bit

//	char ** reads_address;//临时用一下等下要改

    bool flag = false; //标志是否跨行了
    for (int i = 0; i < k; i++) { //检测是否跨行
        if (reads_address[i] == '\n') {
            flag = true;
        }
    }

    int length = strlen(reads_address);
    int i = 0;
    if (!flag) { //如果没有跨行
        i = 1;
    }
//        cout << reads_address[i] << endl;
    int k_flag = 0;           //用于判断kmer是否完整
    uint_32 new_bit = 0;   //用于存放刚拿到的bit
    uint_32 kmer_tmp = 0;  //存放拼接好的kmer
    for (; i < length; i++) {
        switch (reads_address[i]) {
            case 'A':
            case 'a':
                new_bit = 0b00;
                //new_char = "A";
                break;
            case 'C':
            case 'c':
                new_bit = 0b01;
                //new_char = "C";
                break;
            case 'G':
            case 'g':
                new_bit = 0b10;
                //new_char = "G";
                break;
            case 'T':
            case 't':
                new_bit = 0b11;
                //new_char = "T";
                break;
            case '\n':
                continue;
            default:
//                cout << (int)reads_address[j] << endl;
                k_flag = -1;
                //i = i + k;
                break;
        }
        if (k_flag == -1) {
            k_flag = 0;
            kmer_tmp = 0;
        } else if (k_flag < (k - 1)) {
            kmer_tmp = (kmer_tmp << 2) + new_bit;
            //kmer_tmp_char = kmer_tmp_char + new_char;
            k_flag++;
        } else //拼接完整！！！！！！！！！！！！！！！！！
        {
            kmer_tmp = (kmer_tmp << 2) + new_bit;

            /********************************************************************************************************************************************************************************************/

            root_table[kmer_tmp]++;

            /********************************************************************************************************************************************************************************************/
            kmer_tmp = kmer_tmp & get_end_2k_2;
        }
    }

    address_array->enqueue(reads_address);
}

void counter_less_than_14::print() {
    //fstream print_less_than_14 = fstream("counter_less_than_14.dat", ios::out | ios::binary);
    //int length = (int)pow(4, k);
    //uint_64 sum = 0;
    //for (int i = 0; i < length; i++)
    //{
    //	sum = sum + root_table[i];
    //}
    //cout << "总条数cout " << cout_all << endl;
    //cout << "总条数sum：" << sum << endl;
    //cout << "length" << length << endl;
    //print_less_than_14.write((char*)root_table, (int)pow(4, k) * 4);
    //print_less_than_14.close();

    int print_less_than_14 = open((filename + ".data").c_str(), O_RDWR | O_CREAT, 0664);
    const size_t len = (int) pow(4, k) * 4;
    ftruncate(print_less_than_14, len);
    uint_32 *mapped = (uint_32 *) mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, print_less_than_14, 0);
    memcpy(mapped, root_table, len);
    munmap((void *) mapped, len);
    close(print_less_than_14);

}

