#define EIGEN_NO_DEBUG

#include <iostream>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "Eigen/Geometry"

#include "window.hpp"
#include "mesh.hpp"
#include "e57handle.hpp"
#include "geometry.hpp"
#include "shader.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "state.hpp"
#include "functions.hpp"

int main(int argc, char** argv) {
    std::cout << "hello flow!" << std::endl;

    float theta = 0;
    std::vector<glm::vec3> fdsa(makeTriangle<void>(Display::m_width, Display::m_height, theta, 200));

    E57Handle e57obj;
    if (argc > 1) {
        if (argv[1] == "-f" && argc > 2) {
            e57obj.SetFileName(argv[2]);
        }
    }
    e57obj.SetAll();
    std::vector<std::vector<float>> data(e57obj.MoveData());

    Geometry geom(&e57obj);
    geom.CollectRawData();
    if (argc > 1) {
        geom.CollectObjData(argv[2]);
    }
    geom.CollectNormalizedData();

    std::vector<glm::vec3> temp(convertPoints<void>(geom.GetNormalizedPxData()));

    Window wind("flow");
    Mesh mesh(temp, fdsa);
    Shader shader("./res/basicShader");
    Transform transform;
    Camera camera(glm::vec3(0,0,8), 0.15f, 0.01f, 100.f);
    float counter = 0.f;

    State3 st3("state3", '3');
    State2 st2("state2", '2', &st3);
    State1 st1("state1", '1', &st2);
    State0 st0("state0", '0', &st1);
    State* here = &st0;
    // static interface initializations
    State::setGeom(&geom);
    State::setWind(&wind);
    State::setMesh(&mesh);
    State::setTransform(&transform);
    State::setMasterkey({{'0', &st0},
                         {'1', &st1},
                         {'2', &st2},
                         {'3', &st3}});

    mesh.SetDrawDynamicPrimitive(GL_TRIANGLES);

	while(!wind.IsClosed()) {
		wind.ClearScreen("light blue");
        // transform.GetPos().x = sinf(counter);
        // transform.GetRot().z = counter;
        shader.Bind();
        fdsa = makeTriangle<void>(Display::m_width, Display::m_height, theta, 800);
        // transform.UpdateRotation<Eigen::Vector3f>(geom.m_rotation_vals);
        shader.Update(transform, camera);
        here = here->Spin(wind);
        // mesh.UpdateDynamic(fdsa);
        mesh.Draw();
        wind.Update();

        wind.LimitFrames();
        theta += M_PI/60;
	}
    return 0;

}