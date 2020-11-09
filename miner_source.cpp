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

#define GET_MAX_THREADS() std::thread::hardware_concurrency()
const unsigned BC_MIN_THREAD_COUNT = 1;
const unsigned BC_MAX_THREAD_COUNT = GET_MAX_THREADS() == 0 ? BC_MIN_THREAD_COUNT : GET_MAX_THREADS();

using uchar = unsigned int;
using uint = unsigned int;

constexpr uchar DEFAULT_DIFF = 4;
constexpr ullint DEFAULT_HASH = 0;
constexpr uint DEFAULT_LEN = 100;
constexpr uchar DEFAULT_THREADS = 2;

//for multithreading
std::mutex mtx;
volatile bool nonceFound;
ullint nonceVal;

uint I;
Timer processTimer, bt;
struct sigaction act;

void threadMine(Block &block, uchar threadCount); 
void mineBlockTS(Block b, uchar threadNum, uchar threadCount);
void setNonce(ullint nonce);
void sigintHandler(int n);

void parseArgs(int argc, char **argv, uchar *diff, uchar *thrCount, uint *chainLen, ullint *startHash);

int main(int argc, char ** argv) {

	//signal handling setup
	act.sa_handler = sigintHandler;
	sigemptyset(&act.sa_mask);
	sigaction(SIGINT, &act, NULL);

	uchar diff = DEFAULT_DIFF;
	uchar thrCount;
	if (BC_MAX_THREAD_COUNT >= DEFAULT_THREADS)
		thrCount = DEFAULT_THREADS;
	else
		thrCount = BC_MAX_THREAD_COUNT;
	uint chainLen = DEFAULT_LEN;
	ullint startHash = DEFAULT_HASH;

	parseArgs(argc, argv, &diff, &thrCount, &chainLen, &startHash);

	printf("Block difficulty : %u\n", (uint)diff);
	printf("Chain length     : %u\n", chainLen);
	printf("Starting hash    : %llu\n", startHash);
	printf("Thread count     : %u ", (uint)thrCount);
	fflush(stdout);
	
	
	Block b(0, 0, startHash, 0, 0, 0);
	processTimer.start();
	for (I = 0; I < chainLen; I++) {
		b = Block(I, b.getSolvedHash(), diff);
		bt.start();
		threadMine(b, thrCount);
		bt.end();
		printf("\nid=%03u  time-elapsed=%12s  hash=%016llx  nonce=%13llu ", I, bt.toString(Timer::MILLI, Timer::MINUTE).c_str(), b.getSolvedHash(), b.getNonce());
		fflush(stdout);
	}
	processTimer.end();
	printf("\nprogram runtime: %s\n", processTimer.toString(Timer::MILLI).c_str());
	
	return 0;
}

void parseArgs(int argc, char **argv, uchar *diff, uchar *thrCount, uint *chainLen, ullint *startHash) {
	auto helpMsg = [argv] (FILE *stream) {
		fprintf(stream, "Usage: %s [-d diff_level] [-l chain_len] [[-h|H] hash] [-t thread_count]\n", argv[0]);
		fprintf(stream, "\t-d\tsets the required number of leading zeros of the hash\n"
				"\t\tdefault is %hhu, valid values range from 0 to 16\n", DEFAULT_DIFF);
		fprintf(stream, "\t-l\tsets the number of blocks in the chain\n"
				"\t\tdefault is %u\n", DEFAULT_LEN);
		fprintf(stream, "\t-h\tsets the first block's hash value\n"
				"\t\tdefault is %llu\n", DEFAULT_HASH);
		fprintf(stream, "\t-H\tidentical to -h except reads a hex value\n");
		fprintf(stream, "\t-t\tsets the number of threads used\n"
				"\t\tdefault is %hhu, valid values range from %u to %u\n", DEFAULT_THREADS, BC_MIN_THREAD_COUNT, BC_MAX_THREAD_COUNT);
		fprintf(stream, "\t-T\tgets the maximum number of threads the processor supports\n");
	};
	bool getThreads = false;
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
			if (*diff > 16) {
				fprintf(stderr, "%s: invalid difficulty: %s\n", argv[0], argv[i]);
				helpMsg(stderr);
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
			if (sscanf(argv[i], "%llu", startHash) != 1) {
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
			if (sscanf(argv[i], "%llx", startHash) != 1) {
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
			if (*thrCount < BC_MIN_THREAD_COUNT || *thrCount > BC_MAX_THREAD_COUNT) {
				fprintf(stderr, "%s: invalid count: %s is not between %u and %u\n", argv[0], argv[i], BC_MIN_THREAD_COUNT, BC_MAX_THREAD_COUNT);
				helpMsg(stderr);
				exit(1);
			}
		}
		if (opt.compare("-T") == 0) {
			getThreads = true;
		}
	}
	if (getThreads) {
		printf("Max threads: %u\n", BC_MAX_THREAD_COUNT);
		exit(0);
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
	for (ullint i = (ullint)threadNum; i <= (ullint)-1 && !nonceFound; i += threadCount) {
		if (b.tryNonce(i)) {
			setNonce(i); //sets nonceVal and is mutex locked
		}
		if (i > i + threadCount) { //overflow
			return;
		}
	} 
}

void setNonce(ullint nonce) {
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
