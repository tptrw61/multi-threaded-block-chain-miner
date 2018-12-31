#pragma once

#include <ctime>
#include <functional>
#include <string>

#define DEFAULT_DIFFICULTY 3

const size_t DIFFICULTY_VALUES[] = {
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
	size_t previousHash;
	size_t solvedHash;
	size_t nonce;
	clock_t timeCreated;
	clock_t timeSolved;
	size_t threshold;
	bool nSol; //no solutions, true only if the hash is impossible at current difficulty

	static std::hash<std::string> hasher;

public:

	Block();
	Block(unsigned id, size_t previousHash, unsigned difficulty = DEFAULT_DIFFICULTY);
	Block(unsigned id, size_t previousHash, size_t solvedHash, size_t nonce, clock_t timeCreated, clock_t timeSolved, unsigned difficulty = DEFAULT_DIFFICULTY);

	bool isSolved() const;
	bool tryNonce(size_t nonce);
	bool tryNonce(size_t nonce) const;

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
	size_t getPreviousHash() const;
	size_t getSolvedHash() const;
	size_t getNonce() const;
	clock_t getTimeCreated() const;
	clock_t getTimeSolved() const;
	bool hasNoSolution() const;

	bool editDifficulty(unsigned difficulty);

	std::string toString(bool abridged = true) const;

};

//0 => no solution
//1 => found solution (mined)
//2 => solution already found (already mined)
int mineBlock(Block &block, size_t nonceStart = 0, int nonceIncrement = 1, size_t nonceEnd = -1);

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
