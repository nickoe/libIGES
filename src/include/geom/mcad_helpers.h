/*
 * file: mcad_helpers.h
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: miscellaneous useful functions
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

#ifndef MCAD_HELPERS_H
#define MCAD_HELPERS_H

#include <string>
#include <mcad_elements.h>

// return true if the 2 points match to within 'minRes'
bool PointMatches( MCAD_POINT p1, MCAD_POINT p2, double minRes );

// check and renormalize a vector; return false if vector is invalid
bool CheckNormal( double& X, double &Y, double& Z );

// print out an IGES transform
class MCAD_TRANSFORM;
void print_transform( const MCAD_TRANSFORM* T );

// print out an IGES matrix
class MCAD_MATRIX;
void print_matrix( const MCAD_MATRIX* m );

// print out an IGES point (decimal places limited to 3)
class MCAD_POINT;
void print_vec( const MCAD_POINT* p );

// calculate the normal given points p0, p1, p2
bool CalcNormal( const MCAD_POINT* p0, const MCAD_POINT* p1, const MCAD_POINT* p2, MCAD_POINT* pn );

#endif  // MCAD_HELPERS_H
