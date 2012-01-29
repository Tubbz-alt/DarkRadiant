#pragma once

#include "ibrush.h"
#include "math/Plane3.h"

namespace map
{

// TODO: hack
#ifdef __linux__
#define _alloca alloca
#endif

#define MAX_WORLD_COORD	( 128 * 1024 )
#define MIN_WORLD_COORD	( -128 * 1024 )
#define MAX_WORLD_SIZE	( MAX_WORLD_COORD - MIN_WORLD_COORD )

#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2
#define	SIDE_CROSS	3

#define ON_EPSILON 0.1f

class ProcWinding : 
	public IWinding
{
public:
	ProcWinding() :
		IWinding()
	{}

	ProcWinding(const Plane3& plane) :
		IWinding(4) // 4 points for this plane
	{
		setFromPlane(plane);
	}

	// Creates a near-infinitely large winding from the given plane
	void setFromPlane(const Plane3& plane);

	// Clips this winding against the given plane
	void clip(const Plane3& plane, const float epsilon = 0.0f);

	// Splits this winding in two, using the given plane as splitter. This winding itself stays unaltered
	int split(const Plane3& plane, const float epsilon, ProcWinding& front, ProcWinding& back) const;

	int planeSide(const Plane3& plane, const float epsilon = ON_EPSILON) const;

	// A winding is not tiny if at least three edges are above a certain threshold
	bool isTiny() const;

	// A base winding made from a plane is typically huge
	bool isHuge() const;

	float getArea() const;

	Vector3 getCenter() const;

private:
	Plane3 getPlane() const;
};

} // namespace
