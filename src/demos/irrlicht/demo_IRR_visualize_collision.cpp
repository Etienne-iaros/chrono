// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2018 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.tx
// =============================================================================
// Authors: Kishor Bhalerao, Radu Serban
// =============================================================================
//
// Demonstrate the use of the debug collision visualization callback with the
// Bullet or Chrono collision system.
// =============================================================================

#include "chrono/collision/ChCollisionSystemBullet.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono/physics/ChBodyEasy.h"
#include "chrono/core/ChTimer.h"
#include "chrono/core/ChRealtimeStep.h"

#include "chrono_irrlicht/ChIrrApp.h"

using namespace chrono;
using namespace chrono::collision;
using namespace chrono::irrlicht;

auto csys_type = collision::ChCollisionSystemType::BULLET;
////auto csys_type = collision::ChCollisionSystemType::CHRONO;

class DebugDrawer : public ChCollisionSystem::VisualizationCallback {
  public:
    explicit DebugDrawer(ChIrrApp* app) : m_app(app) {}
    ~DebugDrawer() {}

    virtual void DrawLine(const ChVector<>& from, const ChVector<>& to, const ChColor& color) override {
        m_app->GetVideoDriver()->draw3DLine(irr::core::vector3dfCH(from), irr::core::vector3dfCH(to),
                                            irr::video::SColor(255, color.R * 255, color.G * 255, color.B * 255));
    }

    virtual double GetNormalScale() const override { return 1.0; }

    void Draw(int flags, bool use_zbuffer = true) {
        m_app->GetVideoDriver()->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());
        irr::video::SMaterial mattransp;
        mattransp.ZBuffer = use_zbuffer;
        mattransp.Lighting = false;
        m_app->GetVideoDriver()->setMaterial(mattransp);

        m_app->GetSystem()->GetCollisionSystem()->Visualize(flags);
    }

  private:
    ChIrrApp* m_app;
};

int main(int argc, char* argv[]) {
    GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";

    // Create the Chrono system, bodies, and collison shapes
    ChSystemNSC sys;
    sys.SetCollisionSystemType(csys_type);

    auto mat = chrono_types::make_shared<ChMaterialSurfaceNSC>();

    auto ground = chrono_types::make_shared<ChBodyEasyBox>(10, 3, 10, 100, mat, csys_type);
    ground->SetBodyFixed(true);
    ground->SetPos(ChVector<>(0.0, 0.0, 0.0));
    ground->AddAsset(chrono_types::make_shared<ChColorAsset>(0.2f, 0.3f, 0.6f));
    sys.AddBody(ground);

    auto cyl = chrono_types::make_shared<ChBodyEasyCylinder>(0.5, 1.0, 100, mat, csys_type);
    cyl->SetPos(ChVector<>(0.0, 3.0, 0.0));
    cyl->AddAsset(chrono_types::make_shared<ChColorAsset>(0.2f, 0.3f, 0.6f));
    sys.AddBody(cyl);

    auto box = chrono_types::make_shared<ChBodyEasyBox>(0.5, 0.5, 0.5, 100, mat, csys_type);
    box->SetPos(ChVector<>(0.2, 2.0, 0.0));
    box->AddAsset(chrono_types::make_shared<ChColorAsset>(0.2f, 0.3f, 0.6f));
    sys.AddBody(box);

    auto sphere = chrono_types::make_shared<ChBodyEasySphere>(0.25, 100.0, mat, csys_type);
    sphere->SetPos(ChVector<>(-0.2, 2.0, 0.75));
    sphere->AddAsset(chrono_types::make_shared<ChColorAsset>(0.2f, 0.3f, 0.6f));
    sys.AddBody(sphere);

    auto ellipse = chrono_types::make_shared<ChBodyEasyEllipsoid>(ChVector<>(0.2, 0.4, 0.6), 100.0, mat, csys_type);
    ellipse->SetPos(ChVector<>(0.2, 2.0, -1.0));
    ellipse->AddAsset(chrono_types::make_shared<ChColorAsset>(0.2f, 0.3f, 0.6f));
    sys.AddBody(ellipse);

    auto mesh = chrono_types::make_shared<ChBodyEasyMesh>(GetChronoDataFile("models/cube.obj"), 100.0, mat, 0.05, csys_type);
    mesh->SetPos(ChVector<>(2.0, 3.5, -2.0));
    mesh->AddAsset(chrono_types::make_shared<ChColorAsset>(0.2f, 0.3f, 0.6f));
    sys.AddBody(mesh);

    // Create the Irrlicht visualization system
    ChIrrApp application(&sys, L"Collision visualization demo", irr::core::dimension2d<irr::u32>(800, 600));
    application.AddLogo();
    application.AddSkyBox();
    application.AddTypicalLights();
    application.AddCamera(irr::core::vector3df(0, 8, 6));

    application.AssetBindAll();
    application.AssetUpdateAll();

    // Set the debug drawer for collision visualization
    auto drawer = chrono_types::make_shared<DebugDrawer>(&application);
    sys.GetCollisionSystem()->RegisterVisualizationCallback(drawer);

    // Specify what information is visualized
    int mode = ChCollisionSystem::VIS_Shapes | ChCollisionSystem::VIS_Aabb;
    //////int mode = ChCollisionSystem::VIS_Shapes;
    ////int mode = ChCollisionSystem::VIS_Contacts;

    bool use_zbuffer = true;

    // Simulation loop
    application.SetTimestep(0.005);
    application.SetTryRealtime(true);
    while (application.GetDevice()->run()) {
        application.BeginScene(true, true, irr::video::SColor(255, 140, 161, 192));
        application.DrawAll();
        application.DoStep();
        drawer->Draw(mode, use_zbuffer);
        application.EndScene();
    }

    return 0;
}
