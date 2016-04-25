#ifndef GAMEPIECE_H
#define GAMEPIECE_H

//Klasse repräsentiert alle Spielsteine. Enthält Spielernummer, ob es ein Überschreibstein ist und Anzahl der Bomben.
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
	void setIsDisqualified(const bool isDisqualified);//falls Spieler disqualifiziert wird, somit kann überprüft werden ob Berechnung für Spieler nicht mehr nötig ist

	void setBombsNrHigher(); //erhöht Bomben um 1 beim bonus feld
	void setOverrideStonesNrHigher();// erhöht Überschreibsteine um 1 beim bonus feld

	bool setOverrideStonesLower(); //verringert und setzt ein Überschreibstein ein, falls möglich
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