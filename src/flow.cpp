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

    std::vector<glm::vec3> asdf({glm::vec3(-0.05,  0.05, 0),
                                 glm::vec3(-0.05, -0.05, 0),
                                 glm::vec3( 0.05,  0.05, 0),
                                 glm::vec3( 0.05, -0.05, 0)});
    float theta = 0;
    std::vector<glm::vec3> fdsa(makeTriangle<void>(Display::m_width, Display::m_height, theta, 200));

    E57Handle e57obj;
    if (argc > 1) {
        std::cout << argc << ' ' << argv[1] << '\n';
        e57obj.SetFileName(argv[1]);
    }
    e57obj.SetAll();
    //e57obj.PrintData();

    std::vector<std::vector<float>> data(e57obj.MoveData());
    Eigen::MatrixXf A(data[0].size(), 4);
    std::cout << A.rows() << ' ' << A.cols() << '\n';
    for (size_t i = 0; i < data[0].size(); i++) {
        for (size_t j = 0; j < 3; j++) {
            A(i,j) = data[j][i];
        }
        A(i,3) = 1;
    }
    A = (5*A.array()).matrix();


    Geometry geom(&e57obj);
    geom.CollectRawData();
    geom.CollectNormalizedData();

    std::vector<glm::vec3> temp(convertPoints<void>(geom.GetNormalizedData()));

    Window wind("flow");
    Mesh mesh(temp, fdsa);
    Shader shader("./res/basicShader");
    Transform transform;
    Camera camera(glm::vec3(0,0,-0.2), 70.f, 0.01f, 1000.f);
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