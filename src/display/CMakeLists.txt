target_sources(flow
    PRIVATE
        mesh.cpp
        shader.cpp
        window.cpp       
)

target_include_directories(flow PUBLIC ${CMAKE_CURRENT_LIST_DIR})