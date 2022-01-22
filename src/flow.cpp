#include "display.hpp"
#include "mesh.hpp"
#include "e57handle.hpp"
#include "Eigen/Dense"
#include "Eigen/Geometry"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <vector>
#include <cmath>

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
        out.push_back(glm::vec3(row[0], row[1], 0));
    }
    std::cout << out.size() << '\n';
    return out;
}

int main() {
    std::cout << "hello flow!" << std::endl;

    const int width = 900;
    const int height = 600;
    Display display(width, height, "flow");

    std::vector<glm::vec3> asdf(makecircle(width, height, 50, 100, -150));
    std::vector<glm::vec3> fdsa(makecircle(width, height, 50, 100, -50));
    asdf.insert(asdf.end(), fdsa.begin(), fdsa.end());

    E57Handle e57obj;
    e57obj.SetAll();
    //e57obj.PrintData();

    std::vector<std::vector<float>> data(e57obj.MoveData());
    int reduction_factor = 1;
    Eigen::MatrixXf A(data[0].size()/reduction_factor, 4);
    //Eigen::MatrixXf A(data[0].size()/10, 3);
    std::cout << A.rows() << ' ' << A.cols() << '\n';
    for (size_t i = 0; i < data[0].size()/reduction_factor; i++) {
        for (size_t j = 0; j < 3; j++) {
            A(i,j) = data[j][i*reduction_factor];
        }
        A(i,3) = 1;
    }

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
    std::vector<glm::vec3> temp(convertPoints(C));

    Mesh mesh(temp);

	while(!display.IsClosed()) {
		display.ClearScreen(0.0f, 0.2f, 0.4f, 1.0f);
        mesh.Draw(C.rows(), 12);
        //mesh.Draw(2, 12);
        display.Update();
	}    
    return 0;

}