#ifndef GAMEPIECE_H
#define GAMEPIECE_H

//Klasse repr�sentiert alle Spielsteine. Enth�lt Spielernummer, ob es ein �berschreibstein ist und Anzahl der Bomben.
class GamePiece
{
public:
	GamePiece(int playerNr, int bombs, int strenght, int stones);

	int getPlayerNr() const;
	int getOverrideStonesNr() const;
	int getBombsNr() const;
	int getBombsStrength() const;
	bool getIsDisqualified() const;

	void setPlayerNr(const int playerNr);
	void setIsDisqualified(const bool isDisqualified);//falls Spieler disqualifiziert wird, somit kann �berpr�ft werden ob Berechnung f�r Spieler nicht mehr n�tig ist

	void setBombsNrHigher(); //erh�ht Bomben um 1 beim bonus feld
	void setOverrideStonesNrHigher();// erh�ht �berschreibsteine um 1 beim bonus feld

	bool setOverrideStonesLower(); //verringert und setzt ein �berschreibstein ein, falls m�glich
	bool setBombsLower();

	void print() const;
private:
	int _playerNr;
	int _overrideStonesNr;
	int _bombsNr;
	int _strenghtBombs;
	bool _isDisqualified;
};

#endif