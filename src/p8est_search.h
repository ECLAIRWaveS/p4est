/*
  This file is part of p4est.
  p4est is a C library to manage a parallel collection of quadtrees and/or
  octrees.

  Copyright (C) 2008,2009 Carsten Burstedde, Lucas Wilcox.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef P8EST_SEARCH_H
#define P8EST_SEARCH_H

#include <p8est.h>

SC_EXTERN_C_BEGIN;

/** Find the lowest position tq in a quadrant array such that tq >= q.
 * \return  Returns the id of the matching quadrant
 *                  or -1 if not found or the array is empty.
 */
ssize_t             p8est_find_lower_bound (sc_array_t * array,
                                            const p8est_quadrant_t * q,
                                            size_t guess);

/** Find the highest position tq in a quadrant array such that tq <= q.
 * \return  Returns the id of the matching quadrant
 *                  or -1 if not found or the array is empty.
 */
ssize_t             p8est_find_higher_bound (sc_array_t * array,
                                             const p8est_quadrant_t * q,
                                             size_t guess);

/** Given a sorted \a array of quadrants that have a common ancestor at level
 * \a level, compute the \a indices of the first quadrant in each of the common
 * ancestor's children at level \a level + 1;
 * \param [in] array     The sorted array of quadrants of level > \a level.
 * \param [in] level     The level at which there is a common ancestor.
 * \param [in,out] indices     The indices of the first quadrant in each of
 *                             the ancestors's children, plus an additional
 *                             index on the end.  The quadrants of \a array
 *                             that are descendents of child i have indices
 *                             between indices[i] and indices[i + 1] - 1.  If
 *                             indices[i] = indices[i+1], this indicates that
 *                             no quadrant in the array is contained in
 *                             child i.
 */
void                p8est_split_array (sc_array_t * array, int level,
                                       size_t indices[]);

/** Given two smallest quadrants, \a lq and \a uq, that mark the first and the
 * last quadrant in a range of quadrants, determine which portions of the tree
 * boundary the range touches.
 * \param [in] lq        The smallest quadrant at the start of the range: if
 *                       NULL, the tree's first quadrant is taken to be the
 *                       start of the range.
 * \param [in] uq        The smallest quadrant at the end of the range: if
 *                       NULL, the tree's last quadrant is taken to be the
 *                       end of the range.
 * \param [in] level     The level of the containing quadrant whose boundaries
 *                       are tested: 0 if we want to test the boundaries of the
 *                       whole tree.
 * \param [in/out] faces       An array of size 6 that is filled: faces[i] is
 *                             true if the range touches that face.
 * \param [in/out] edges       An array of size 12 that is filled: edges[i] is
 *                             true if the range touches that edge.
 * \param [in/out] corners     An array of size 8 that is filled: corners[i] is
 *                             true if the range touches that corner.
 *                             \faces, \edges or \corners may be NULL.
 * \return  Returns an int32_t encoded with the same information in \faces,
 *          \edges and \corners: the first (least) six bits represent the six
 *          faces, the next twelve bits represent the twelve edges, the next
 *          eight bits represent the eight corners.
 */
int32_t             p8est_find_range_boundaries (p8est_quadrant_t * lq,
                                                 p8est_quadrant_t * uq,
                                                 int level, int faces[],
                                                 int edges[], int corners[]);

/** Callback function to query the match of a "point" with a quadrant.
 *
 * \param [in] p8est        The forest to be queried.
 * \param [in] which_tree   The tree id under consideration.
 * \param [in] quadrant     The quadrant under consideration.
 *                          This quadrant may be coarser than the quadrants
 *                          that are contained in the forest (an ancestor).
 * \param [in] point        Representation of a "point"; user-defined.
 * \param [in] is_leaf      Specify if the quadrant is an ancestor or a leaf.
 * \return                  True if point may be contained in the quadrant,
 *                          false otherwise.  By returning true for a leaf,
 *                          a successful match is indicated.
 */
typedef int         (*p8est_search_query_t) (p8est_t * p8est,
                                             p4est_topidx_t which_tree,
                                             p8est_quadrant_t * quadrant,
                                             int is_leaf, void *point);

/** Search "points" from a given set in the forest.
 *
 * The search goes over all trees and proceeds recursively top-down.
 * A callback is queried to match each point with a quadrant.
 * The callback is allowed to return true for the same point and more than one
 * quadrant; in this case more than one matching quadrant may be identified.
 * The callback is also allowed to return false for all children
 * of a quadrant that it returned true for earlier.
 * The points can really be anything, p8est does not perform any
 * interpretation, just passes the pointer along to the callback function.
 *
 * \param [in] p8est        The forest to be searched.
 * \param [in] search_fn    Function to return true for a possible match.
 * \param [in] points       User-defined array of "points".
 */
void                p8est_search (p8est_t * p8est,
                                  p8est_search_query_t search_fn,
                                  sc_array_t * points);

SC_EXTERN_C_END;

#endif
