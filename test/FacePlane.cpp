#include "RadiantTest.h"

#include "ibrush.h"
#include "icommandsystem.h"
#include "iselection.h"
#include "itransformable.h"
#include "imap.h"
#include "scenelib.h"
#include "math/Matrix4.h"
#include "math/Quaternion.h"
#include "math/Vector3.h"

namespace test
{

class FacePlaneTest :
    public RadiantTest
{
protected:
    scene::INodePtr _brushNode;

    void performTest(const std::function<bool(const IBrushNodePtr&)>& functor)
    {
        auto worldspawn = GlobalMapModule().findOrInsertWorldspawn();

        _brushNode = GlobalBrushCreator().createBrush();
        worldspawn->addChildNode(_brushNode);

        GlobalSelectionSystem().setSelectedAll(false);
        Node_setSelected(_brushNode, true);

        const double size = 15;
        Vector3 startPos(-size, -size, -size);
        Vector3 endPos(size, size, size);
        GlobalCommandSystem().executeCommand("ResizeSelectedBrushesToBounds",
            startPos, endPos, std::string("shader"));

        auto result = functor(std::dynamic_pointer_cast<IBrushNode>(_brushNode));

        scene::removeNodeFromParent(_brushNode);
        _brushNode.reset();

        EXPECT_TRUE(result) << "Test failed to perform anything.";
    }
};

TEST_F(FacePlaneTest, FacePlaneRotateWithMatrix)
{
    performTest([this](const IBrushNodePtr& brush)
    {
        // Get the plane facing down the x axis and check it
        for (std::size_t i = 0; i < brush->getIBrush().getNumFaces(); ++i)
        {
            auto& face = brush->getIBrush().getFace(i);

            if (face.getPlane3().normal().isParallel(Vector3(1, 0, 0)))
            {
                Plane3 orig = face.getPlane3();

                // Transform the plane with a rotation matrix
                Matrix4 rot = Matrix4::getRotation(Vector3(0, 1, 0), 2);

                Node_getTransformable(_brushNode)->setRotation(Quaternion::createForMatrix(rot));
                Node_getTransformable(_brushNode)->freezeTransform();

                EXPECT_NE(face.getPlane3(), orig);
                EXPECT_EQ(face.getPlane3(), orig.transformed(rot));
                EXPECT_NEAR(face.getPlane3().normal().getLength(), 1, 0.001);

                return true;
            }
        }

        return false;
    });
}

TEST_F(FacePlaneTest, FacePlaneTranslate)
{
    performTest([this](const IBrushNodePtr& brush)
    {
        // Get the plane facing down the x axis and check it
        for (std::size_t i = 0; i < brush->getIBrush().getNumFaces(); ++i)
        {
            auto& face = brush->getIBrush().getFace(i);

            if (face.getPlane3().normal().isParallel(Vector3(0, 1, 0)))
            {
                Plane3 orig = face.getPlane3();

                // Translate in the Y direction
                const Vector3 translation(0, 3, 0);

                Node_getTransformable(_brushNode)->setTranslation(translation);
                Node_getTransformable(_brushNode)->freezeTransform();

                EXPECT_NE(face.getPlane3(), orig);
                EXPECT_EQ(face.getPlane3().normal(), orig.normal());
                EXPECT_EQ(face.getPlane3().dist(), orig.dist() + translation.y());
                EXPECT_NEAR(face.getPlane3().normal().getLength(), 1, 0.001);

                return true;
            }
        }

        return false;
    });
}

}
