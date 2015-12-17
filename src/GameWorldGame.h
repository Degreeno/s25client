// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#ifndef GameWorldGame_h__
#define GameWorldGame_h__

#include "GameWorldBase.h"
#include "gameTypes/MapTypes.h"
#include "TradeRoute.h"
#include <vector>

class TradeGraph;
class nobBaseWarehouse;

/// "Interface-Klasse" f�r das Spiel
class GameWorldGame : public virtual GameWorldBase
{
protected:
    /// trade graphs, one for each player
    std::vector<TradeGraph*> tgs;
private:
    /// vergleicht 2 Punkte, ob sie von unterschiedlichen Spielern sind und setzt
    /// Punkt ggf. zu gar keinem Spieler, 2. Funktion wird f�r Punkte im 2er Abstand verwendet, da es dort ein bisschen anders l�uft!
    void AdjustNodes(const MapCoord x1, const MapCoord y1, const MapCoord x2, const MapCoord y2);
    void AdjustNodes2(const MapCoord x1, const MapCoord y1, const MapCoord x2, const MapCoord y2);
    /// Zerst�rt Spielerteile auf einem Punkt, wenn der Punkt dem Spieler nun nich mehr geh�rt
    void DestroyPlayerRests(const MapPoint pt, const unsigned char new_player, const noBaseBuilding* exception, bool allowdestructionofmilbuildings=true);

    /// Pr�ft, ob auf diesem Punkt Deko-Objekte liegen, die f�r den Wegbau entfernt werden k�nnen
    bool IsObjectionableForRoad(const MapPoint pt);


    /// Punkt vollst�ndig sichtbar?
    bool IsPointCompletelyVisible(const MapPoint pt, const unsigned char player, const noBaseBuilding* const exception) const;
    /// Pr�ft, ob auf angegebenen Punkt sich ein Sp�her von player befindet
    bool IsScoutingFigureOnNode(const MapPoint pt, const unsigned player, const unsigned distance) const;
    /// Berechnet die Sichtbarkeit eines Punktes neu f�r den angegebenen Spieler
    /// exception ist ein Geb�ude (Sp�hturm, Milit�rgeb�ude), was nicht mit in die Berechnugn einbezogen
    /// werden soll, z.b. weil es abgerissen wird
    void RecalcVisibility(const MapPoint pt, const unsigned char player, const noBaseBuilding* const exception);
    /// Setzt Punkt auf jeden Fall auf sichtbar
    void SetVisibility(const MapPoint pt,  const unsigned char player);

    /// Pr�fen, ob zu einem bestimmten K�senpunkt ein Hafenpunkt geh�rt und wenn ja, wird dieser zur�ckgegeben
    unsigned short GetHarborPosID(const MapPoint pt);
    /// Bestimmt die Schifffahrtrichtung, in der ein Punkt relativ zu einem anderen liegt
    unsigned char GetShipDir(Point<int> pos1, Point<int> pos2);


protected:

    /// Erzeugt FOW-Objekte, -Stra�en und -Grensteine von aktuellen Punkt f�r einen bestimmten Spieler
    void SaveFOWNode(const MapPoint pt, const unsigned player);
    /// Berechnet f�r alle Hafenpunkt jeweils die Richtung und Entfernung zu allen anderen Hafenpunkten
    /// �ber die Kartenr�nder hinweg
    void CalcHarborPosNeighbors();
    /// Create Trade graphs
    void CreateTradeGraphs();

public:

    virtual ~GameWorldGame();

    /// Set by the playerSwitch GC
    struct{
        unsigned char oldPlayer, newPlayer;
    } switchedPlayers;

    /// Stellt anderen Spielern/Spielobjekten das Game-GUI-Interface zur Verf�ung
    inline GameInterface* GetGameInterface() const { return gi; }

    inline void SetNO(noBase* obj, const MapPoint pt) { GetNode(pt).obj = obj; }
    void AddFigure(noBase* fig, const MapPoint pt);
    void RemoveFigure(noBase* fig, const MapPoint pt);

    /// Berechnet Bauqualit�ten an Punkt x;y und den ersten Kreis darum neu
    void RecalcBQAroundPoint(const MapPoint pt);
    /// Berechnet Bauqualit�ten wie bei letzterer Funktion, blo� noch den 2. Kreis um x;y herum
    void RecalcBQAroundPointBig(const MapPoint pt);

    /// Pr�ft, ob dieser Punkt von Menschen betreten werden kann
    bool IsNodeForFigures(const MapPoint pt) const;
    /// Kann dieser Punkt von auf Stra�en laufenden Menschen betreten werden? (K�mpfe!)
    bool IsRoadNodeForFigures(const MapPoint pt, const unsigned char dir);
    /// L�sst alle Figuren, die auf diesen Punkt  auf Wegen zulaufen, anhalten auf dem Weg (wegen einem Kampf)
    void StopOnRoads(const MapPoint pt, const unsigned char dir = 0xff);

    /// Sagt Bescheid, dass der Punkt wieder freigeworden ist und l�sst ggf. Figuren drumherum wieder weiterlaufen
    void RoadNodeAvailable(const MapPoint pt);

    /// Flagge an x,y setzen, dis_dir ist der aus welche Richtung der Weg kommt, wenn man einen Weg mit Flagge baut
    /// kann ansonsten auf 255 gesetzt werden
    void SetFlag(const MapPoint pt, const unsigned char player, const unsigned char dis_dir = 255);
    /// Flagge soll zerstr�rt werden
    void DestroyFlag(const MapPoint pt);
    /// Baustelle setzen
    void SetBuildingSite(const BuildingType type, const MapPoint pt, const unsigned char player);
    /// Geb�ude bzw Baustelle abrei�en
    void DestroyBuilding(const MapPoint pt, const unsigned char playe);

    /// Wegfindung f�r Menschen im Stra�ennetz
    unsigned char FindHumanPathOnRoads(const noRoadNode& start, const noRoadNode& goal, unsigned* length = NULL, MapPoint * next_harbor = NULL, const RoadSegment* const forbidden = NULL);
    /// Wegfindung f�r Waren im Stra�ennetz
    unsigned char FindPathForWareOnRoads(const noRoadNode& start, const noRoadNode& goal, unsigned* length = NULL, MapPoint * next_harbor = NULL, unsigned max = 0xFFFFFFFF);
    /// Pr�ft, ob eine Schiffsroute noch G�ltigkeit hat
    bool CheckShipRoute(const MapPoint start, const std::vector<unsigned char>& route, const unsigned pos,
        MapPoint* dest);
    /// Find a route for trade caravanes
    unsigned char FindTradePath(const MapPoint start,
        const MapPoint dest, const unsigned char player, const unsigned max_route = 0xffffffff, const bool random_route = false,
        std::vector<unsigned char> * route = NULL, unsigned* length = NULL,
        const bool record = false) const;
    /// Check whether trade path is still valid
    bool CheckTradeRoute(const MapPoint start, const std::vector<unsigned char>& route, const unsigned pos, const unsigned char player,
        MapPoint * dest = NULL) const;


    /// setzt den Stra�en-Wert an der Stelle X,Y (berichtigt).
    void SetRoad(const MapPoint pt, unsigned char dir, unsigned char type);

    /// setzt den Stra�en-Wert um den Punkt X,Y.
    void SetPointRoad(const MapPoint pt, unsigned char dir, unsigned char type);

    /// Funktionen aus ehemaligen Game
    /// Baut eine Stra�e ( nicht nur visuell, sondern auch wirklich )
    void BuildRoad(const unsigned char playerid, const bool boat_road,
        const MapPoint start, const std::vector<unsigned char>& route);
    /// Rei�t eine Stra�e ab
    void DestroyRoad(const MapPoint pt, const unsigned char dir);
    /// baut eine Stra�e aus
    void UpgradeRoad(const MapPoint pt, const unsigned char dir);

    /// Berechnet das Land in einem bestimmten Bereich (um ein neues, abgerissenes oder eingenommenes
    /// Milit�rgeb�ude rum) neu, destroyed gibt an, ob building abgerissen wurde und somit nicht einberechnet werden soll
    void RecalcTerritory(const noBaseBuilding* const building, const unsigned short radius, const bool destroyed, const bool newBuilt);
    /// Berechnet das Land in einem bestimmten Bereich um ein aktuelles Milit�rgeb�ude rum neu und gibt zur�ck ob sich etwas ver�ndern w�rde (auf f�r ki wichtigem untergrund) wenn das Geb�ude zerst�rt werden w�rde
    bool TerritoryChange(const noBaseBuilding* const building, const unsigned short radius, const bool destroyed, const bool newBuilt);
    /// Greift ein Milit�rgeb�ude auf x,y an (entsendet daf�r die Soldaten etc.)
    void Attack(const unsigned char player_attacker, const MapPoint pt, const unsigned short soldiers_count, const bool strong_soldiers);
    /// Greift ein Milit�regeb�ude mit Schiffen an
    void AttackViaSea(const unsigned char player_attacker, const MapPoint pt, const unsigned short soldiers_count, const bool strong_soldiers);
    // Liefert das entsprechende Milit�rquadrat f�r einen bestimmten Punkt auf der Karte zur�ck (normale Koordinaten)
    std::list<nobBaseMilitary*>& GetMilitarySquare(const MapPoint pt);

    /// F�gt einen Katapultstein der Welt hinzu, der gezeichnt werden will
    inline void AddCatapultStone(CatapultStone* cs) {catapult_stones.push_back(cs); }
    inline void RemoveCatapultStone(CatapultStone* cs) {catapult_stones.remove(cs); }

    /// L�sst alles spielerische abbrennen, indem es alle Flaggen der Spieler zerst�rt
    void Armageddon();

    /// L�sst alles spielerische eines Spielers abbrennen, indem es alle Flaggen eines Spieler zerst�rt
    void Armageddon(const unsigned char player);

    /// Sagt der GW Bescheid, dass ein Objekt von Bedeutung an x,y vernichtet wurde, damit dieser
    /// dass ggf. an den WindowManager weiterleiten kann, damit auch ein Fenster wieder geschlossen wird
    virtual void ImportantObjectDestroyed(const MapPoint pt) = 0;
    /// Sagt, dass ein Milit�rgeb�ude eingenommen wurde und ggf. ein entsprechender "Fanfarensound" abgespielt werden sollte
    virtual void MilitaryBuildingCaptured(const MapPoint pt, const unsigned char player) = 0;

    /// Ist der Punkt ein geeigneter Platz zum Warten vor dem Milit�rgeb�ude
    bool ValidWaitingAroundBuildingPoint(const MapPoint pt, nofAttacker* attacker, const MapPoint center);
    /// Geeigneter Punkt f�r K�mpfe?
    bool ValidPointForFighting(const MapPoint pt, const bool avoid_military_building_flags, nofActiveSoldier* exception = NULL);

    /// Berechnet die Sichtbarkeiten neu um einen Punkt mit radius
    void RecalcVisibilitiesAroundPoint(const MapPoint pt, const MapCoord radius, const unsigned char player, const noBaseBuilding* const exception);
    /// Setzt die Sichtbarkeiten um einen Punkt auf sichtbar (aus Performancegr�nden Alternative zu oberem)
    void SetVisibilitiesAroundPoint(const MapPoint pt, const MapCoord radius, const unsigned char player);
    /// Berechet die ganzen Sichtbarkeiten der Karte neu
    void RecalcAllVisibilities();
    /// Bestimmt bei der Bewegung eines sp�henden Objekts die Sichtbarkeiten an
    /// den R�ndern neu
    void RecalcMovingVisibilities(const MapPoint pt, const unsigned char player, const MapCoord radius,
        const unsigned char moving_dir, MapPoint * enemy_territory);

    /// Stellt fest, ob auf diesem Punkt ein Grenzstein steht (ob das Grenzgebiet ist)
    bool IsBorderNode(const MapPoint pt, const unsigned char player) const;

    // Konvertiert Ressourcen zwischen Typen hin und her oder l�scht sie.
    // F�r Spiele ohne Gold.
    void ConvertMineResourceTypes(unsigned char from, unsigned char to);

    /// Gr�ndet vom Schiff aus eine neue Kolonie, gibt true zur�ck bei Erfolg
    bool FoundColony(const unsigned harbor_point, const unsigned char player, const unsigned short sea_id);
    /// Registriert eine Baustelle eines Hafens, die vom Schiff aus gesetzt worden ist
    void AddHarborBuildingSiteFromSea(noBuildingSite* building_site) { harbor_building_sites_from_sea.push_back(building_site); }
    /// Removes it. It is allowed to be called with a regular harbor building site (no-op in that case)
    void RemoveHarborBuildingSiteFromSea(noBuildingSite* building_site);
    /// Gibt zur�ck, ob eine bestimmte Baustellen eine Baustelle ist, die vom Schiff aus errichtet wurde
    bool IsHarborBuildingSiteFromSea(const noBuildingSite* building_site) const;
    /// Liefert eine Liste der Hafenpunkte, die von einem bestimmten Hafenpunkt erreichbar sind
    void GetHarborPointsWithinReach(const unsigned hp, std::vector<unsigned>& hps) const;

    /// Creates a Trade Route from one point to another
    TradeRoute CreateTradeRoute(const nobBaseWarehouse& start, const nobBaseWarehouse& dest, const unsigned char player);
    /// Retrieves a trade graph
    TradeGraph* GetTradeGraph(const unsigned char player) const { return tgs[player]; }
};

#endif // GameWorldGame_h__