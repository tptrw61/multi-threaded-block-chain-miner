#include <cstdlib>
#include <cstdio>
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

void threadMine(Block &block, uchar threadCount); 
void mineBlockTS(Block b, uchar threadNum, uchar threadCount);
void setNonce(size_t nonce);

void parseArgs(int argc, char **argv, uchar *diff, uchar *thrCount, uint *chainLen, size_t *startHash);

int main(int argc, char ** argv) {

	/*
	auto usageMsg = [argv] () -> int {
		fprintf(stderr, "Usage: %s [--help]|[[-d DIFF(=4)] [-l CHAIN_LEN(=100)] [-h HASH(=0)] [-t THREAD_CT(=2)]]\n", argv[0]);
		fprintf(stderr, "\tThe order of the flags does not matter.\n");
		fprintf(stderr, "\tVAR_NAME(=#): # is the default value of VAR_NAME.\n");
		fprintf(stderr, "\tDIFF must be in the range 0..16 (inclusive). Larger values are harder.\n");
		fprintf(stderr, "\tTHREAD_CT must be in range %d..%d (inclusive).\n", BC_MIN_THREAD_COUNT, BC_MAX_THREAD_COUNT);
		fprintf(stderr, "\tCHAIN_LEN must be at least 1.\n\tHASH must be at least 0.\n");
		return 1;
	};
	
	if (argc == 2 && std::string(argv[1]).compare("--help") == 0) {
		printf("Usage: %s [--help]|[[-d DIFF(=4)] [-l CHAIN_LEN(=100)] [-h HASH(=0)] [-t THREAD_CT(=2)]]\n", argv[0]);
		printf("\tThe order of the flags does not matter.\n");
		printf("\tVAR_NAME(=#): # is the default value of VAR_NAME.\n");
		printf("\tDIFF must be in the range 0..16 (inclusive). Larger values are harder.\n");
		printf("\tTHREAD_CT must be in range %d..%d (inclusive).\n", BC_MIN_THREAD_COUNT, BC_MAX_THREAD_COUNT);
		printf("\tCHAIN_LEN must be at least 1.\n\tHASH must be at least 0.\n");
		return 0;
	}
	
	if ((argc - 1) % 2 == 1 || argc - 1 > 8) { // extra/missing argument. max of 8 args (4 flag-val pairs)
		fprintf(stderr, "Error: invalid argument count (arguements must be in pairs, up to 4 pairs)\n");
		return usageMsg();
	}
	*/
	uchar diff = 4, thrCount = 2;
	uint chainLen = 100;
	size_t startHash = 0;

	parseArgs(argc, argv, &diff, &thrCount, &chainLen, &startHash);
	/*
	char flagsUsed = 0;
	for (int i = 1; i < argc; i += 2) {
		std::string flg = argv[i], val = argv[i + 1];
		if (flg.compare("-d") == 0) {
			if (flagsUsed & 1) {
				fprintf(stderr, "Error: cannot use '-d' more than once\n");
				return 1;
			}
			try {
				if ((diff = (uchar)std::stoi(val)) > 16) {
					fprintf(stderr, "Error: '%d' is not a valid value for '-d'\n", (int)diff);
					return usageMsg();
				}
				flagsUsed += 1;
			} catch (std::exception e) {
				fprintf(stderr, "Error: '%s' is not a valid value for '-d'\n", val.c_str());
				return usageMsg();
			}
		} else if (flg.compare("-l") == 0) {
			if (flagsUsed & 2) {
				fprintf(stderr, "Error: cannot use '-l' more than once\n");
				return 1;
			}
			try {
				chainLen = (uint)std::stoul(val);
				flagsUsed += 2;
			} catch (std::exception e) {
				fprintf(stderr, "Error: '%s' is not a valid value for '-l'\n", val.c_str());
				return usageMsg();
			}
		} else if (flg.compare("-h") == 0) {
			if (flagsUsed & 4) {
				fprintf(stderr, "Error: cannot use '-h' more than once\n");
				return 1;
			}
			try {
				startHash = (size_t)std::stoull(val);
				flagsUsed += 4;
			} catch (std::exception e) {
				fprintf(stderr, "Error: '%s' is not a valid value for '-h'\n", val.c_str());
				return usageMsg();
			}
		} else if (flg.compare("-t") == 0) {
			if (flagsUsed & 8) {
				fprintf(stderr, "Error: cannot use '-t' more than once\n");
				return 1;
			}
			try {
				thrCount = (uchar)std::stoi(val);
				if (thrCount < BC_MIN_THREAD_COUNT || thrCount > BC_MAX_THREAD_COUNT) {
					fprintf(stderr, "Error: '%d' is not a valid value for '-t'\n", (int)thrCount);
					return usageMsg();
				}
				flagsUsed += 8;
			} catch (std::exception e) {
				fprintf(stderr, "Error: '%s' is not a valid value for '-t'\n", val.c_str());
				return usageMsg();
			}
		} else {
			fprintf(stderr, "Error: '%s' is not a valid argument\n", flg.c_str());
			return usageMsg();
		}
	}
	*/

	printf("Block difficulty : %u\n", (uint)diff);
	printf("Chain length     : %u\n", chainLen);
	printf("Starting hash    : %zu\n", startHash);
	printf("Thread count     : %u\n", (uint)thrCount);
	
	
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
