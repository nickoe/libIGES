/*
 * file: entity508.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * This file is part of libIGES.
 *
 * libIGES is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libIGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libIGES.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * Description: IGES Entity 508: Loop, Section 4.149, p.590+ (618+)
 */

#ifndef ENTITY_508_H
#define ENTITY_508_H

#include <iges_entity.h>

// NOTE:
// The associated parameter data are:
// + N: Int: number of edge tuples
// + TYPE(n): BOOL: 0 = Edge list, 1 = Vertex list
// + EDGE(n): INT: DE to Vertex or Edge list
// + NDX(n):  INT: index into Vertex or Edge list
// + OF(n):   BOOL: 1 = orientation flag agrees with curve (LHS of curve vector is inside the curve)
// + K(n):    INT: number of optional Parameter Space curves
// + ISOP(n,k): BOOL: 1 = PS curve is isoparametric on the surface bounded by this loop
// + CURVP(n,k): INT: DE to PS curve entity
// Forms:
//  1: Note: '0' is also given as a valid form but with no corresponding documentation
//
// Unused DE items:
// + Structure
// + View


/**
 * Struct LOOP_DEIDX
 * stores information on Directory Entry indices to data being read from an IGES file.
 */
struct LOOP_DEIDX
{
    bool isVertex;      //< true if curve is described by a vertex list
    int  data;          //< (DE index) pointer to E502 (vertex list) or E504 (edge list)
    int  idx;           //< index into E502 or E504 entities
    bool orientFlag;    //< boundary curve orientation flag
    /// list of (DE index to) curves in parameter space and associated orientation
    std::list< std::pair<bool, int> > pcurves;

    LOOP_DEIDX()
    {
        isVertex = false;
        data = 0;
        idx = 0;
        orientFlag = true;
    }
};


/**
 * Struct LOOP_DATA
 * stores information on instantiated entities referenced by the Loop Entity
 */
struct LOOP_DATA
{
    bool isVertex;
    IGES_ENTITY* data;
    int  idx;
    bool orientFlag;
    std::list< std::pair<bool, IGES_ENTITY*> > pcurves;

    LOOP_DATA()
    {
        isVertex = false;
        orientFlag = true;
        data = NULL;
        idx = 0;
    }
};


/**
 * Class IGES_ENTITY_508
 * represents the Loop Entity
 */
class IGES_ENTITY_508 : public IGES_ENTITY
{
private:
    /// add a parent reference to a Vertex or Edge list entity and maintain a refcount
    bool addEdge( IGES_ENTITY* aEdge );

    /**
     * decrement refcount and release entity if appropriate; aFlagAll indicates
     * that all LOOP_DATA structures containing this edge and their associated
     * PCurves should be released
     */

    bool delEdge( IGES_ENTITY* aEdge, bool aFlagAll, bool aFlagUnlink );

    /// add a parent reference to a parameter space curve and ensure no duplicates
    bool addPCurve( IGES_ENTITY* aCurve );

    /**
     * delete parent reference from the given parameter space curve and delete
     * the associated edge instance if required
     */
    bool delPCurve( IGES_ENTITY* aCurve, bool aFlagDelEdge, bool aFlagUnlink );

protected:

    friend class IGES;
    virtual bool format( int &index );
    virtual bool rescale( double sf );

    std::list<LOOP_DEIDX> deItems;  // Data for EDGE, including DE indices
    std::list<LOOP_DATA> edges;
    std::list<std::pair<IGES_ENTITY*, int> > redges;   // refcounts for edges

public:
    IGES_ENTITY_508( IGES* aParent );
    virtual ~IGES_ENTITY_508();

    // Inherited virtual functions
    virtual bool Associate( std::vector<IGES_ENTITY*>* entities );
    virtual bool Unlink( IGES_ENTITY* aChild );
    virtual bool IsOrphaned( void );
    virtual bool AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate );
    virtual bool DelReference( IGES_ENTITY* aParentEntity );
    virtual bool ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar );
    virtual bool ReadPD( std::ifstream& aFile, int& aSequenceVar );
    virtual bool SetTransform( IGES_ENTITY* aTransform );
    virtual bool SetEntityForm( int aForm );
    virtual bool SetDependency( IGES_STAT_DEPENDS aDependency );
    virtual bool SetHierarchy( IGES_STAT_HIER aHierarchy );
    // parameters not supported by the specification:
    virtual bool SetView( IGES_ENTITY* aView );

    // functions unique to E508

    /**
     * Function GetLoopData
     * returns a pointer to the list of data structures
     * representing this loop entity.
     */
    const std::list<LOOP_DATA>* GetLoopData( void );


    /**
     * Function AddEdge
     * adds references to data representing an edge along
     * this loop and returns true on success.
     *
     * @param aEdge = pointer to data structure storing information
     * representing the edge to be added.
     */
    bool AddEdge( LOOP_DATA& aEdge );
};

#endif  // ENTITY_508_H
