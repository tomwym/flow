#include "e57handle.hpp"

/**
 * This class interfaces with the E57 library to extract point cloud data
 * It is broken into multiple functions for encapsulation of concepts
 */

E57Handle::E57Handle()
    : m_filename(""), m_M(0), m_N(0), m_protoname({}), m_data({{}}) {
    std::cout << "default constructor!" << '\n';
}

E57Handle::E57Handle(const std::string& _filename)
    : m_filename(_filename), m_M(0), m_N(0), m_protoname({}), m_data({{}}) {
    std::cout << "filename constructor!" << '\n';
}

E57Handle::~E57Handle() {
    m_imf->close();
}

// Set the m_filename from a 
void E57Handle::SetFileName(const std::string& _filename) {
    std::cout << "SetFileName" << '\n';
    m_filename = _filename;
    return;
}

// Set the root object as StructureNode
void E57Handle::SetRoot() {
    std::cout << "SetRoot" << '\n';

    // initialize the image file to open
    if (m_filename != "") {
        m_imf = std::make_unique<e57::ImageFile>(m_filename+".e57", "r");
    } else {
        return;
    }

    // ctor StructureNode::StructureNode(const Node& n)
    m_root = std::make_unique<e57::StructureNode>(m_imf->root());
    return;
}

// Set the points object as CompressedVectorNode
void E57Handle::SetPoints() {
    /*
     * m_imf
     * ├── root (Structure)
     *     ├── images2D [0, 1, ...] (Vector)
     *     ├── data3D [0, 1, ...] (Vector)
     *         ├── data3D (CompressedVector)
     */

    std::cout << "SetPoints" << '\n';
    // get data3D child from root
    e57::VectorNode data3D(m_root->get("data3D"));
    // assuming data3D vector only has one object in vector (take first index)
    // vectorChild points to the first instance in data3D
    e57::StructureNode vectorChild(data3D.get("0"));
    // get points from the instance of data3D
    m_points = std::make_unique<e57::CompressedVectorNode>(vectorChild.get("points"));
    return;
}

// the prototype node defines the structure of the data in a CompressedVectorNode
void E57Handle::SetPrototype() {
    std::cout << "SetPrototype" << '\n';
    // m_M returns the number of elements recorded in points
    // == count of number of 3d points
    // roughly: sizeof(points) == m_M * sizeof(typename Prototype)
    m_M = m_points->childCount();
    if (m_points->prototype().type() != e57::E57_STRUCTURE) {
        return;
    }

    m_prototype = std::make_unique<e57::StructureNode>(
                    static_cast<e57::StructureNode>(m_points->prototype())
                );
    // m_N gives the number of categories to record in the points struct
    // the number of primitive fields within the prototype struct
    m_N = m_prototype->childCount();
    return;
}

// name the primitive fields in the prototype struct
void E57Handle::SetProtoname() {
    std::cout << "SetProtoname" << '\n';
    m_protoname.reserve(m_N);
    for(int i=0; i<m_N; i++) {
        // .type() returns enumerator of NodeType
        std::cout << m_prototype->get(i).type() << '\n';
        m_protoname.push_back(m_prototype->get(i).elementName());
    }
    return;
}


void E57Handle::SetData() {
    std::cout << "SetData" << '\n';
    // preallocate data
    m_data = std::vector<std::vector<float>>(m_N, std::vector<float>(m_M, 0));
    m_data_linear.reserve(m_N*m_M);
    for (int i = 0; i < m_N; i++) {
        // get reference for node in prototype
        e57::Node n = m_prototype->get(i);
        std::cout << "reading " << n.pathName() << ":" << std::endl;

        try {
            // standard way of extracting points from binary, using *Reader
            // create empty bufffer than can hold data, here assumed floats
            std::vector<float> rawValue(m_M);

            // not exactly sure what's going on here, but need a vector of
            // SourceDestBuffer object, and cannot be empty
            std::vector<e57::SourceDestBuffer> vdestbufs;
            e57::SourceDestBuffer srcDestBuff(*m_imf.get(), n.pathName(), rawValue.data(),
                                              m_M, true, false);
            vdestbufs.push_back(srcDestBuff);

            // Set up reader object, reader.read() actually reads the object
            e57::CompressedVectorReader reader = m_points->reader(vdestbufs);
            // operation that reads in the data, returns # of objects read
            if (reader.read() != m_M) {
                std::cerr << "**** Elements read not equal to expected number m_M" << std::endl;
                return;
            }

            std::cout << " Read " << m_M << " records from "
                      << reader.compressedVectorNode().pathName() << '\n';

            // this doesn't work for some reason... :p
            //tracking[i].assign(std::begin(rawValue), std::end(rawValue));
            m_data[i].assign(rawValue.begin(), rawValue.end());
            m_data_linear.insert(m_data_linear.end(), rawValue.begin(), rawValue.end());
            reader.close();

        } catch(e57::E57Exception& ex) {
            ex.report(__FILE__, __LINE__, __FUNCTION__);
            std::cerr << "**** Reading " << n.pathName() << " failed" << std::endl;
        }
    }
    return;
}

void E57Handle::SetIntermediateFile() {
    std::ofstream file;
    file.open(m_filename+".dat");
    for (int i = 0; i < m_N; i++) {
        file << m_protoname[i] << ',';
    }
    file << '\n';

    for (unsigned j = 0; j < m_M; j++) {
        for (int i = 0; i < m_N; i++) {
            file << m_data[i][j] << ',';
        }
        file << '\n';
    }
    file.close();
    return;
}


// wrapper for all Set functions
void E57Handle::SetAll() {
    std::cout << "SetAll" << '\n';
    SetFileName(DEFAULT_NAME);
    SetRoot();
    SetPoints();
    SetPrototype();
    SetProtoname();
    SetData();
    SetIntermediateFile();
}

std::vector<std::vector<float>>&& E57Handle::MoveData() {
    return std::move(m_data);
}

std::vector<float>&& E57Handle::MoveLinearData() {
    return std::move(m_data_linear);
}

const std::vector<float>& E57Handle::HandleLinearData() const {
    return m_data_linear;
}

std::pair<int64_t, int64_t> E57Handle::GetDataDims() const {
    return std::make_pair(m_M, m_N);
}

void E57Handle::PrintData() {
    for (unsigned j = 0; j < m_M; j++) {
        for (int i = 0; i < m_N; i++) {
            std::cout << m_data[i][j] << ' ';
        }
        std::cout << '\n';
    }
    return;
}

std::ostream& operator<<(std::ostream& out, const e57::NodeType& value) {
    static std::map<e57::NodeType, std::string> strings;
    if (strings.size() == 0){
        #define INSERT_ELEMENT(p) strings[p] = #p
        INSERT_ELEMENT(e57::E57_STRUCTURE        );
        INSERT_ELEMENT(e57::E57_VECTOR           );
        INSERT_ELEMENT(e57::E57_COMPRESSED_VECTOR);
        INSERT_ELEMENT(e57::E57_INTEGER          );
        INSERT_ELEMENT(e57::E57_SCALED_INTEGER   );
        INSERT_ELEMENT(e57::E57_FLOAT            );
        INSERT_ELEMENT(e57::E57_STRING           );
        INSERT_ELEMENT(e57::E57_BLOB             );
        #undef INSERT_ELEMENT
    }
    return out << strings[value];
}
