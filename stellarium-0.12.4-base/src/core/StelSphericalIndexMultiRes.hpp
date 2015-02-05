/*
 * Stellarium
 * Copyright (C) 2009 Fabien Chereau
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#ifndef _STELSPHERICALINDEXMULTIRES_HPP_
#define _STELSPHERICALINDEXMULTIRES_HPP_

#define MAX_INDEX_LEVEL 8

#include "StelRegionObject.hpp"
#include "StelSphericalIndex.hpp"

//! @class StelSphericalIndexMultiRes
//! Container allowing to store and query SphericalRegion.
class StelSphericalIndexMultiRes : public StelSphericalIndex
{
public:
	StelSphericalIndexMultiRes(int maxObjectsPerNode = 100);
	virtual ~StelSphericalIndexMultiRes();

	//! Insert the given object in the StelSphericalIndexMultiRes.
	void insert(StelRegionObjectP obj);

	//! Process all the objects intersecting the given region using the passed function object.
	template<class FuncObject> void processIntersectingRegions(const SphericalRegionP& region, FuncObject& func) const
	{
		for (int i=1;i<MAX_INDEX_LEVEL;++i)
		{
			const RootNode* node = treeForRadius[i-1];
			if (node!=NULL)
				node->processIntersectingRegions(region, func);
		}
	}

	//! Process all the objects intersecting the given region using the passed function object.
	template<class FuncObject> void processAll(FuncObject& func) const
	{
		for (int i=1;i<MAX_INDEX_LEVEL;++i)
		{
			const RootNode* node = treeForRadius[i-1];
			if (node!=NULL)
				node->processAll(func);
		}
	}

private:
	
	//! The elements stored in the container.
	struct NodeElem
	{
		NodeElem() {;}
		NodeElem(StelRegionObjectP aobj) : obj(aobj), cap(obj->getRegion()->getBoundingCap()) {;}
		StelRegionObjectP obj;
		SphericalCap cap;
	};

	//! @class Node
	//! The base node class. Final nodes contain a list of NodeElem, other
	//! nodes link to child nodes subdivising it spatially.
	struct Node
	{
		QVector<NodeElem> elements;
		QVector<Node> children;
		SphericalConvexPolygon triangle;
		//! Split each triangles in to 4 subtriangles.
		virtual void split()
		{
			// Default implementation for HTM triangle more than level 0
			Q_ASSERT(children.empty());
			Q_ASSERT(triangle.getConvexContour().size() == 3);

			const Vec3d& c0 = triangle.getConvexContour().at(0);
			const Vec3d& c1 = triangle.getConvexContour().at(1);
			const Vec3d& c2 = triangle.getConvexContour().at(2);

			Q_ASSERT((c1^c0)*c2 >= 0.0);
			Vec3d e0(c1[0]+c2[0], c1[1]+c2[1], c1[2]+c2[2]);
			e0.normalize();
			Vec3d e1(c2[0]+c0[0], c2[1]+c0[1], c2[2]+c0[2]);
			e1.normalize();
			Vec3d e2(c0[0]+c1[0], c0[1]+c1[1], c0[2]+c1[2]);
			e2.normalize();

			children.resize(4);
			children[0].triangle = SphericalConvexPolygon(e1,c0,e2);
			Q_ASSERT(children[0].triangle.checkValid());
			children[1].triangle = SphericalConvexPolygon(e0,e2,c1);
			Q_ASSERT(children[1].triangle.checkValid());
			children[2].triangle = SphericalConvexPolygon(c2,e1,e0);
			Q_ASSERT(children[2].triangle.checkValid());
			children[3].triangle = SphericalConvexPolygon(e2,e0,e1);
			Q_ASSERT(children[3].triangle.checkValid());
		}
	};

	//! @class RootNode
	//! The first Node of a tree. It has a special subdivision of the sphere in an octahedron.
	class RootNode : public Node
	{
		public:
			RootNode(double amargin, int amaxObjectsPerNode, int amaxLevel) : maxObjectsPerNode(amaxObjectsPerNode), maxLevel(amaxLevel), margin(amargin)
			{
			}
		
			//! Create the 8 triangles of the octahedron.
			virtual void split()
			{
				static const Vec3d vertice[6] =
				{
					Vec3d(0,0,1), Vec3d(1,0,0), Vec3d(0,1,0), Vec3d(-1,0,0), Vec3d(0,-1,0), Vec3d(0,0,-1)
				};

				static const int verticeIndice[8][3] =
				{
					{0,2,1}, {0,1,4}, {0,4,3}, {0,3,2}, {5,1,2}, {5,4,1}, {5,3,4}, {5,2,3}
				};
			
				// Create the 8 base triangles
				Node node;
				for (int i=0;i<8;++i)
				{
					node.triangle = SphericalConvexPolygon(vertice[verticeIndice[i][0]], vertice[verticeIndice[i][1]], vertice[verticeIndice[i][2]]);
					Q_ASSERT(node.triangle.checkValid());
					children.append(node);
				}
			}
		
			//! Insert the given element in the StelSphericalIndexMultiRes.
			void insert(const NodeElem& el, int level)
			{
				insert(*this, el, level);
			}
		
			//! Process all the objects intersecting the given region using the passed function object.
			template<class FuncObject> void processIntersectingRegions(const SphericalRegionP& region, FuncObject& func) const
			{
				processIntersectingRegions(*this, region->getEnlarged(margin), func);
			}
		
			//! Process all the objects intersecting the given region using the passed function object.
			template<class FuncObject> void processAll(FuncObject& func) const
			{
				processAll(*this, func);
			}
				
		private:
			//! Insert the given element in the given node.
			void insert(Node& node, const NodeElem& el, int level)
			{
				if (node.children.isEmpty())
				{
					node.elements.append(el);
					// If we have too many objects in the node, we split it
					if (level>=maxLevel && node.elements.size() >= maxObjectsPerNode)
					{
						node.split();
						const QVector<NodeElem> nodeElems = node.elements;
						for (QVector<NodeElem>::ConstIterator iter = nodeElems.begin();iter != nodeElems.end(); ++iter)
						{
							insert(node, *iter, level+1);
						}
						node.elements.clear();
					}
				}
				else
				{
					// If we have children, store it in a sub-level
					for (QVector<Node>::iterator iter = node.children.begin(); iter!=node.children.end(); ++iter)
					{
						if (iter->triangle.contains(el.cap.n))
						{
							insert(*iter, el, level+1);
							return;
						}
					}
				}
			}

			//! Process all the objects intersecting the given region using the passed function object.
			template<class FuncObject> void processIntersectingRegions(const Node& node, const SphericalRegionP& region, FuncObject& func) const
			{
				foreach (const NodeElem& el, node.elements)
				{
					if (region->intersects(el.obj->getRegion()))
						func(el.obj);
				}
				foreach (const Node& child, node.children)
				{
					if (region->contains(node.triangle))
						processAll(child, func);
					else if (region->intersects(node.triangle))
						processIntersectingRegions(child, region, func);
				}
			}

			//! Process all the objects intersecting the given region using the passed function object.
			template<class FuncObject> void processAll(const Node& node, FuncObject& func) const
			{
				foreach (const NodeElem& el, node.elements)
					func(el.obj);
				foreach (const Node& child, node.children)
					processAll(child, func);
			}
		
			//! The maximum number of objects per node.
			int maxObjectsPerNode;
			//! The maximum level of the grid. Prevents grid split into too small triangles if unecessary.
			int maxLevel;
			//! The margin in radian to add to the query region.
			double margin;
	};
	
	//! The maximum allowed number of object per node.
	int maxObjectsPerNode;
	
	//! One tree per object radius. Each query is propagated to each tree with the proper additional margin.
	RootNode* treeForRadius[MAX_INDEX_LEVEL];
	
	double cosRadius[MAX_INDEX_LEVEL];
};

#endif // _STELSPHERICALINDEXMULTIRES_HPP_

