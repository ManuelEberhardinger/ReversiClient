#ifndef MAP_H
#define MAP_H
#include <string>
#include <vector>
#include <iostream>
#include "GamePiece.h"
#include "AIMove.h"
#include <memory>
#include <unordered_set>
#include <unordered_map>

class Map {
	friend std::ostream&  operator<<(std::ostream&, const Map&);
public:
	Map(const std::string&);
	~Map();
	bool CheckMove(int x, int y, GamePiece &stone, int bonus);

	int GetStones() const;
	int GetBombs() const;
	int GetStrength() const;
	int GetPlayers() const;
	int GetHeight() const;
	int GetLength() const;
	int GetEmptyFields() const;
	std::shared_ptr<char> GetField() const;
	void SetField(const std::shared_ptr<char> field);
	void SetEmptyFieldsLower(int x, int y);

	int evaluate(const GamePiece &player);
	int evaluate(const GamePiece &player, int moveCount);

	//Schreibt in moves alle möglichen Züge ändert firstX und firstY
	void getMoveList(const GamePiece &player, std::vector<AIMove> &moves);
    void getReducedMoveList(const GamePiece &player, std::vector<AIMove> &moves);
	AIMove GetFirstPossibleMove(const GamePiece &player);

	void Set(int x, int y, int bonus, GamePiece &player);
	int CheckForWin(int playerNr) const;
	int CountStones(int playerNr) const;

	bool moveDirectionsShort(int x, int y, int playerNr, int dir, bool flippendStone = false, bool transitioned = false) const;

	int Bomb(int x, int y);
	void FindFirstBombMove(AIMove &move);
	void FindSimpleBombMove(AIMove &move, int myPlayerNr, bool &enoughTime);

private:
	char* field;
	int players, stones, bombs, strength, height, length;
	std::vector<std::vector<unsigned int> > transitions;
	bool* isTransitionList;
	std::vector <std::vector<std::vector<std::vector<int> > > >  TransitionDirections; 
	double* fieldValues; //Wert für Bewertung der versch. Felder

	bool _checkMove;
	int  _firstX, _firstY, _emptyFields;

	void InversionEvent();
	
	void findFieldValue();

	void FlipStones(int x, int y, int bonus, GamePiece &player, int flippedStones[8]);
	bool MoveDirection(int x, int y, int playerNr, int dir, int &flippedStones, bool transitioned);
	bool CheckMove(int x, int y, int playerNr, int flippedStones[8]);
	void MoveDirectionAndFlip(int x, int y, int playerNr, int dir, int StonesToFlip, bool transitioned);

	//Schreibt in coordinates alle Felder die von der Bombe erwischt wurden
	void Bombing(int x, int y, int strengthLeft, std::unordered_map<std::pair<int, int>, int> &coordinates);
};

#endif

