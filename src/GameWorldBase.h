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

#ifndef GameWorldBase_h__
#define GameWorldBase_h__

#include "TerrainRenderer.h"
#include "buildings/nobBaseMilitary.h"
#include "gameTypes/MapNode.h"
#include "gameTypes/MapTypes.h"
#include "gameTypes/LandscapeType.h"
#include "gameTypes/GO_Type.h"
#include <vector>
#include <list>

class GameInterface;
class CatapultStone;
class noBuildingSite;
class GameClientPlayerList;
class GameClientPlayer;
class nobBaseMilitary;
class noNothing;
class fowNothing;
class nofPassiveSoldier;
class GameWorldBase;
struct lua_State;

typedef bool (*FP_Node_OK_Callback)(const GameWorldBase& gwb, const MapPoint pt, const unsigned char dir, const void* param);

/// Grundlegende Klasse, die die Gamewelt darstellt, enth�lt nur deren Daten
class GameWorldBase
{
protected:

    /// Interface zum GUI
    GameInterface* gi;

    /// Breite und H�he der Karte in Kontenpunkten
    unsigned short width_, height_;
    /// Landschafts-Typ
    LandscapeType lt;

    std::vector<unsigned short> handled_nodes;  /// Array von abgeklapperten Knoten und deren Punktzahlen

    /// Eigenschaften von einem Punkt auf der Map
    std::vector<MapNode> nodes;

    /// Rendert das Terrain
    TerrainRenderer tr;

    /// Informationen �ber die Weltmeere
    struct Sea
    {
        /// Anzahl der Knoten, welches sich in diesem Meer befinden
        unsigned nodes_count;

        Sea() : nodes_count(0) {}
        Sea(const unsigned nodes_count) : nodes_count(nodes_count) {}
    };
    std::vector<Sea> seas;

    /// Alle Hafenpositionen
    struct HarborPos
    {
        MapPoint pos;

        struct CoastalPoint
        {
            unsigned short sea_id;
        } cps[6];

        struct Neighbor
        {
            unsigned id;
            unsigned distance;

            Neighbor() {}
            Neighbor(const unsigned id, const unsigned distance)
                : id(id), distance(distance) {}

            bool operator<(const Neighbor& two) const
            { return (distance < two.distance) || (distance == two.distance && id < two.id); }
        };

        std::vector<Neighbor> neighbors[6];

        HarborPos(){}
        HarborPos(const MapPoint pt): pos(pt){}
    };

    std::vector< HarborPos > harbor_pos;

    /// Baustellen von H�fen, die vom Schiff aus errichtet wurden
    std::list<noBuildingSite*> harbor_building_sites_from_sea;

    GameClientPlayerList* players;
    noNothing* noNodeObj;
    fowNothing* noFowObj;

public:
    unsigned int map_size;

    std::list<CatapultStone*> catapult_stones; /// Liste von allen umherfliegenden Katapultsteinen.

    /// Liste von Milit�rgeb�uden (auch HQ und Haufengeb�ude, daher normale Geb�ude) pro "Milit�rquadrat"
    std::vector< std::list<nobBaseMilitary*> > military_squares;

public:

    GameWorldBase();
    virtual ~GameWorldBase();

    // Grundlegende Initialisierungen
    void Init();
    /// Aufr�umen
    void Unload();

    /// Setzt GameInterface
    inline void SetGameInterface(GameInterface* const gi) { this->gi = gi; }

    /// Gr��e der Map abfragen
    inline unsigned short GetWidth() const { return width_; }
    inline unsigned short GetHeight() const { return height_; }

    /// Landschaftstyp abfragen
    LandscapeType GetLandscapeType() const { return lt; }

    /// Gibt Punkt um diesen Punkt (X-Koordinate) direkt zur�ck in einer Richtung von 0-5
    MapCoord GetXA(const MapCoord x, const MapCoord y, unsigned dir) const;
    MapCoord GetXA(const MapPoint pt, unsigned dir) const { return GetNeighbour(pt, dir).x; }
    /// Gibt Punkt um diesen Punkt (Y-Koordinate)  direkt zur�ck in einer Richtung von 0-5
    MapCoord GetYA(const MapCoord x, const MapCoord y, unsigned dir) const;
    /// Returns neighbouring point in one direction (0-5)
    MapPoint GetNeighbour(const MapPoint, unsigned dir) const;
    /// Returns neighbouring point (2nd layer: dir 0-11)
    MapPoint GetNeighbour2(const MapPoint, unsigned dir) const;
    /// Berechnet die Differenz zweier Koordinaten von x1 zu x2, wenn man ber�cksichtigt, dass man �ber den
    /// Rand weitergehen kann
    MapCoord CalcDistanceAroundBorderX(const MapCoord x1, const MapCoord x2) const;
    MapCoord CalcDistanceAroundBorderY(const MapCoord y1, const MapCoord y2) const;

    /// Ermittelt Abstand zwischen 2 Punkten auf der Map unter Ber�cksichtigung der Kartengrenz�berquerung
    unsigned CalcDistance(int x1, int y1, int x2, int y2) const;
    inline unsigned CalcDistance(const MapPoint p1, const MapPoint p2) const { return CalcDistance(p1.x, p1.y, p2.x, p2.y); }

    /// Returns a MapPoint from a point. This ensures, the coords are actually in the map [0, mapSize)
    MapPoint MakeMapPoint(Point<int> pt) const;
    // Erzeugt eindeutige ID aus gegebenen X und Y-Werten
    inline unsigned MakeCoordID(const MapPoint pt) const { return GetIdx(pt); }

    // Returns the linear index for a map point
    inline unsigned GetIdx(const MapPoint pt) const
    { return static_cast<unsigned>(pt.y) * static_cast<unsigned>(width_) + static_cast<unsigned>(pt.x); }

    /// Gibt Map-Knotenpunkt zur�ck
    inline const MapNode& GetNode(const MapPoint pt) const { assert(pt.x < width_ && pt.y < height_);  return nodes[GetIdx(pt)]; }
    MapNode& GetNode(const MapPoint pt) { assert(pt.x < width_ && pt.y < height_); return nodes[GetIdx(pt)]; }
    /// Gibt MapKnotenpunkt darum zur�ck
    inline const MapNode& GetNodeAround(const MapPoint pt, const unsigned i) const
    { return GetNode(GetNeighbour(pt, i));  }
    inline MapNode& GetNodeAround(const MapPoint pt, const unsigned i)
    { return GetNode(GetNeighbour(pt, i));  }

    // Gibt ein NO zur�ck, falls keins existiert, wird ein "Nothing-Objekt" zur�ckgegeben
    noBase* GetNO(const MapPoint pt);
    // Gibt ein NO zur�ck, falls keins existiert, wird ein "Nothing-Objekt" zur�ckgegeben
    const noBase* GetNO(const MapPoint pt) const;
    /// Gibt ein FOW-Objekt zur�ck, falls keins existiert, wird ein "Nothing-Objekt" zur�ckgegeben
    const FOWObject* GetFOWObject(const MapPoint pt, const unsigned spectator_player) const;
    /// Gibt den GOT des an diesem Punkt befindlichen Objekts zur�ck bzw. GOT_NOTHING, wenn keins existiert
    GO_Type GetGOT(const MapPoint pt) const;

    /// Gibt Figuren, die sich auf einem bestimmten Punkt befinden, zur�ck
    /// nicht bei laufenden Figuren oder
    const std::list<noBase*>& GetFigures(const MapPoint pt) const { return GetNode(pt).figures; }
    /// Gibt Dynamische Objekte, die von einem bestimmten Punkt aus laufen oder dort stehen sowie andere Objekte,
    /// die sich dort befinden, zur�ck
    std::vector<noBase*> GetDynamicObjectsFrom(const MapPoint pt) const;

    // Gibt ein spezifisches Objekt zur�ck
    template<typename T> inline T* GetSpecObj(const MapPoint pt) { return dynamic_cast<T*>( GetNode(pt).obj ); }
    // Gibt ein spezifisches Objekt zur�ck
    template<typename T> inline const T* GetSpecObj(const MapPoint pt) const { return dynamic_cast<const T*>( GetNode(pt).obj ); }

    /// Gibt ein Terrain-Dreieck um einen Punkt herum zur�ck.
    TerrainType GetTerrainAround(const MapPoint pt, unsigned char dir) const;
    /// Gibt das Terrain zur�ck, �ber das ein Mensch/Tier laufen m�sste, von X,Y in Richtung DIR (Vorw�rts).
    TerrainType GetWalkingTerrain1(const MapPoint pt, unsigned char dir) const;
    /// Gibt das Terrain zur�ck, �ber das ein Mensch/Tier laufen m�sste, von X,Y in Richtung DIR (R�ckw�rts).
    TerrainType GetWalkingTerrain2(const MapPoint pt, unsigned char dir) const;
    /// Gibt zur�ck, ob ein Punkt vollst�ndig von Wasser umgeben ist
    bool IsSeaPoint(const MapPoint pt) const;

    /// liefert den Stra�en-Wert an der Stelle X,Y
    unsigned char GetRoad(const MapPoint pt, unsigned char dir, bool all = false) const;
    /// liefert den Stra�en-Wert um den Punkt X,Y.
    unsigned char GetPointRoad(const MapPoint pt, unsigned char dir, bool all = false) const;
    /// liefert FOW-Stra�en-Wert um den punkt X,Y
    unsigned char GetPointFOWRoad(MapPoint pt, unsigned char dir, const unsigned char viewing_player) const;

    /// Kann dorthin eine Stra�e gebaut werden?
    bool RoadAvailable(const bool boat_road, const MapPoint pt, unsigned char to_dir, const bool visual = true) const;
    /// Pr�ft ob exakt die gleiche Stra�e schon gebaut wurde
    bool RoadAlreadyBuilt(const bool boat_road, const MapPoint start, const std::vector<unsigned char>& route);
    /// Bauqualit�ten berechnen, bei flagonly gibt er nur 1 zur�ck, wenn eine Flagge m�glich ist
    BuildingQuality CalcBQ(const MapPoint pt, const unsigned char player, const bool flagonly = false, const bool visual = true, const bool ignore_player = false) const;
    /// Setzt die errechnete BQ gleich mit
    inline void SetBQ(const MapPoint pt, const unsigned char player, const bool flagonly = false, const bool visual = true)
    { GetNode(pt).bq = CalcBQ(pt, player, flagonly, visual); }

    /// Pr�ft, ob der Pkut zu dem Spieler geh�rt (wenn er der Besitzer ist und es false zur�ckliefert, ist es Grenzgebiet)
    bool IsPlayerTerritory(const MapPoint pt) const;
    /// Berechnet BQ bei einer gebauten Stra�e
    void CalcRoad(const MapPoint pt, const unsigned char player);
    /// Ist eine Flagge irgendwo um x,y ?
    bool FlagNear(const MapPoint pt) const;
    /// Pr�ft, ob sich in unmittelbarer N�he (im Radius von 4) Milit�rgeb�ude befinden
    bool IsMilitaryBuildingNearNode(const MapPoint nPt, const unsigned char player) const;

    /// setzt den virtuellen Stra�en-Wert an der Stelle X,Y (berichtigt).
    void SetVirtualRoad(const MapPoint pt, unsigned char dir, unsigned char type);
    /// setzt den virtuellen Stra�en-Wert um den Punkt X,Y.
    void SetPointVirtualRoad(const MapPoint pt, unsigned char dir, unsigned char type);

    /// Test, ob auf dem besagten Punkt ein Milit�rgeb�ude steht
    bool IsMilitaryBuilding(const MapPoint pt) const;

    /// Erstellt eine Liste mit allen Milit�rgeb�uden in der Umgebung, radius bestimmt wie viele K�stchen nach einer Richtung im Umkreis
    sortedMilitaryBlds LookForMilitaryBuildings(const MapPoint pt, unsigned short radius) const;

    /// Pr�ft, ob von einem bestimmten Punkt aus der Untergrund f�r Figuren zug�nglich ist (kein Wasser,Lava,Sumpf)
    bool IsNodeToNodeForFigure(const MapPoint pt, const unsigned dir) const;

    /// Informationen, ob Grenzen �berquert wurden
    struct CrossBorders
    {
        CrossBorders(void) : left(false), top(false), right(false), bottom(false) { }
        bool left, top, right, bottom;
    };


    /// Wegfindung in freiem Terrain - Basisroutine
    bool FindFreePath(const MapPoint start,
        const MapPoint dest, const bool random_route,
        const unsigned max_route, std::vector<unsigned char> * route, unsigned* length, unsigned char* first_dir,
        FP_Node_OK_Callback IsNodeOK, FP_Node_OK_Callback IsNodeToDestOk, const void* param, const bool record) const;
    bool FindFreePathAlternatingConditions(const MapPoint start,
        const MapPoint dest, const bool random_route,
        const unsigned max_route, std::vector<unsigned char> * route, unsigned* length, unsigned char* first_dir,
        FP_Node_OK_Callback IsNodeOK, FP_Node_OK_Callback IsNodeOKAlternate, FP_Node_OK_Callback IsNodeToDestOk, const void* param, const bool record) const;
    /// Ermittelt, ob eine freie Route noch passierbar ist und gibt den Endpunkt der Route zur�ck
    bool CheckFreeRoute(const MapPoint start, const std::vector<unsigned char>& route,
        const unsigned pos, FP_Node_OK_Callback IsNodeOK, FP_Node_OK_Callback IsNodeToDestOk,
        MapPoint* dest, const void* const param = NULL) const;
    /// Wegfindung auf Stra�en - Basisroutine
    bool FindPathOnRoads(const noRoadNode* const start, const noRoadNode* const goal,
        const bool ware_mode, unsigned* length, unsigned char* first_dir, MapPoint * next_harbor,
        const RoadSegment* const forbidden, const bool record = true, unsigned max = 0xFFFFFFFF) const;
    /// Findet einen Weg f�r Figuren
    unsigned char FindHumanPath(const MapPoint start,
        const MapPoint dest, const unsigned max_route = 0xFFFFFFFF, const bool random_route = false, unsigned* length = NULL, const bool record = true) const;
    /// Wegfindung f�r Schiffe auf dem Wasser
    bool FindShipPath(const MapPoint start, const MapPoint dest, std::vector<unsigned char> * route, unsigned* length, const unsigned max_length = 200,
        CrossBorders* cb = NULL);


    /// Baut eine (bisher noch visuell gebaute) Stra�e wieder zur�ck
    void RemoveVisualRoad(const MapPoint start, const std::vector<unsigned char>& route);

    /// x,y ist ein Punkt auf irgendeinem Wegstck, gibt die Flagge zur�ck
    noFlag* GetRoadFlag(MapPoint pt, unsigned char& dir, unsigned last_i = 255);

    /// Konvertiert die Koordinaten.
    MapPoint ConvertCoords(Point<int> pt) const;
    MapPoint ConvertCoords(int x, int y) const { return ConvertCoords(Point<int>(x, y)); }

    /// Erzeugt eine GUI-ID f�r die Fenster von Map-Objekten
    inline unsigned CreateGUIID(const MapPoint pt) const { return 1000 + width_ * pt.y + pt.x; }
    /// Gibt Terrainkoordinaten zur�ck
    inline Point<float> GetTerrain(const MapPoint pt){ return tr.GetTerrain(pt); }
    inline float GetTerrainX(const MapPoint pt){ return GetTerrain(pt).x; }
    inline float GetTerrainY(const MapPoint pt){ return GetTerrain(pt).y; }

    /// Ver�ndert die H�he eines Punktes und die damit verbundenen Schatten
    void ChangeAltitude(const MapPoint pt, const unsigned char altitude);

    /// Ermittelt Sichtbarkeit eines Punktes auch unter Einbeziehung der Verb�ndeten des jeweiligen Spielers
    Visibility CalcWithAllyVisiblity(const MapPoint pt, const unsigned char player) const;

    /// Gibt die Anzahl an Hafenpunkten zur�ck
    inline unsigned GetHarborPointCount() const
    { return harbor_pos.size() - 1; }
    /// Ist es an dieser Stelle f�r einen Spieler m�glich einen Hafen zu bauen
    bool IsHarborPointFree(const unsigned harbor_id, const unsigned char player,
        const unsigned short sea_id) const;
    /// Gibt die Koordinaten eines bestimmten Hafenpunktes zur�ck
    MapPoint GetHarborPoint(const unsigned harbor_id) const;
    /// Gibt die ID eines Hafenpunktes zur�ck
    inline unsigned GetHarborPointID(const MapPoint pt) const { return GetNode(pt).harbor_id; }
    /// Ermittelt, ob ein Punkt K�stenpunkt ist, d.h. Zugang zu einem schiffbaren Meer hat
    /// und gibt ggf. die Meeres-ID zur�ck, ansonsten 0
    unsigned short IsCoastalPoint(const MapPoint pt) const;
    /// Ermittelt, ob ein Punkt K�stenpunkt ist, d.h. Zugang zu einem schiffbaren Meer, an dem auch mindestens 1 Hafenplatz liegt, hat
    /// und gibt ggf. die Meeres-ID zur�ck, ansonsten 0
    unsigned short IsCoastalPointToSeaWithHarbor(const MapPoint pt) const;
    /// Grenzt der Hafen an ein bestimmtes Meer an?
    bool IsAtThisSea(const unsigned harbor_id, const unsigned short sea_id) const;
    /// Gibt den Punkt eines bestimmtes Meeres um den Hafen herum an, sodass Schiffe diesen anfahren k�nnen
    MapPoint GetCoastalPoint(const unsigned harbor_id, const unsigned short sea_id) const;
    /// Sucht freie Hafenpunkte, also wo noch ein Hafen gebaut werden kann
    unsigned GetNextFreeHarborPoint(const MapPoint pt, const unsigned origin_harbor_id, const unsigned char dir,
        const unsigned char player) const;
    /// Gibt die angrenzenden Sea-IDs eines Hafenpunktes zur�ck
    void GetSeaIDs(const unsigned harbor_id, unsigned short* sea_ids) const;
    /// Berechnet die Entfernung zwischen 2 Hafenpunkten
    unsigned CalcHarborDistance(const unsigned habor_id1, const unsigned harbor_id2) const;
    /// Bestimmt f�r einen beliebigen Punkt auf der Karte die Entfernung zum n�chsten Hafenpunkt
    unsigned CalcDistanceToNearestHarbor(const MapPoint pos) const;
    /// returns true when a harborpoint is in SEAATTACK_DISTANCE for figures!
    bool IsAHarborInSeaAttackDistance(const MapPoint pos) const;

    inline void SetPlayers(GameClientPlayerList* pls) { players = pls; }
    /// Liefert einen Player zur�ck
    GameClientPlayer& GetPlayer(const unsigned int id) const;

    struct PotentialSeaAttacker
    {
        /// Comparator that compares only the soldier pointer
        struct CmpSoldier
        {
            nofPassiveSoldier* const search;
            CmpSoldier(nofPassiveSoldier* const search): search(search){}
            bool operator()(const PotentialSeaAttacker& other)
            {
                return other.soldier == search;
            }
        };
        /// Soldat, der als Angreifer in Frage kommt
        nofPassiveSoldier* soldier;
        /// Hafen, den der Soldat zuerst ansteuern soll
        nobHarborBuilding* harbor;
        /// Entfernung Hafen-Hafen (entscheidende)
        unsigned distance;

        /// Komperator zum Sortieren
        bool operator<(const PotentialSeaAttacker& pa) const;
    };


    /// Liefert Hafenpunkte im Umkreis von einem bestimmten Milit�rgeb�ude
    std::vector<unsigned> GetHarborPointsAroundMilitaryBuilding(const MapPoint pt) const;
    /// returns all sea_ids from which a given building can be attacked by sea
    std::vector<unsigned> GetValidSeaIDsAroundMilitaryBuildingForAttack(const MapPoint pt, std::vector<bool>& use_seas, const unsigned char player_attacker)const;
    /// returns all sea_ids found in the given vector from which a given building can be attacked by sea
    void GetValidSeaIDsAroundMilitaryBuildingForAttackCompare(const MapPoint pt, std::vector<unsigned short>& use_seas, const unsigned char player_attacker)const;
    /// Sucht verf�gbare Soldaten, um dieses Milit�rgeb�ude mit einem Seeangriff anzugreifen
    std::vector<PotentialSeaAttacker> GetAvailableSoldiersForSeaAttack(const unsigned char player_attacker, const MapPoint pt) const;
    /// Gibt Anzahl oder gesch�tzte St�rke(rang summe + anzahl) der verf�gbaren Soldaten die zu einem Schiffsangriff starten k�nnen von einer bestimmten sea id aus
    unsigned int GetAvailableSoldiersForSeaAttackAtSea(const unsigned char player_attacker, unsigned short seaid, bool count = true) const;

protected:

    /// Berechnet die Schattierung eines Punktes neu
    void RecalcShadow(const MapPoint pt);

    /// F�r abgeleitete Klasse, die dann das Terrain entsprechend neu generieren kann
    virtual void AltitudeChanged(const MapPoint pt) = 0;
    /// F�r abgeleitete Klasse, die dann das Terrain entsprechend neu generieren kann
    virtual void VisibilityChanged(const MapPoint pt) = 0;

    /// Gibt n�chsten Hafenpunkt in einer bestimmten Richtung zur�ck, bzw. 0, wenn es keinen gibt
    unsigned GetNextHarborPoint(const MapPoint pt, const unsigned origin_harbor_id, const unsigned char dir,
        const unsigned char player, bool (GameWorldBase::*IsPointOK)(const unsigned, const unsigned char, const unsigned short) const) const;

    lua_State* lua;

    static int LUA_DisableBuilding(lua_State* L);
    static int LUA_EnableBuilding(lua_State* L);
    static int LUA_SetRestrictedArea(lua_State* L);
    static int LUA_ClearResources(lua_State *L);
    static int LUA_AddWares(lua_State* L);
    static int LUA_AddPeople(lua_State* L);
    static int LUA_GetGF(lua_State *L);
    static int LUA_Log(lua_State *L);
    static int LUA_Chat(lua_State *L);
    static int LUA_MissionStatement(lua_State *L);
    static int LUA_PostMessage(lua_State *L);
    static int LUA_PostMessageWithLocation(lua_State *L);
    static int LUA_GetPlayerCount(lua_State *L);
    static int LUA_GetBuildingCount(lua_State *L);
    static int LUA_GetWareCount(lua_State *L);
    static int LUA_GetPeopleCount(lua_State *L);
    static int LUA_AddEnvObject(lua_State *L);
    static int LUA_AIConstructionOrder(lua_State *L);
    static int LUA_AddStaticObject(lua_State *L);
    static int LUA_PostNewBuildings(lua_State *L);

public:
    void LUA_EventExplored(unsigned player, const MapPoint pt);
    void LUA_EventOccupied(unsigned player, const MapPoint pt);
    void LUA_EventStart();
    void LUA_EventGF(unsigned number);
    void LUA_EventResourceFound(unsigned char player, const MapPoint pt, const unsigned char type, const unsigned char quantity);
};

#endif // GameWorldBase_h__