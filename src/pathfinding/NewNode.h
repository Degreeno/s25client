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

#ifndef NewNode_h__
#define NewNode_h__

#include "pathfinding/PathfindingPoint.h"
#include "pathfinding/OpenListBinaryHeap.h"
#include <set>

/// Konstante f�r einen ung�ltigen Vorg�ngerknoten
const unsigned INVALID_PREV = 0xFFFFFFFF;

/// Class for a node used by the free pathfinding
struct NewNode
{
    NewNode() : way(0), dir(0), prev(INVALID_PREV), lastVisited(0) {}

    /// Wegkosten, die vom Startpunkt bis zu diesem Knoten bestehen
    unsigned way;
    unsigned wayEven;
    /// Die Richtung, �ber die dieser Knoten erreicht wurde
    unsigned char dir;
    unsigned char dirEven;
    /// ID (gebildet aus y*Kartenbreite+x) des Vorg�nngerknotens
    unsigned prev;
    unsigned prevEven;
    /// Iterator auf Position in der Priorit�tswarteschlange (std::set), freies Pathfinding
    std::set<PathfindingPoint>::iterator it_p;
    /// Wurde Knoten schon besucht (f�r A*-Algorithmus), wenn lastVisited == currentVisit
    unsigned lastVisited;
    unsigned lastVisitedEven; //used for road pathfinding (for ai only for now)
    MapPoint mapPt;
};

struct NewNode2
{
    /// Indicator if node was visited (lastVisited == currentVisit)
    unsigned lastVisited;
    /// Previous node
    NewNode2* prev;
    /// Distance from start to this node
    unsigned curDistance;
    /// Distance from node to target
    unsigned targetDistance;
    /// Distance from start over thise node to target (== curDistance + targetDistance)
    unsigned estimatedDistance;
    /// Index used to distinguish nodes with same estimate
    unsigned idx;
    /// Direction used to reach this node
    unsigned char dir;
    /// Point on map which this node represents
    MapPoint mapPt;
    OpenListBinaryHeapBase<NewNode2>::PosMarker posMarker;
};
#endif // NewNode_h__