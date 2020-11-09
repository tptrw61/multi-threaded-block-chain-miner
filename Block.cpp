#include "Block.hpp"

std::hash<std::string> Block::hasher;

Block::Block() {
	this->id = 1;
	this->difficulty = DEFAULT_DIFFICULTY;
	this->previousHash = 0;
	this->solvedHash = 0;
	this->nonce = 0;
	this->timeCreated = clock();
	this->timeSolved = 0;
	this->threshold = DIFFICULTY_VALUES[DEFAULT_DIFFICULTY];
	this->nSol = 0;
};

Block::Block(unsigned id, ullint previousHash, unsigned difficulty) {
	this->id = id;
	this->difficulty = difficulty <= 16 ? difficulty : DEFAULT_DIFFICULTY;
	this->previousHash = previousHash;
	this->solvedHash = 0;
	this->nonce = 0;
	this->timeCreated = clock();
	this->timeSolved = 0;
	this->threshold = DIFFICULTY_VALUES[difficulty];
	this->nSol = 0;
}
Block::Block(unsigned id, ullint previousHash, ullint solvedHash, ullint nonce, clock_t timeCreated, clock_t timeSolved, unsigned difficulty) {
	this->id = id;
	this->previousHash = previousHash;
	this->solvedHash = solvedHash;
	this->nonce = nonce;
	this->timeCreated = timeCreated;
	this->timeSolved = timeSolved;
	this->difficulty = difficulty;
	this->threshold = DIFFICULTY_VALUES[difficulty];
	this->nSol = 1;
};

bool Block::isSolved() const {
	return timeSolved || nSol;
};

bool Block::tryNonce(ullint nonce) {
	ullint attempt = hasher(std::to_string(this->previousHash) + std::to_string(nonce));
	if (attempt > this->threshold || this->nSol)
		return 0;
	if (timeSolved)
		return 0;
	this->solvedHash = attempt;
	this->nonce = nonce;
	this->timeSolved = clock();
	return 1;
};

bool Block::tryNonce(ullint nonce) const {
	ullint attempt = hasher(std::to_string(this->previousHash) + std::to_string(nonce));
	if (attempt > this->threshold || this->nSol)
		return 0;
	if (timeSolved)
		return 0;
	return 1;
};


bool Block::operator==(Block b) const { return this->id == b.id && this->nonce == b.nonce; };
bool Block::operator!=(Block b) const { return this->id != b.id || this->nonce != b.nonce; };
bool Block::operator>=(Block b) const { return this->id >= b.id ? (this->nonce >= b.nonce) : false; };
bool Block::operator<=(Block b) const { return this->id <= b.id ? (this->nonce <= b.nonce) : false; };
bool Block::operator>(Block b) const { return this->id > b.id ? (this->nonce > b.nonce) : false; };
bool Block::operator<(Block b) const { return this->id < b.id ? (this->nonce < b.nonce) : false; };

void Block::setNoSolution() {
	if (timeSolved)
		return;
	this->solvedHash = hasher(std::to_string(previousHash));
	this->timeSolved = clock();
	this->nSol = 1;
};

unsigned Block::getId() const { return this->id; };
unsigned Block::getDifficulty() const { return this->difficulty; };
ullint Block::getPreviousHash() const { return this->previousHash; };
ullint Block::getSolvedHash() const { return this->solvedHash; };
ullint Block::getNonce() const { return this->nonce; };
clock_t Block::getTimeCreated() const { return this->timeCreated; };
clock_t Block::getTimeSolved() const { return this->timeSolved; };
bool Block::hasNoSolution() const { return this->nSol; };

bool Block::editDifficulty(unsigned difficulty) {
	if (difficulty > 16)
		return 0;
	this->difficulty = difficulty;
	return 1;
};

std::string Block::toString(bool abridged) const {
	std::string s = "";
	if (abridged) {
		s += "id: " + std::to_string(this->id) + "; ";
		s += "nonce: " + std::to_string(this->nonce);
	} else {
		s += "id            : " + std::to_string(this->id) + "\n";
		s += "previous hash : " + std::to_string(this->previousHash) + "\n";
		s += "solved hash   : " + std::to_string(this->solvedHash) + "\n";
		s += "nonce         : " + std::to_string(this->nonce) + "\n";
		s += "time created  : " + std::to_string(this->timeCreated) + "\n";
		s += "time solved   : " + std::to_string(this->timeSolved);
	}
	return s;
};



int mineBlock(Block &block, ullint nonceStart, int nonceIncrement, ullint nonceEnd) {
	ullint i;
	for (i = nonceStart; i <= nonceEnd; i++) {
		if (block.isSolved())
			return 2; //already done
		else if (block.tryNonce(i))
			return 1; //mined
		if (i == nonceEnd)
			return 0; //no solution
	}
	return 0;
};
