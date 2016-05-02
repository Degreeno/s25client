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

#ifndef GameWorldViewer_h__
#define GameWorldViewer_h__

#include "world/GameWorldBase.h"
#include "TerrainRenderer.h"
#include "gameTypes/MapTypes.h"

class MouseCoords;
class noShip;
class FOWObject;
struct RoadBuildState;

/// "Interface-Klasse" für GameWorldBase, die die Daten grafisch anzeigt
class GameWorldViewer: public virtual GameWorldBase
{
    TerrainRenderer tr;
public:

    GameWorldViewer(GameClientPlayerList& players, const GlobalGameSettings& gameSettings, EventManager& em);

    TerrainRenderer& GetTerrainRenderer() { return tr; }

    /// Get number of soldiers that can attack that point
    unsigned GetAvailableSoldiersForAttack(const unsigned char player_attacker, const MapPoint pt);

    /// Ermittelt Sichtbarkeit eines Punktes für den lokalen Spieler, berücksichtigt ggf. Teamkameraden
    Visibility GetVisibility(const MapPoint pt) const;

    /// Höhe wurde verändert: TerrainRenderer Bescheid sagen, damit es entsprechend verändert werden kann
    void AltitudeChanged(const MapPoint pt) override;
    /// Sichtbarkeit wurde verändert: TerrainRenderer Bescheid sagen, damit es entsprechend verändert werden kann
    void VisibilityChanged(const MapPoint pt) override;

    /// liefert sichtbare Strasse, im Nebel entsprechend die FoW-Strasse
    unsigned char GetVisibleRoad(const MapPoint pt, unsigned char dir, const Visibility visibility) const;

    /// Get the "youngest" FOWObject of all players who share the view with the local player
    const FOWObject* GetYoungestFOWObject(const MapPoint pos) const;

    /// Gets the youngest fow node of all visible objects of all players who are connected
    /// with the local player via team view
    unsigned char GetYoungestFOWNodePlayer(const MapPoint pos) const;

    /// Schattierungen (vor allem FoW) neu berechnen
    void RecalcAllColors();

    /// Get first found ship of this player at that point or NULL of none
    noShip* GetShip(const MapPoint pt, const unsigned char player) const;

    /// Get number of soldiers for attacking a point via sea
    unsigned GetAvailableSoldiersForSeaAttackCount(const unsigned char player_attacker, const MapPoint pt) const;

};

#endif // GameWorldViewer_h__
