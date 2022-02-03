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

int main() {
    std::cout << "hello flow!" << std::endl;

    std::vector<glm::vec3> asdf({glm::vec3(-0.05,  0.05, 0),
                                 glm::vec3(-0.05, -0.05, 0),
                                 glm::vec3( 0.05,  0.05, 0),
                                 glm::vec3( 0.05, -0.05, 0)});
    float theta = 0;                                 
    std::vector<glm::vec3> fdsa(makeTriangle<void>(Display::m_width, Display::m_height, theta, 200));

    E57Handle e57obj;
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
    /*
    Eigen::Vector3f T(0,0,-0.1);
    float r0 = 0.5*M_PI;
    float r1 = 0;
    float r2 = 0.5*M_PI;

    Eigen::Matrix3f R0 ({{1, 0, 0},
                         {0, cos(r0), -sin(r0)},
                         {0, sin(r0), cos(r0)}});
    Eigen::Matrix3f R1 ({{cos(r1), 0, sin(r1)},
                         {0, 1, 0},
                         {-sin(r1), 0, cos(r1)}});
    Eigen::Matrix3f R2 ({{cos(r2), -sin(r2), 0},
                         {sin(r2), cos(r2), 0},
                         {0, 0, 1}});
    Eigen::Matrix3f R = 2 * R1 * R1;

    Eigen::Matrix4f RT = Eigen::Matrix4f::Identity();
    RT.block<3,3>(0,0) = R;
    RT.block<3,1>(0,3) = T;
    std::cout << RT << '\n';
    A = A * RT.transpose();
    A = A.block(0, 0, A.rows(), 3);
    // transpose and 

    Eigen::MatrixXf B = A.transpose() * A;
    Eigen::JacobiSVD<Eigen::MatrixXf> svdobj(B, Eigen::ComputeFullU | Eigen::ComputeFullV);
    Eigen::MatrixXf C = 5 * A * svdobj.matrixU()(Eigen::all, {1,2});
    std::cout << C.rows() << ' ' << C.cols() << '\n';
    */
    std::vector<glm::vec3> temp(convertPoints<void>(geom.GetNormalizedData()));
    
    Window wind("flow");
    Mesh mesh(temp, fdsa);
    Shader shader("./res/basicShader");
    Transform transform;
    Camera camera(glm::vec3(0,0,-0.2), 70.0f, 0.01f, 1000.0f);
    float counter = 0.f;
    // State st("state0", '0');
    // State* here = &st;

    State3 st3("state3", '3');
    State2 st2("state2", '2', &st3);
    State1 st1("state1", '1', &st2);
    State0 st0("state0", '0', &st1);
    State* here = &st0;
    // static interface initializations
    State::setGeom(&geom);
    State::setMesh(&mesh);
    State::setMasterkey({{'0', &st0},
                         {'1', &st1},
                         {'2', &st2},
                         {'3', &st3}});

	while(!wind.IsClosed()) {
		wind.ClearScreen(0.0f, 0.2f, 0.4f, 1.0f);
        // transform.GetPos().x = sinf(counter);
        // transform.GetRot().z = counter;
        shader.Bind();
        fdsa = makeTriangle<void>(Display::m_width, Display::m_height, theta, 800);
        transform.UpdateRotation<Eigen::Vector3f>(geom.m_rotation_vals);
        shader.Update(transform, camera);
        mesh.Draw(fdsa, 2, 12);
        wind.Update();
        here = here->Spin(wind);
        wind.LimitFrames();
        theta += M_PI/60;
	}    
    return 0;

}