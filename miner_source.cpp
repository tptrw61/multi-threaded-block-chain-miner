#include <cstdio>
#include <thread>
#include <mutex>
#include "ConsoleStall.h"
#include "timer.hpp"
#include "Array.hpp"
#include "Block.hpp"

#define GET_MAX_THREADS() std::thread::hardware_concurrency()
const unsigned BC_MIN_THREAD_COUNT = 1;
const unsigned BC_MAX_THREAD_COUNT = 
	GET_MAX_THREADS() == 0 ? 2 : GET_MAX_THREADS();

using uchar = unsigned char;
using uint = unsigned int;

//for multithreading
std::mutex mtx;
volatile bool nonceFound;
size_t nonceVal;

void threadMine(Block &block, uchar threadCount); 
void mineBlockTS(Block b, uchar threadNum, uchar threadCount);
void setNonce(size_t nonce);


int main() {
	
	uchar diff, thrCount;
	uint chainLen;
	size_t startHash;
	
	do {
		printf("Block difficulty: ");
		scanf("%hhu", &diff);
	} while (diff > 16);
	do {
		printf("Chain length: ");
		scanf("%u", &chainLen);
	} while (chainLen == 0);
	printf("Starting hash: ");
	scanf("%zu", &startHash);
	do {
		printf("Thread count: ");
		scanf("%hhu", &thrCount);
	} while (thrCount < BC_MIN_THREAD_COUNT || thrCount > BC_MAX_THREAD_COUNT);
	
	Block b(0, 0, startHash, 0, 0, 0);
	Timer processTimer, bt;
	processTimer.start();
	for (uint i = 0; i < chainLen; i++) {
		b = Block(i, b.getSolvedHash(), diff);
		bt.start();
		threadMine(b, thrCount);
		bt.end();
		printf("id=%03u  time-elapsed=%12s  hash=%016zx  nonce=%13zu\n", i, bt.toString(Timer::MILLI, Timer::MINUTE).c_str(), b.getSolvedHash(), b.getNonce());
	}
	processTimer.end();
	printf("program runtime: %s\n", processTimer.toString(Timer::MILLI).c_str());
	
	continueConsole(1);
	return 0;
}

void threadMine(Block &block, uchar threadCount) {
	nonceFound = false;
	Array<std::thread> threadArr(threadCount);
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
