target_sources(flow
    PUBLIC
        mesh.cpp
        shader.cpp
        window.cpp       
)

target_include_directories(flow PUBLIC ${CMAKE_CURRENT_LIST_DIR})