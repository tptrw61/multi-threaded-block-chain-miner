#include <cstdio>
#include <thread>
#include <mutex>
#include "ConsoleStall.h"
#include "Array.hpp"
#include "Block.hpp"

#define BC_MIN_THREAD_COUNT 1
#define BC_MAX_THREAD_COUNT 12

using uchar = unsigned char;
using uint = unsigned int;

//for multithreading
std::mutex mtx;
bool nonceFound;
size_t nonceVal;

void threadMine(Block &block, uchar threadCount); 
void mineBlockTS(Block b, uchar threadNum, uchar threadCount);
void setNonce(size_t nonce);


int main() {
	
	uchar diff, thrCount;
	uint chainLen;
	size_t startHash;
	
	printf("Block difficulty: ");
	scanf("%hhu", &diff);
	printf("Chain length: ");
	scanf("%u", &chainLen);
	printf("Starting hash: ");
	scanf("%zu", &startHash);
	printf("Thread count: ");
	scanf("%hhu", &thrCount);
	
	
	
	continueConsole(1);
	return 0;
}

void threadMine(Block &block, uchar threadCount) {
	nonceFound = false;
	Array<std::thread> threadArr;
	uchar i = 0;
	for (auto &thr : threadArr)
		thr = std::thread(mineBlockTS, block, i++, threadCount);
	for (auto &thr : threadArr)
		thr.join();
	if (nonceFound)
		block.tryNonce(nonceVal); //plug in found nonce
	else
		block.setNoSolution();
}

void mineBlockTS(Block b, uchar threadNum, uchar threadCount) {
	for (size_t i = (size_t)threadNum; i <= (size_t)-1 && !nonceFound; i += threadCount) {
		if (b.tryNonce(i)) {
			setNonce(i); //sets nonceVal and is mutex locked
		}
		if (i > i + threadCount) { //overflow
			return;
		}
	} 
}

void setNonce(size_t nonce) {
	std::lock_guard<std::mutex> guard(mtx);
	if (!nonceFound) {
		nonceFound = true;
		nonceVal = nonce;
	}
}