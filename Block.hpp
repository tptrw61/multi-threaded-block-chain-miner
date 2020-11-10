#pragma once

#include <ctime>
#include <functional>
#include <string>

#define DEFAULT_DIFFICULTY 3

using ullint = unsigned long long int;

const ullint DIFFICULTY_VALUES[] = {
	0xffffffffffffffff,
	0x0fffffffffffffff,
	0x00ffffffffffffff,
	0x000fffffffffffff,
	0x0000ffffffffffff,
	0x00000fffffffffff,
	0x000000ffffffffff,
	0x0000000fffffffff,
	0x00000000ffffffff,
	0x000000000fffffff,
	0x0000000000ffffff,
	0x00000000000fffff,
	0x000000000000ffff,
	0x0000000000000fff,
	0x00000000000000ff,
	0x000000000000000f,
	0x0000000000000000
};



class Block {

	unsigned id;
	unsigned difficulty;
	ullint previousHash;
	ullint solvedHash;
	ullint nonce;
	clock_t timeCreated;
	clock_t timeSolved;
	ullint threshold;
	bool nSol; //no solutions, true only if the hash is impossible at current difficulty

	//static std::hash<std::string> hasher;

public:

	Block();
	Block(unsigned id, ullint previousHash, unsigned difficulty = DEFAULT_DIFFICULTY);
	Block(unsigned id, ullint previousHash, ullint solvedHash, ullint nonce, clock_t timeCreated, clock_t timeSolved, unsigned difficulty = DEFAULT_DIFFICULTY);

	bool isSolved() const;
	bool tryNonce(ullint nonce);
	bool tryNonce(ullint nonce) const;

	bool operator==(Block) const;
	bool operator!=(Block) const;
	bool operator>=(Block) const;
	bool operator<=(Block) const;
	bool operator>(Block) const;
	bool operator<(Block) const;

	//if the miner has checked all possible nonces andthere is no solution at this difficulty
	//sets timeSolved, sets solvedHash to hash of previousHash, sets nonce to 0, and sets nSol to true
	void setNoSolution();

	unsigned getId() const;
	unsigned getDifficulty() const;
	ullint getPreviousHash() const;
	ullint getSolvedHash() const;
	ullint getNonce() const;
	clock_t getTimeCreated() const;
	clock_t getTimeSolved() const;
	bool hasNoSolution() const;

	bool editDifficulty(unsigned difficulty);

	std::string toString(bool abridged = true) const;

};

//0 => no solution
//1 => found solution (mined)
//2 => solution already found (already mined)
int mineBlock(Block &block, ullint nonceStart = 0, int nonceIncrement = 1, ullint nonceEnd = -1);

#ifdef COMPARE_BLOCK_STRUCT
class CompareBlock {
public:

	static const int COMPARE_DEFAULT = 0;
	static const int COMPARE_ID = 1;
	static const int COMPARE_NONCE = 2;

	int mode = COMPARE_DEFAULT;

	bool eq(Block b1, Block b2) {
		if (mode == COMPARE_DEFAULT)
			return b1 == b2;
		if (mode == COMPARE_ID)
			return b1.getId() == b2.getId();
		if (mode == COMPARE_NONCE)
			return b1.getNonce() == b2.getNonce();
		return false;
	};

	bool ne(Block b1, Block b2) {
		if (mode == COMPARE_DEFAULT)
			return b1 != b2;
		if (mode == COMPARE_ID)
			return b1.getId() != b2.getId();
		if (mode == COMPARE_NONCE)
			return b1.getNonce() != b2.getNonce();
		return false;
	};

	bool ge(Block b1, Block b2) {
		if (mode == COMPARE_DEFAULT)
			return b1 >= b2;
		if (mode == COMPARE_ID)
			return b1.getId() >= b2.getId();
		if (mode == COMPARE_NONCE)
			return b1.getNonce() >= b2.getNonce();
		return false;
	};

	bool le(Block b1, Block b2) {
		if (mode == COMPARE_DEFAULT)
			return b1 <= b2;
		if (mode == COMPARE_ID)
			return b1.getId() <= b2.getId();
		if (mode == COMPARE_NONCE)
			return b1.getNonce() <= b2.getNonce();
		return false;
	};

	bool gt(Block b1, Block b2) {
		if (mode == COMPARE_DEFAULT)
			return b1 > b2;
		if (mode == COMPARE_ID)
			return b1.getId() > b2.getId();
		if (mode == COMPARE_NONCE)
			return b1.getNonce() > b2.getNonce();
		return false;
	};

	bool lt(Block b1, Block b2) {
		if (mode == COMPARE_DEFAULT)
			return b1 < b2;
		if (mode == COMPARE_ID)
			return b1.getId() < b2.getId();
		if (mode == COMPARE_NONCE)
			return b1.getNonce() < b2.getNonce();
		return false;
	};

};
#endif
