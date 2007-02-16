#ifndef RENDERABLEPICOSURFACE_H_
#define RENDERABLEPICOSURFACE_H_

#include "VectorLightList.h"

#include "GLProgramAttributes.h"
#include "picomodel.h"
#include "cullable.h"
#include "render.h"
#include "math/aabb.h"

/* FORWARD DECLS */
class ModelSkin;
class Renderer;
class SelectionTest;
class Selector;
class Shader;

namespace model
{

/* Renderable class containing a series of polygons textured with the same
 * material. RenderablePicoSurface objects are composited into a RenderablePicoModel
 * object to create a renderable static mesh.
 */

class RenderablePicoSurface
: public OpenGLRenderable,
  public Cullable
{
	// Name of the material this surface is using, both originally and after a skin
	// remap.
	std::string _originalShaderName;
	std::string _mappedShaderName;
	
	// Shader object containing the material shader for this surface
	Shader* _shader;
	
	// Vector of ArbitraryMeshVertex structures, containing the coordinates,
	// normals, tangents and texture coordinates of the component vertices
	typedef std::vector<ArbitraryMeshVertex> VertexVector; 
	VertexVector _vertices;
	
	// Vector of render indices, representing the groups of vertices to be
	// used to create triangles
	typedef std::vector<unsigned int> Indices;
	Indices _indices;
	
	// Keep track of the number of indices to iterate over, since vector::size()
	// may not be fast
	unsigned int _nIndices;

	// The AABB containing this surface, in local object space.
	AABB _localAABB;
	
	// Vector of RendererLight references which illuminate this surface
	VectorLightList _lights;

private:

	// Calculate tangent and bitangent vectors for all vertices.
	void calculateTangents();

public:

	/** Constructor. Accepts a picoSurface_t struct and the file extension to determine
	 * how to assign materials.
	 */
	RenderablePicoSurface(picoSurface_t* surf, const std::string& fExt);
	
	/** 
	 * Destructor. Vectors will be automatically destructed, but we need to 
	 * release the shader.
	 */
	~RenderablePicoSurface();
	
	/**
	 * Front-end render function used by the main renderer.
	 * 
	 * @param rend
	 * The sorting Renderer object which accepts renderable geometry.
	 * 
	 * @param localToWorld
	 * Object to world-space transform.
	 */
	void submitRenderables(Renderer& rend, const Matrix4& localToWorld);		
	
	/** 
	 * Render function from OpenGLRenderable
	 */
	void render(RenderStateFlags flags) const;
	
	/**
	 * Add a light to this surface. No volume intersection tests are performed,
	 * so this should already have been done by the PicoModel.
	 */
	void addLight(const RendererLight& light);
	
	/**
	 * Remove all lights from this surface.
	 */
	void clearLights();
	
	/** Return the vertex count for this surface
	 */
	int getVertexCount() const {
		return _vertices.size();
	}
	
	/** Return the poly count for this surface
	 */
	int getPolyCount() const {
		return _indices.size() / 3; // 3 indices per triangle
	}
	
	/** Get the Shader for this surface.
	 */
	Shader* getShader() const {
		return _shader;
	}	 
	
	/** Get the active shader name for this surface, after any skin remaps.
	 */
	const std::string& getActiveMaterial() const {
		return _mappedShaderName;
	}

	/** Get the containing AABB for this surface.
	 */
	const AABB& getAABB() const {
		return _localAABB;	
	}
	
	/** Apply the provided skin to this surface. If the skin has a remap for
	 * this surface's material, it will be applied, otherwise no action will
	 * occur.
	 * 
	 * @param skin
	 * ModelSkin object to apply to this surface.
	 */
	void applySkin(const ModelSkin& skin);
	
	/**
	 * Perform a volume intersection (AABB) test on this surface.
	 */
	VolumeIntersectionValue intersectVolume(const VolumeTest& test,
									 		const Matrix4& localToWorld) const;
									 		
	/** 
	 * Perform a selection test on this surface.
	 */
	void testSelect(Selector& selector, 
					SelectionTest& test,
					const Matrix4& localToWorld) const; 
};

}

#endif /*RENDERABLEPICOSURFACE_H_*/
