#include <iostream>
#include  <iomanip>
#include <cmath>
#include <vector>
struct cache_shape {
	int tag = 0;
	int data = 0;
	bool V_bit = false;
};
using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(64*1024)

unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */

unsigned int rand_()
{
	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * (m_w & 65535) + (m_w >> 16);
	return (m_z << 16) + m_w;  /* 32-bit result */
}

unsigned int memGen1()
{
	static unsigned int addr = 0;
	return (addr++) % (DRAM_SIZE);
}

unsigned int memGen2()
{
	static unsigned int addr = 0;
	return  rand_() % (128 * 1024);
}

unsigned int memGen3()
{
	return rand_() % (DRAM_SIZE);
}

unsigned int memGen4()
{
	static unsigned int addr = 0;
	return (addr++) % (1024);
}

unsigned int memGen5()
{
	static unsigned int addr = 0;
	return (addr++) % (1024 * 64);
}

unsigned int memGen6()
{
	static unsigned int addr = 0;
	return (addr += 256) % (DRAM_SIZE);
}

int cacheSim(unsigned int addr, cache_shape** myarray, char cache_t, int offset, int index_bits, int tag, int set, vector <int> indexing_array, int &array_index)
{
	if (cache_t == 'd' || cache_t == 'D'){
		unsigned int index = ((addr) << tag);
		index = index >> tag;
		index = index >> offset;
		//cout << index << endl;
		unsigned int tag_num = (addr >> (index_bits + offset));
		if (myarray[index][0].V_bit == true){
			if (myarray[index][0].tag == tag_num)
				return 1;
			else{
				myarray[index][0].V_bit = true;
				myarray[index][0].tag = tag_num;
				myarray[index][0].data = addr;
				return 0;
			}
		}
		else{
			myarray[index][0].V_bit = true;
			myarray[index][0].tag = tag_num;
			myarray[index][0].data = addr;
			return 0;
		}
	}
	else
	if (cache_t == 'f' || cache_t == 'F'){
		bool flag2 = true;
		int tag_num = addr >> offset;
		int power = pow(2, index_bits);
		int save_index;
		for (int i = 0; i < power; i++){
			if (myarray[i][0].V_bit == true){
				if (myarray[i][0].tag == tag_num)
					return 1;
			}

			else{
				if (flag2){
					flag2 = false;   //CACHE IS NOT FULL
					save_index = i; //first index that is empty
				}
			}
		}
		//////////////////////////////////////////RANDOM REPLACEMENT START
		/*	if (!flag2){
		myarray[save_index][0].V_bit = true;
		myarray[save_index][0].tag = tag_num;
		myarray[save_index][0].data = addr;
		}
		else {	//CACHE IS FULL
		save_index = rand() % power;
		myarray[save_index][0].V_bit = true;
		myarray[save_index][0].tag = tag_num;
		myarray[save_index][0].data = addr;
		}*/
		/////////////////////////////////////////RANDOM REPLACEMENT END

		/////////////////////////////////////////////////////////////////FIFO REPLACEMENT START
		if (!flag2){
			myarray[save_index][0].V_bit = true;
			myarray[save_index][0].tag = tag_num;
			myarray[save_index][0].data = addr;
			indexing_array.push_back(save_index);
			return 0;
		}
		else { //CACHE IS FULL
			save_index = indexing_array[array_index++];
			array_index = array_index % power;
			myarray[save_index][0].V_bit = true;
			myarray[save_index][0].tag = tag_num;
			myarray[save_index][0].data = addr;
			return 0;
		}


		/////////////////////////////////////////////////////////////////FIFO REPLACEMENT END
	}
	else
	if (cache_t == 's' || cache_t == 'S')	{
		unsigned int index = addr << tag;
		index = index >> tag;
		index = index >> offset;
		//cout << index << endl;
		bool flag2 = true;
		int save_index;
		unsigned int tag_num = (addr >> (index_bits + offset));
		for (int i = 0; i < set; i++){
			if (myarray[index][i].V_bit == true){
				if (myarray[index][i].tag == tag_num)
					return 1;
			}
			else{
				if (flag2){
					flag2 = false;   //SET IS NOT FULL
					save_index = i; //first index that is empty
				}
			}
		}
		//////////////////////////////////////////RANDOM REPLACEMENT START
		if (!flag2){
			myarray[index][save_index].V_bit = true;
			myarray[index][save_index].tag = tag_num;
			myarray[index][save_index].data = addr;
		}
		else{ //SET IS FULL

			save_index = rand() % set;
			myarray[index][save_index].V_bit = true;
			myarray[index][save_index].tag = tag_num;
			myarray[index][save_index].data = addr;
		}
		/////////////////////////////////////////RANDOM REPLACEMENT END
	}
	return 0;
}

int main()
{
	int inst = 0;
	int r;
	int set = 1;
	int block_size_in_bits;
	int cache_size, index_size_in_bits, block_size, tag;
	vector <int> indexing_array;
	int array_indexing = 0;
	char cache_t;
	double avg_sum = 0;
	cache_shape** myarray;
	unsigned int addr;
	srand(time(NULL));
	//////////////////////////////////////////////////////
	cout << " Enter the type of cache you want to simulate " << endl;
	cin >> cache_t;
	cout << " Enter the block_size in bytes " << endl;
	cin >> block_size;
	cout << " Enter the Cache Size " << endl;
	cin >> cache_size;
	if (cache_t == 'd' || cache_t == 'D') //Direct Mapped Cache
		index_size_in_bits = log2(cache_size / block_size);
	else
	if (cache_t == 'f' || cache_t == 'F')  //Fully Associative Cache
		index_size_in_bits = log2(cache_size / block_size);
	else if (cache_t == 's' || cache_t == 'S') //Set Associative Cache
	{
		cout << " Enter the number of ways " << endl;
		cin >> set;
		while (set == 0 || (set & (set - 1))){
			cout << " You did not enter a number of ways that is a power of 2\n Re-enter " << endl;
			cin >> set;
		}
		index_size_in_bits = log2((cache_size / block_size) / set);
	}
	else
		return 0;
	////////////////////////////////////////////////////////
	block_size_in_bits = log2(block_size);
	tag = 32 - block_size_in_bits - index_size_in_bits;
	cout << block_size_in_bits << endl << index_size_in_bits << endl;
	int rows = pow(2, index_size_in_bits);
	myarray = new cache_shape*[rows];
	for (int i = 0; i < rows; i++)
		myarray[i] = new cache_shape[set];
	cout << "Cache Simulator\n";
	// change the number of iterations into 10,000,000
	for (; inst<10000000; inst++){
		addr = memGen6();
		r = cacheSim(addr, myarray, cache_t, block_size_in_bits, index_size_in_bits, tag, set, indexing_array, array_indexing);
		if (r == 1)
			avg_sum++;
	}
	cout << " Number of hits is " << avg_sum << endl;
	avg_sum = avg_sum / double(10000000);
	cout << " HIT Ratio:\t" << avg_sum << "\n Miss Ratio:\t" << 1 - avg_sum << endl;
	system("pause");
	return 0;
}