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

std::vector<glm::vec3> makeTriangle(float width, float height, const float theta, const float r) {
    constexpr static float dtheta = M_PI/9.0;
    float rx = 2*r/(float)width;
    float ry = 2*r/(float)height;
    std::vector<glm::vec3> out({glm::vec3(0,0,0)});
    out.push_back(glm::vec3(rx*std::cos(theta), ry*std::sin(theta), 0));
    out.push_back(glm::vec3(rx*std::cos(theta+dtheta), ry*std::sin(theta+dtheta), 0));
    return out;
}

std::vector<glm::vec3> makecircle(float width, float height, float r, float x, float y) {
    float normalized_x = 2*x/width;
    float normalized_y = 2*y/height;
    float rx = 2*r/width;
    float ry = 2*r/height;
    // std::cout << rx << ' ' << ry << '\n';
    // std::cout << normalized_x << ' ' << normalized_y << '\n';

    std::vector<glm::vec3> out({{normalized_x,normalized_y,0}});
    for (float i=0; i<=2*M_PI; i+= 2*M_PI/10) {
        out.push_back(glm::vec3(rx*std::cos(i) + normalized_x, ry*std::sin(i) + normalized_y, 0));
    }

    return out;
}

std::vector<glm::vec3> convertPoints(const Eigen::MatrixXf& X) {
    std::vector<glm::vec3> out;
    out.reserve(X.rows());
    for (const auto& row : X.rowwise()) {
        out.push_back(glm::vec3(row[0], row[1], row[2]));
    }
    std::cout << out.size() << '\n';
    return out;
}

int main() {
    std::cout << "hello flow!" << std::endl;

    std::vector<glm::vec3> asdf({glm::vec3(-0.05,  0.05, 0),
                                 glm::vec3(-0.05, -0.05, 0),
                                 glm::vec3( 0.05,  0.05, 0),
                                 glm::vec3( 0.05, -0.05, 0)});
    float theta = 0;                                 
    std::vector<glm::vec3> fdsa(makeTriangle(Display::m_width, Display::m_height, theta, 200));

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
    std::vector<glm::vec3> temp(convertPoints(geom.GetNormalizedData()));
    
    Window wind("flow");
    Mesh mesh(temp, fdsa);
    Shader shader("./res/basicShader");
    Transform transform;
    Camera camera(glm::vec3(0,0,-0.2), 70.0f, 0.01f, 1000.0f);
    float counter = 0.f;
    // State st("state0", '0');
    // State* here = &st;

	while(!wind.IsClosed()) {
		wind.ClearScreen(0.0f, 0.2f, 0.4f, 1.0f);
        // transform.GetPos().x = sinf(counter);
        // transform.GetRot().z = counter;
        shader.Bind();
        fdsa = makeTriangle(Display::m_width, Display::m_height, theta, 800);

        shader.Update(transform, camera);
        mesh.Draw(fdsa, 2, 12);
        wind.UpdateTransitions();
        wind.LimitFrames();
        theta += M_PI/60;
        counter += 0.01;
	}    
    return 0;

}