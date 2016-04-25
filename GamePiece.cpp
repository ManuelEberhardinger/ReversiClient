#include "GamePiece.h"
#include <iostream>

GamePiece::GamePiece(int playerNr, int bombs, int strength, int stones)
	: _playerNr(playerNr), _bombsNr(bombs), _strenghtBombs(strength), _overrideStonesNr(stones), _isDisqualified(false)
{}

int GamePiece::getPlayerNr() const
{
	return _playerNr;
}

void GamePiece::setPlayerNr(const int playerNr)
{
	_playerNr = playerNr;
}

int GamePiece::getBombsNr() const
{
	return _bombsNr;
}

int GamePiece::getBombsStrength() const
{
	return _strenghtBombs;
}

int GamePiece::getOverrideStonesNr() const
{
	return _overrideStonesNr;
}

void GamePiece::setBombsNrHigher()
{
	_bombsNr++;
}

void GamePiece::setOverrideStonesNrHigher()
{
	_overrideStonesNr++;
}

bool GamePiece::setOverrideStonesLower()
{
	if (_overrideStonesNr <= 0)
		return false;
	else
	{
		_overrideStonesNr--;
		return true;
	}
}

bool GamePiece::setBombsLower()
{
	if (_bombsNr <= 0)
		return false;
	else
	{
		_bombsNr--;
		return true;
	}
}

bool GamePiece::getIsDisqualified() const
{
	return _isDisqualified;
}

void GamePiece::setIsDisqualified(const bool isDisqualified)
{
	_isDisqualified = isDisqualified;
}

void GamePiece::print() const {
	std::cout << "Anzahl Überschreibsteine: " << _overrideStonesNr << std::endl;
	std::cout << "Anzahl Bomben: " << _bombsNr << std::endl;
}