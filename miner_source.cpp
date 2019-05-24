#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <thread>
#include <mutex>
#include <string>
#include <exception>
#include "ConsoleStall.h"
#include "timer.hpp"
#include "Array.hpp"
#include "Block.hpp"

#define BC_MIN_THREAD_COUNT 1
#define BC_MAX_THREAD_COUNT 12

using uchar = unsigned int;
using uint = unsigned int;

//for multithreading
std::mutex mtx;
volatile bool nonceFound;
size_t nonceVal;

uint I;
Timer processTimer, bt;
struct sigaction act;

void threadMine(Block &block, uchar threadCount); 
void mineBlockTS(Block b, uchar threadNum, uchar threadCount);
void setNonce(size_t nonce);
void sigintHandler(int n);

void parseArgs(int argc, char **argv, uchar *diff, uchar *thrCount, uint *chainLen, size_t *startHash);

int main(int argc, char ** argv) {

	//signal handling setup
	act.sa_handler = sigintHandler;
	sigemptyset(&act.sa_mask);
	sigaction(SIGINT, &act, NULL);

	uchar diff = 4, thrCount = 2;
	uint chainLen = 100;
	size_t startHash = 0;

	parseArgs(argc, argv, &diff, &thrCount, &chainLen, &startHash);

	printf("Block difficulty : %u\n", (uint)diff);
	printf("Chain length     : %u\n", chainLen);
	printf("Starting hash    : %zu\n", startHash);
	printf("Thread count     : %u ", (uint)thrCount);
	fflush(stdout);
	
	
	Block b(0, 0, startHash, 0, 0, 0);
	processTimer.start();
	for (I = 0; I < chainLen; I++) {
		b = Block(I, b.getSolvedHash(), diff);
		bt.start();
		threadMine(b, thrCount);
		bt.end();
		printf("\nid=%03u  time-elapsed=%12s  hash=%016zx  nonce=%13zu ", I, bt.toString(Timer::MILLI, Timer::MINUTE).c_str(), b.getSolvedHash(), b.getNonce());
		fflush(stdout);
	}
	processTimer.end();
	printf("\nprogram runtime: %s\n", processTimer.toString(Timer::MILLI).c_str());
	
	return 0;
}

void parseArgs(int argc, char **argv, uchar *diff, uchar *thrCount, uint *chainLen, size_t *startHash) {
	auto helpMsg = [argv] (FILE *stream) {
		fprintf(stream, "Usage: %s [-d diff_level] [-l chain_len] [[-h|H] hash] [-t thread_count]\n", argv[0]);
		fprintf(stream, "\t-d\tsets the required number of leading zeros of the hash\n"
				"\t\tdefault is 4, valid values range from 0 to 16\n");
		fprintf(stream, "\t-l\tsets the number of blocks in the chain\n"
				"\t\tdefault is 100\n");
		fprintf(stream, "\t-h\tsets the first block's hash value\n"
				"\t\tdefault is 0\n");
		fprintf(stream, "\t-H\tidentical to -h except reads a hex value\n");
		fprintf(stream, "\t-t\tsets the number of threads used\n"
				"\t\tdefault is 2\n");
	};
	for (int i = 1; i < argc; i++) {
		std::string opt = argv[i];
		if (opt.compare("--help") == 0) {
			helpMsg(stdout);
			exit(0);
		}
		if (opt.compare("-d") == 0) {
			if (i+1 == argc) {
				fprintf(stderr, "%s: missing argument: d\n", argv[0]);
				helpMsg(stderr);
				exit(1);
			}
			i++;
			if (sscanf(argv[i], "%u", diff) != 1) {
				fprintf(stderr, "%s: invalid argument: %s\n", argv[0], argv[i]);
				helpMsg(stderr);
				exit(1);
			}
		}
		if (opt.compare("-l") == 0) {
			if (i+1 == argc) {
				fprintf(stderr, "%s: missing argument: l\n", argv[0]);
				helpMsg(stderr);
				exit(1);
			}
			i++;
			if (sscanf(argv[i], "%u", chainLen) != 1) {
				fprintf(stderr, "%s: invalid argument: %s\n", argv[0], argv[i]);
				helpMsg(stderr);
				exit(1);
			}
		}
		if (opt.compare("-h") == 0) {
			if (i+1 == argc) {
				fprintf(stderr, "%s: missing argument: h\n", argv[0]);
				helpMsg(stderr);
				exit(1);
			}
			i++;
			if (sscanf(argv[i], "%zu", startHash) != 1) {
				fprintf(stderr, "%s: invalid argument: %s\n", argv[0], argv[i]);
				helpMsg(stderr);
				exit(1);
			}
		}
		if (opt.compare("-H") == 0) {
			if (i+1 == argc) {
				fprintf(stderr, "%s: missing argument: H\n", argv[0]);
				helpMsg(stderr);
				exit(1);
			}
			i++;
			if (sscanf(argv[i], "%zx", startHash) != 1) {
				fprintf(stderr, "%s: invalid argument: %s\n", argv[0], argv[i]);
				helpMsg(stderr);
				exit(1);
			}
		}
		if (opt.compare("-t") == 0) {
			if (i+1 == argc) {
				fprintf(stderr, "%s: missing argument: t\n", argv[0]);
				helpMsg(stderr);
				exit(1);
			}
			i++;
			if (sscanf(argv[i], "%u", thrCount) != 1) {
				fprintf(stderr, "%s: invalid argument: %s\n", argv[0], argv[i]);
				helpMsg(stderr);
				exit(1);
			}
		}
	}
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

void sigintHandler(int n) {
	bt.end();
	processTimer.end();
	printf("\nid=%03u  time-elapsed=%12s\n", I, bt.toString(Timer::MILLI, Timer::MINUTE).c_str());
	printf("program runtime: %s\n", processTimer.toString(Timer::MILLI).c_str());
	exit(0);
}
