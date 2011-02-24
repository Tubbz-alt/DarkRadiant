#pragma once

#include <cstddef>
#include "imodule.h"
#include <boost/shared_ptr.hpp>
#include <boost/function/function_fwd.hpp>

class RenderableCollector;
class View;

class Selectable;


namespace scene
{
	class INode;
	typedef boost::shared_ptr<INode> INodePtr;
}

template<typename Element> class BasicVector2;
typedef BasicVector2<float> Vector2;
template<typename Element> class BasicVector3;
typedef BasicVector3<float> Vector3;
template<typename Element> class BasicVector4;
typedef BasicVector4<float> Vector4;
class Matrix4;
typedef Vector4 Quaternion;

typedef boost::function<void (const Selectable&)> SelectionChangeCallback;

class SelectionInfo;

namespace selection { struct WorkZone; }

const std::string MODULE_SELECTIONSYSTEM("SelectionSystem");

class SelectionSystem :
	public RegisterableModule
{
public:
  enum EModifier {
	eManipulator,	// greebo: This is the standard case (drag, click without modifiers)
	eToggle,	// This is for Shift-Clicks to toggle the selection of an instance
	eReplace,	// This is active if the mouse is moved to a NEW location and Alt-Shift is held
	eCycle,		// This is active if the mouse STAYS at the same position and Alt-Shift is held
  };

  enum EMode
  {
    eEntity,
    ePrimitive,
	eGroupPart,
    eComponent,
  };

	// The possible modes when in "component manipulation mode"
	enum EComponentMode {
		eDefault,
		eVertex,
		eEdge,
		eFace,
	};

	// The possible manipulator modes
	enum EManipulatorMode {
		eTranslate,
		eRotate,
		eScale,
		eDrag,
		eClip,
	};

	/** greebo: An SelectionSystem::Observer gets notified
	 * as soon as the selection is changed.
	 */
	class Observer
	{
	public:
		virtual ~Observer() {}
		/** greebo: This gets called upon selection change.
		 *
		 * @instance: The instance that got affected (this may also be the parent brush of a FaceInstance).
		 * @isComponent: is TRUE if the changed selectable is a component (like a FaceInstance, VertexInstance).
		 */
		virtual void selectionChanged(const scene::INodePtr& node, bool isComponent) = 0;
	};

	virtual void addObserver(Observer* observer) = 0;
	virtual void removeObserver(Observer* observer) = 0;

	virtual const SelectionInfo& getSelectionInfo() = 0;

  virtual void SetMode(EMode mode) = 0;
  virtual EMode Mode() const = 0;
  virtual void SetComponentMode(EComponentMode mode) = 0;
  virtual EComponentMode ComponentMode() const = 0;
  virtual void SetManipulatorMode(EManipulatorMode mode) = 0;
  virtual EManipulatorMode ManipulatorMode() const = 0;

  virtual std::size_t countSelected() const = 0;
  virtual std::size_t countSelectedComponents() const = 0;
  virtual void onSelectedChanged(const scene::INodePtr& node, const Selectable& selectable) = 0;
  virtual void onComponentSelection(const scene::INodePtr& node, const Selectable& selectable) = 0;

	virtual scene::INodePtr ultimateSelected() = 0;
	virtual scene::INodePtr penultimateSelected() = 0;

    /**
     * \brief
     * Set the selection status of all objects in the scene.
     *
     * \param selected
     * true to select all objects, false to deselect all objects.
     */
    virtual void setSelectedAll(bool selected) = 0;

  virtual void setSelectedAllComponents(bool selected) = 0;

    /**
     * @brief
     * Visitor interface the for the selection system.
     *
     * This defines the Visitor interface which is used in the foreachSelected()
     * and foreachSelectedComponent() visit methods.
     */
	class Visitor
	{
	public:
		virtual ~Visitor() {}

        /**
         * @brief
         * Called by the selection system for each visited node.
         */
		virtual void visit(const scene::INodePtr& node) const = 0;
	};

    /**
     * @brief
     * Use the provided Visitor object to enumerate each selected node.
     */
    virtual void foreachSelected(const Visitor& visitor) = 0;

    /**
     * @brief
     * Use the provided Visitor object to enumerate each selected component.
     */
    virtual void foreachSelectedComponent(const Visitor& visitor) = 0;

	/**
	 * greebo: Connect a signal handler to get notified about selection changes.
	 */
	virtual void addSelectionChangeCallback(const SelectionChangeCallback& callback) = 0;

  virtual void NudgeManipulator(const Vector3& nudge, const Vector3& view) = 0;

  virtual void translateSelected(const Vector3& translation) = 0;
  virtual void rotateSelected(const Quaternion& rotation) = 0;
  virtual void scaleSelected(const Vector3& scaling) = 0;

  virtual void pivotChanged() const = 0;

  virtual bool SelectManipulator(const View& view, const Vector2& devicePoint, const Vector2& deviceEpsilon) = 0;
  virtual void SelectPoint(const View& view, const Vector2& devicePoint, const Vector2& deviceEpsilon, EModifier modifier, bool face) = 0;
  virtual void SelectArea(const View& view, const Vector2& devicePoint, const Vector2& deviceDelta, EModifier modifier, bool face) = 0;

  virtual void MoveSelected(const View& view, const Vector2& devicePoint) = 0;
  virtual void endMove() = 0;
  virtual void cancelMove() = 0;

	/**
	 * Returns the current "work zone", which is defined by the
	 * currently selected elements. Each time a scene node is selected,
	 * the workzone is adjusted to surround the current selection.
	 * Deselecting nodes doesn't change the workzone.
	 *
	 * The result is used to determine the "third" component of operations
	 * performed in the 2D views, like placing an entity.
	 *
	 * Note: the struct is defined in selectionlib.h.
	 */
	virtual const selection::WorkZone& getWorkZone() = 0;
};

inline SelectionSystem& GlobalSelectionSystem() {
	// Cache the reference locally
	static SelectionSystem& _selectionSystem(
		*boost::static_pointer_cast<SelectionSystem>(
			module::GlobalModuleRegistry().getModule(MODULE_SELECTIONSYSTEM)
		)
	);
	return _selectionSystem;
}
