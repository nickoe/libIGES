/*
 * file: entity126.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 126: NURBS Curve, Section 4.23, p.133(161+)
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

#include <sstream>
#include <sisl.h>
#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <mcad_helpers.h>
#include <entity124.h>
#include <entity126.h>
#include <entity142.h>

using namespace std;

IGES_ENTITY_126::IGES_ENTITY_126( IGES* aParent ) : IGES_CURVE( aParent )
{
    entityType = 126;
    form = 0;
    K = 0;
    M = 0;
    PROP1 = 0;
    PROP2 = 0;
    PROP3 = 0;
    PROP4 = 0;
    V0 = 0.0;
    V1 = 0.0;

    nKnots = 0;
    nCoeffs = 0;
    knots = NULL;
    coeffs = NULL;
    scurve = NULL;

    return;
}


IGES_ENTITY_126::~IGES_ENTITY_126()
{
    if( knots )
        delete [] knots;

    if( coeffs )
        delete [] coeffs;

    if( scurve )
        freeCurve( scurve );

    return;
}


bool IGES_ENTITY_126::Associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::Associate( entities ) )
    {
        ERRMSG << "\n + [INFO] failed to establish associations\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_126::format( int &index )
{
    pdout.clear();

    if( !knots || !coeffs )
    {
        ERRMSG << "\n + [INFO] no curve data\n";
        return false;
    }

    if( index < 1 || index > 9999999 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
        return false;
    }

    parameterData = index;

    if( !parent )
    {
        ERRMSG << "\n + [INFO] method invoked with no parent IGES object\n";
        return false;
    }

    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;
    // any REAL parameters are NURBS data, possibly (U,V) curve on surface; maintain high precision
    double uir = 1e-15;

    if( K < 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for K\n";
        return false;
    }

    if( M < 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for M\n";
        return false;
    }

    if( PROP1 < 0 || PROP1 > 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for PROP1\n";
        return false;
    }

    if( PROP2 < 0 || PROP2 > 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for PROP2\n";
        return false;
    }

    if( PROP3 < 0 || PROP3 > 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for PROP3\n";
        return false;
    }

    if( PROP4 < 0 || PROP4 > 1 )
    {
        ERRMSG << "\n + [INFO] invalid value for PROP4\n";
        return false;
    }

    // # of knots = 2 + K + M
    if( (2 + K + M) != nKnots )
    {
        ERRMSG << "\n + [INFO] invalid number of knots (" << nKnots;
        cerr << ") expecting " << (2 + K + M) << "\n";
        return false;
    }

    // # of coefficients = K + 1
    if( (1 + K) != nCoeffs )
    {
        ERRMSG << "\n + [INFO] invalid number of coefficients (" << nCoeffs;
        cerr << ") expecting " << (1 + K) << "\n";
        return false;
    }

    ostringstream ostr;
    ostr << entityType << pd;
    ostr << K << pd;
    ostr << M << pd;
    ostr << PROP1 << pd;
    ostr << PROP2 << pd;
    ostr << PROP3 << pd;
    ostr << PROP4 << pd;
    string lstr = ostr.str();
    string tstr;

    for( int i = 0; i < nKnots; ++i )
    {
        if( !FormatPDREal( tstr, knots[i], pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format knots\n";
            return false;
        }

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    double tD = 1.0;

    for( int i = 0, j = 3; i < nCoeffs; ++i )
    {
        if( 0 == PROP3 )
            tD = coeffs[j++];

        if( !FormatPDREal( tstr, tD, pd, 1e-6 ) )
        {
            ERRMSG << "\n + [INFO] could not format weights\n";
            return false;
        }

        j += 3;
        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    for( int i = 0, j = 0; i < nCoeffs; ++i )
    {
        if( !FormatPDREal( tstr, coeffs[j++], pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format control points\n";
            return false;
        }

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

        if( !FormatPDREal( tstr, coeffs[j++], pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format control points\n";
            return false;
        }

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

        if( !FormatPDREal( tstr, coeffs[j++], pd, uir ) )
        {
            ERRMSG << "\n + [INFO] could not format control points\n";
            return false;
        }

        if( 0 == PROP3 )
            ++j;

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );
    }

    if( !FormatPDREal( tstr, V0, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format V0\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tstr, V1, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format V1\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tstr, vnorm.x, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format normal vector\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    if( !FormatPDREal( tstr, vnorm.y, pd, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format normal vector\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    char tc = rd;

    if( !extras.empty() )
        tc = pd;

    if( !FormatPDREal( tstr, vnorm.z, tc, uir ) )
    {
        ERRMSG << "\n + [INFO] could not format normal vector\n";
        return false;
    }

    AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    if( !extras.empty() && !formatExtraParams( lstr, index, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] could not format optional parameters\n";
        pdout.clear();
        iExtras.clear();
        return false;
    }

    if( !formatComments( index ) )
    {
        ERRMSG << "\n + [INFO] could not format comments\n";
        pdout.clear();
        return false;
    }

    paramLineCount = index - parameterData;

    return true;
}


bool IGES_ENTITY_126::rescale( double sf )
{
    // Before scaling we must determine if this curve is a member of the BPTR
    // of a Curve on a Parametric Surface (BPTR to Entity 144). We must traverse
    // the ancestors of this NURBS curve and decide whether or not it
    // makes sense to scale the control points. If a Curve on Surface is scaled,
    // only the Z values should be scaled

    list<IGES_ENTITY*> eps;
    eps.push_back( this );

    IGES_ENTITY* ep = GetFirstParentRef();
    IGES_ENTITY* cps = NULL;

    while( ep )
    {
        if( ep->GetEntityType() == ENT_CURVE_ON_PARAMETRIC_SURFACE )
        {
            cps = ep;
            break;
        }

        eps.push_back( ep );
        ep = ep->GetFirstParentRef();
    }

    bool scaleXY = true;

    if( cps )
    {
        // block the operation if this entity or a parent is equal to BPTR
        if( ((IGES_ENTITY_142*)cps)->GetBPTR( &ep ) )
        {
            list<IGES_ENTITY*>::iterator sP = eps.begin();
            list<IGES_ENTITY*>::iterator eP = eps.end();

            while( sP != eP )
            {
                if( *sP == ep )
                {
                    scaleXY = false;
                    break;
                }

                ++sP;
            }
        }
    }

    if( NULL == coeffs )
        return true;

    for( int i = 0, j = 0; i < nCoeffs; ++i )
    {
        if( scaleXY )
        {
            coeffs[j] *= sf;
            ++j;
            coeffs[j] *= sf;
            ++j;
        }
        else
        {
            j += 2;
        }

        coeffs[j] *= sf;
        ++j;

        if( 0 == PROP3 )
            ++j;
    }

    return true;
}


bool IGES_ENTITY_126::Unlink( IGES_ENTITY* aChild )
{
    return IGES_ENTITY::Unlink( aChild );
}


bool IGES_ENTITY_126::IsOrphaned( void )
{
    if( refs.empty() && depends != STAT_INDEPENDENT )
        return true;

    return false;
}


bool IGES_ENTITY_126::AddReference( IGES_ENTITY* aParentEntity, bool& isDuplicate )
{
    return IGES_ENTITY::AddReference( aParentEntity, isDuplicate );
}


bool IGES_ENTITY_126::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_126::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored

    if( form != 0 && form != 1 && form != 2
        && form != 3 && form != 4 && form != 5 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number (" << form << ") in NURBS curve\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_126::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadPD( aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] could not read data for Surface of Revolution\n";
        pdout.clear();
        return false;
    }

    int idx;
    bool eor = false;
    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;

    idx = pdout.find( pd );

    if( idx < 1 || idx > 8 )
    {
        ERRMSG << "\n + [BAD FILE] strange index for first parameter delimeter (";
        cerr << idx << ")\n";
        pdout.clear();
        return false;
    }

    ++idx;

    if( !ParseInt( pdout, idx, K, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read K (upper index sum)\n";
        pdout.clear();
        return false;
    }

    if( K < 1 )
    {
        ERRMSG << "\n + [INFO] invalid K value (" << K << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, M, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read M (degree of basis functions)\n";
        pdout.clear();
        return false;
    }

    if( M < 1 )
    {
        ERRMSG << "\n + [INFO] invalid M value (" << M << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, PROP1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP1 (0/1:planar/nonplanar)\n";
        pdout.clear();
        return false;
    }

    if( PROP1 != 0 && PROP1 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP1 value (" << PROP1 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, PROP2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP2 (0/1:open/closed curve)\n";
        pdout.clear();
        return false;
    }

    if( PROP2 != 0 && PROP2 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP2 value (" << PROP2 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, PROP3, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP3 (0/1:rational/polynomial)\n";
        pdout.clear();
        return false;
    }

    if( PROP3 != 0 && PROP3 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP3 value (" << PROP3 << ")\n";
        pdout.clear();
        return false;
    }

    if( !ParseInt( pdout, idx, PROP4, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read PROP4 (0/1:nonperiodic/periodic)\n";
        pdout.clear();
        return false;
    }

    if( PROP4 != 0 && PROP4 != 1 )
    {
        ERRMSG << "\n + [INFO] invalid PROP4 value (" << PROP4 << ")\n";
        pdout.clear();
        return false;
    }

    double tR;

    if( knots )
        delete [] knots;

    if( coeffs )
        delete [] coeffs;

    knots = NULL;
    coeffs = NULL;
    nKnots = 2 + K + M;
    knots = new double[nKnots];

    if( NULL == knots )
    {
        ERRMSG << "\n + [INFO] couldn't allocate memory for knots\n";
        pdout.clear();
        return false;
    }

    for( int i = 0; i < nKnots; ++i )
    {
        if( !ParseReal( pdout, idx, tR, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read knot value #" << (i + 1) << "\n";
            delete [] knots;
            knots = NULL;
            pdout.clear();
            return false;
        }

        knots[i] = tR;
    }

    nCoeffs = K + 1;

    if( 0 == PROP3 )
        coeffs = new double[nCoeffs * 4];   // rational
    else
        coeffs = new double[nCoeffs * 3];   // polynomial

    if( NULL == coeffs )
    {
        ERRMSG << "\n + [INFO] couldn't allocate memory for coefficients\n";
        delete [] knots;
        knots = NULL;
        pdout.clear();
        return false;
    }

    for( int i = 0, j = 3; i <= K; ++i )
    {
        if( !ParseReal( pdout, idx, tR, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read weight value #" << (i + 1) << "\n";
            delete [] knots;
            knots = NULL;
            delete [] coeffs;
            coeffs = NULL;
            pdout.clear();
            return false;
        }

        if( tR <= 0 )
        {
            ERRMSG << "\n + [CORRUPT FILE] invalid weight (" << tR << ")\n";
            delete [] knots;
            knots = NULL;
            delete [] coeffs;
            coeffs = NULL;
            pdout.clear();
            return false;
        }

        if( 0 == PROP3 )
        {
            coeffs[j] = tR;
            j += 4;
        }
    }

    double tX;
    double tY;
    double tZ;

    for( int i = 0, j = 0; i <= K; ++i )
    {
        if( !ParseReal( pdout, idx, tX, eor, pd, rd )
            || !ParseReal( pdout, idx, tY, eor, pd, rd )
            || !ParseReal( pdout, idx, tZ, eor, pd, rd ) )
        {
            ERRMSG << "\n + [INFO] couldn't read control point #" << (i + 1) << "\n";
            delete [] knots;
            knots = NULL;
            delete [] coeffs;
            coeffs = NULL;
            pdout.clear();
            return false;
        }

        coeffs[j++] = tX;
        coeffs[j++] = tY;
        coeffs[j++] = tZ;

        if( 0 == PROP3 )
            ++j;
    }

    if( !ParseReal( pdout, idx, V0, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read starting parameter value\n";
        delete [] knots;
        knots = NULL;
        delete [] coeffs;
        coeffs = NULL;
        pdout.clear();
        return false;
    }

    if( !ParseReal( pdout, idx, V1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read ending parameter value\n";
        delete [] knots;
        knots = NULL;
        delete [] coeffs;
        coeffs = NULL;
        pdout.clear();
        return false;
    }

    // unit normal vector (ignored if curve is not planar)
    if( !ParseReal( pdout, idx, tX, eor, pd, rd )
        || !ParseReal( pdout, idx, tY, eor, pd, rd )
        || !ParseReal( pdout, idx, tZ, eor, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] couldn't read unit normal vector\n";
        delete [] knots;
        knots = NULL;
        delete [] coeffs;
        coeffs = NULL;
        pdout.clear();
        return false;
    }

    if( PROP1 == 1 )
    {
        if( !CheckNormal( tX, tY, tZ ) )
        {
            ERRMSG << "\n + [INFO] bad normal\n";
            delete [] knots;
            knots = NULL;
            delete [] coeffs;
            coeffs = NULL;
            pdout.clear();
            return false;
        }

        vnorm.x = tX;
        vnorm.y = tY;
        vnorm.z = tZ;
    }
    else
    {
        vnorm.x = 0.0;
        vnorm.y = 0.0;
        vnorm.z = 1.0;
    }

    if( !eor && !readExtraParams( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read optional pointers\n";
        delete [] knots;
        knots = NULL;
        delete [] coeffs;
        coeffs = NULL;
        pdout.clear();
        return false;
    }

    if( !readComments( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read extra comments\n";
        delete [] knots;
        knots = NULL;
        delete [] coeffs;
        coeffs = NULL;
        pdout.clear();
        return false;
    }

    pdout.clear();
    return true;
}


bool IGES_ENTITY_126::SetEntityForm( int aForm )
{
    if( aForm != 0 && aForm != 1 && aForm != 2
        && aForm != 3 && aForm != 4 && aForm != 5 )
    {
        ERRMSG << "\n + [INFO] invalid Form(" << aForm;
        cerr << "), valid forms are 0..5 only\n";
        return false;
    }

    form = aForm;
    return true;
}


bool IGES_ENTITY_126::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    // hierarchy is ignored so always return true
    return true;
}


bool IGES_ENTITY_126::IsClosed( void )
{
    if( PROP2 )
        return true;

    return false;
}


bool IGES_ENTITY_126::IsPlanar( void )
{
    if( PROP1 )
        return true;

    return false;
}


bool IGES_ENTITY_126::IsRational( void )
{
    if( PROP3 )
        return false;

    return true;
}


bool IGES_ENTITY_126::isPeriodic( void )
{
    if( PROP4 )
        return true;

    return false;
}


bool IGES_ENTITY_126::GetNormal( MCAD_POINT& aNorm )
{
    aNorm = vnorm;
    return IsPlanar();
}


int IGES_ENTITY_126::GetNCurves( void )
{
    return 1;
}


IGES_CURVE* IGES_ENTITY_126::GetCurve( int index )
{
    // there are no child curves; return NULL
    return NULL;
}


bool IGES_ENTITY_126::GetStartPoint( MCAD_POINT& pt, bool xform )
{
    if( nCoeffs < 2 )
        return false;

    if( !scurve )
    {
        scurve = newCurve( nCoeffs, M + 1, knots, coeffs, PROP3 ? 1 : 2, 3, 0 );

        if( !scurve )
        {
            ERRMSG << "\n + [INFO] memory allocation failed in SISL newCurve()\n";
            return false;
        }
    }

    double vals[6];
    int kt = 0;
    double r = 0;
    int stat = 0;

    s1225( scurve, 0, V0, &kt, vals, &vals[3], &r, &stat );

    switch( stat )
    {
        case 0:
            break;

        case 1:
            ERRMSG << "\n + [WARNING] unspecified warning from SISL s1225() [evaluate position from left]\n";
            stat = 0;
            break;

        default:
            ERRMSG << "\n + [ERROR] SISL s1225() could not compute the position on a curve\n";
            return false;
            break;
    }

    pt.x = vals[0];
    pt.y = vals[1];
    pt.z = vals[2];

    if( xform && pTransform )
        pt = pTransform->GetTransformMatrix() * pt;

    return true;
}


bool IGES_ENTITY_126::GetEndPoint( MCAD_POINT& pt, bool xform )
{
    if( nCoeffs < 2 )
        return false;

    if( !scurve )
    {
        scurve = newCurve( nCoeffs, M + 1, knots, coeffs, PROP3 ? 1 : 2, 3, 0 );

        if( !scurve )
        {
            ERRMSG << "\n + [INFO] memory allocation failed in SISL newCurve()\n";
            return false;
        }
    }

    double vals[6];
    int kt = 0;
    double r = 0;
    int stat = 0;

    s1225( scurve, 0, V1, &kt, vals, &vals[3], &r, &stat );

    switch( stat )
    {
        case 0:
            break;

        case 1:
            ERRMSG << "\n + [WARNING] unspecified warning from SISL s1225() [evaluate position from left]\n";
            stat = 0;
            break;

        default:
            ERRMSG << "\n + [ERROR] SISL s1225() could not compute the position on a curve\n";
            return false;
            break;
    }

    pt.x = vals[0];
    pt.y = vals[1];
    pt.z = vals[2];

    if( xform && pTransform )
        pt = pTransform->GetTransformMatrix() * pt;

    return true;
}


int IGES_ENTITY_126::GetNSegments( void )
{
    // return the number of coefficients; this allows the user
    // to ensure that each piecewise section of curve is represented
    return nCoeffs;
}


bool IGES_ENTITY_126::Interpolate( MCAD_POINT& pt, int nSeg, double var, bool xform )
{
    pt.x = 0.0;
    pt.y = 0.0;
    pt.z = 0.0;

    if( nCoeffs < 2 )
    {
        ERRMSG << "\n + [ERROR] no data\n";
        return false;
    }

    if( !scurve )
    {
        scurve = newCurve( nCoeffs, M + 1, knots, coeffs, PROP3 ? 1 : 2, 3, 0 );

        if( !scurve )
        {
            ERRMSG << "\n + [INFO] memory allocation failed in SISL newCurve()\n";
            return false;
        }
    }

    if( var < 0.0 || var > 1.0 )
    {
        ERRMSG << "\n + [ERROR] var out of range (must be 0 .. 1.0)\n";
        return false;
    }

    if( nSeg < 0 || nSeg >= nCoeffs )
    {
        ERRMSG << "\n + [ERROR] nSeg out of range; max nSeg == " << (nCoeffs -1) << "\n";
        return false;
    }

    int idx0 = (nKnots - nCoeffs) >> 1;

    var = (1.0 - var) * knots[idx0 + nSeg] + var * knots[idx0 + nSeg + 1];

    double vals[6];
    int kt = 0;
    double r = 0;
    int stat = 0;

    s1225( scurve, 0, V1, &kt, vals, &vals[3], &r, &stat );

    switch( stat )
    {
        case 0:
            break;

        case 1:
            ERRMSG << "\n + [WARNING] unspecified warning from SISL s1225() [evaluate position from left]\n";
            stat = 0;
            break;

        default:
            ERRMSG << "\n + [ERROR] SISL s1225() could not compute the position on a curve\n";
            break;
    }

    pt.x = vals[0];
    pt.y = vals[1];
    pt.z = vals[2];

    if( xform && pTransform )
        pt = pTransform->GetTransformMatrix() * pt;

    return true;
}


bool IGES_ENTITY_126::GetNURBSData( int& nCoeff, int& order, double** knot, double** coeff, bool& isRational,
                                    bool& isClosed, bool& isPeriodic )
{
    nCoeff = 0;
    order =0 ;
    knot = NULL;
    coeff = NULL;

    if( !knots )
        return false;

    *knot = knots;
    *coeff = coeffs;
    nCoeff = nCoeffs;
    order = M + 1;

    if( PROP2 )
        isClosed = true;
    else
        isClosed = false;

    if( PROP3 )
        isRational = false;
    else
        isRational = true;

    if( PROP4 )
        isPeriodic = true;
    else
        isPeriodic = false;

    return true;
}


bool IGES_ENTITY_126::SetNURBSData( int nCoeff, int order, const double* knot, const double* coeff, bool isRational )
{
    if( !knot || !coeff )
    {
        ERRMSG << "\n + [INFO] invalid NURBS parameter pointer (NULL)\n";
        return false;
    }

    if( order < 2 )
    {
        ERRMSG << "\n + [INFO] invalid order; minimum is 2 which represents a line\n";
        return false;
    }

    if( nCoeff < order )
    {
        ERRMSG << "\n + [INFO] invalid number of control points; minimum is equal to the order of the B-Splines\n";
        return false;
    }

    // M = Degree of basis function; Order = Degree + 1
    // # of knots = 2 + K + M
    // # of coefficients = K + 1

    nKnots = nCoeff + order;
    nCoeffs = nCoeff;
    K = nCoeff - 1;
    M = order - 1;

    if( scurve )
    {
        freeCurve( scurve );
        scurve = NULL;
    }

    if( knots )
    {
        delete [] knots;
        knots = NULL;
    }

    if( coeffs )
    {
        delete [] coeffs;
        coeffs = NULL;
    }

    // flag whether the curve is rational or polynomial
    if( isRational )
        PROP3 = 0;
    else
        PROP3 = 1;

    knots = new double[nKnots];

    if( !knots )
    {
        ERRMSG << "\n + [INFO] memory allocation failed for knots[]\n";
        return false;
    }

    int nDbls;

    if( isRational )
        nDbls = nCoeffs * 4;
    else
        nDbls = nCoeffs * 3;

    coeffs = new double[nDbls];

    if( !coeffs )
    {
        ERRMSG << "\n + [INFO] memory allocation failed for coeffs[]\n";
        delete [] knots;
        knots = NULL;
        return false;
    }

    for( int i = 0; i < nKnots; ++i )
        knots[i] = knot[i];

    for( int i = 0; i < nDbls; ++i )
        coeffs[i] = coeff[i];

    scurve = newCurve( nCoeffs, M + 1, knots, coeffs, PROP3 ? 1 : 2, 3, 0 );

    if( !scurve )
    {
        ERRMSG << "\n + [INFO] memory allocation failed in SISL newCurve()\n";
        return false;
    }

    int stat = 0;
    s1363( scurve, &V0, &V1, &stat );

    switch ( stat )
    {
    case 0:
        break;

    case 1:
        ERRMSG << "\n + [WARNING] unspecified problems determining V0, V1 parameter values\n";
        stat = 0;
        break;

    default:
        ERRMSG << "\n + [INFO] could not determine V0, V1 parameter values\n";
        return false;
        break;
    }

    if( 0.0 == V0 && 1.0 != V1 )
    {
        // normalize the knot vector
        for( int i = 0; i < nKnots; ++i )
            knots[i] /= V1;

        V1 = 1.0;
    }


    // determine planarity
    if( hasUniquePlane( &vnorm ) )
        PROP1 = 1;
    else
        PROP1 = 0;

    // determine periodicity, and closure
    double uir = 1e-8;
    stat = 0;

    if( parent )
        uir = parent->globalData.minResolution;

    s1364( scurve, uir, &stat );

    switch( stat )
    {
        case 2:
            // closed and periodic
            PROP2 = 1;
            PROP4 = 1;
            break;

        case 1:
            // curve is closed
            PROP2 = 1;
            PROP4 = 0;
            break;

        case 0:
            // curve is open
            PROP2 = 0;
            PROP4 = 0;
            break;

        default:
            ERRMSG << "\n + [ERROR] s1364() failed\n";
            return false;
    }

    return true;
}


bool IGES_ENTITY_126::hasUniquePlane( MCAD_POINT* norm )
{
    // if we have a line (or no data) return false
    if( nCoeffs < 3 )
    {
        norm->x = 0.0;
        norm->y = 0.0;
        norm->z = 1.0;
        return false;
    }

    // we must test for planarity by taking the normal vector of every
    // 3 control points; if all normals are equal (or anti) then we have a plane
    MCAD_POINT p0;
    MCAD_POINT p1;
    MCAD_POINT p2;

    MCAD_POINT* pts[3] = { &p0, &p1, &p2 };

    int i = 0;
    int j = 0;

    MCAD_POINT tnorm0;
    MCAD_POINT tnorm1;

    for( i = 0; i < 3; ++i )
    {
        pts[i]->x = coeffs[j++];
        pts[i]->y = coeffs[j++];
        pts[i]->z = coeffs[j++];

        if( 0 == PROP3 )
            ++j;
    }

    CalcNormal( &p0, &p1, &p2, norm );

    if( nCoeffs == 3 )
        return true;

    tnorm0 = *norm;
    MCAD_POINT* px;

    for( i = 3; i < nCoeffs; ++i )
    {
        px = pts[0];
        pts[0] = pts[1];
        pts[1] = pts[2];
        pts[2] = px;

        pts[2]->x = coeffs[j++];
        pts[2]->y = coeffs[j++];
        pts[2]->z = coeffs[j++];

        if( 0 == PROP3 )
            ++j;

        CalcNormal( pts[0], pts[1], pts[2], &tnorm1 );

        if( !PointMatches( tnorm0, tnorm1, 1e-8 )
            && !PointMatches( tnorm0, tnorm1 * -1.0, 1e-8 ) )
        {
            norm->x = 0.0;
            norm->y = 0.0;
            norm->z = 1.0;
            return false;
        }

        tnorm0 = tnorm1;
    }

    return true;
}
