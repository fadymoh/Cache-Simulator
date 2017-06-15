#include <iostream>
#include  <iomanip>
#include <cmath>
struct cache_shape {

	int tag = 0;
	int data = 0;
	bool V_bit = false;
};
using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(64*1024)


enum cacheResType { MISS = 0, HIT = 1 };

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
	return  rand_() % (128*1024);
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


// Cache Simulator
int cacheSim(unsigned int addr, cache_shape** myarray, char cache_t, int offset, int index_bits, int tag)
{
	// This function accepts the memory address for the read and 
	// returns whether it caused a cache miss or a cache hit
	bool flag = true;
	// The current implementation assumes there is no cache; so, every transaction is a miss
	while (flag)
	{
			if (cache_t == 'd' || cache_t == 'D')
		{
				unsigned int index = ((addr) << tag);
				index = index >> tag;
				index = index >> offset;
				unsigned int tag = (addr >> (index_bits + offset));
			//	cout <<  tag << endl;
				//cout << dec << index << endl << tag << endl;
			
				if (myarray[index][1].V_bit)
				{
					if (myarray[index][1].tag == tag)
					{
						//cout << " same tag " << endl;
						return 1;
					}
					else
					{
						myarray[index][1].V_bit = true;
						myarray[index][1].tag = tag;
						myarray[index][1].data = addr;
						return 0;
					}
				}
				else
				{
					myarray[index][1].V_bit = true;
					myarray[index][1].tag = tag;
					myarray[index][1].data = addr;
					return 0;
				}
		}
		else
			if (cache_t == 'f' || cache_t == 'F')
			{

			}
			else
				if (cache_t == 's' || cache_t == 'S')
				{

				}
	}
	return 0;
}

char *msg[2] = { "Miss","Hit" };

int main()
{
	int inst = 0;
	int r;
	int set = 1;
	int block_size_in_bits;
	int cache_size, index_size_in_bits, block_size, tag;
	char cache_t;
	double avg_sum = 0;
	cache_shape** myarray;
	unsigned int addr;
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
			index_size_in_bits = 0;
		else if (cache_t == 's' || cache_t == 'S') //Set Associative Cache
		{
			cout << " Enter the number of ways " << endl;
			cin >> set;
			while (set % 2 != 0)
			{
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
	myarray = new cache_shape*[pow(2,index_size_in_bits)];
	cout << pow(2, index_size_in_bits) << endl;
	for (int i = 0; i < pow(2,index_size_in_bits); i++)
		myarray[i] = new cache_shape[set];
	cout << "Cache Simulator\n";
	// change the number of iterations into 10,000,000
	for (; inst<100000; inst++)
	{

		addr = memGen2();

		r = cacheSim(addr, myarray, cache_t, block_size_in_bits, index_size_in_bits, tag);
		if (r == 1)
		{
			cout << "HIT" << endl;
			avg_sum++;
		}
		else
			cout << "MISS"<< endl;

	}
	avg_sum = avg_sum / float(100000);
	cout << avg_sum << endl;
	system("pause");
	return 0;
}