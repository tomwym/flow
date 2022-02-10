#pragma once

#include <iostream>
#include <fstream>
#include <array>
#include <map>
#include <string>
#include <iterator>
#include <algorithm>
#include <utility>
#include "E57Foundation.h"

class E57Handle {
public:
    E57Handle();
    E57Handle(const std::string&);
    ~E57Handle();
    void SetFileName(const std::string&);
    void SetAll();
    void PrintData();

    std::vector<std::vector<float>>&& MoveData();
    std::vector<float>&& MoveLinearData();
    const std::vector<float>& HandleLinearData() const;
    std::pair<int64_t, int64_t> GetDataDims() const;
protected:
private:
    // disallow copy construction and copy assignment
    // we only want one handle object
    E57Handle(const E57Handle& other) {}
    void operator=(const E57Handle& other) {}

    void SetRoot();
    // Set {Points, Prototyype, Protoname} to instantiate dynamic objs on heap
    void SetPoints();
    void SetPrototype();
    void SetProtoname();
    // Read all of the point data into data
    void SetData();
    void SetIntermediateFile();

    const std::string DEFAULT_NAME = "./dat/bunnyDouble";
    std::string m_filename;
    std::unique_ptr<e57::ImageFile> m_imf;
    // number of points (number of prototype structs)
    int64_t m_M;
    // number of objects in the prototype struct
    int64_t m_N;
    // the reason these are managed by unique_ptr is because they have no default ctor (private default ctor)
    // stack objects members will throw compilation error
    std::unique_ptr<e57::StructureNode>         m_root;
    std::unique_ptr<e57::CompressedVectorNode>  m_points;
    std::unique_ptr<e57::StructureNode>         m_prototype;

    std::vector<std::string>        m_protoname;
    std::vector<std::vector<float>> m_data;
    std::vector<float>              m_data_linear;
};

std::ostream& operator<<(std::ostream& out, const e57::NodeType& value);
