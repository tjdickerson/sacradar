cmake_minimum_required(VERSION 3.19)
project(sacradar)

set(sacradar_src_dir src/)
set(externals_dir ${sacradar_src_dir}external/)
set(imgui_dir ${externals_dir}imgui/)
set(glsl_shader_dir ${sacradar_src_dir}shaders/glsl)
set(resources_dir resources/)

file(COPY ${resources_dir} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(COPY ${glsl_shader_dir} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders/glsl)

include_directories(${sacradar_src_dir})

set(sacradar_src_files

#        ${sacradar_src_dir}main.cpp

        ${sacradar_src_dir}windows/win_main.cpp
        ${sacradar_src_dir}windows/win_support.h
        ${sacradar_src_dir}windows/win_opengl.cpp
        ${sacradar_src_dir}windows/win_opengl.h

        ${sacradar_src_dir}tjd_share.h

        ${externals_dir}glad/glad.h
        ${externals_dir}glad.c
        ${externals_dir}KHR/khrplatform.h

        ${imgui_dir}imgui.cpp
        ${imgui_dir}imgui_demo.cpp
        ${imgui_dir}imgui_draw.cpp
        ${imgui_dir}imgui_impl_opengl3.cpp
        ${imgui_dir}imgui_impl_win32.cpp
        ${imgui_dir}imgui_tables.cpp
        ${imgui_dir}imgui_widgets.cpp

        ${imgui_dir}imconfig.h
        ${imgui_dir}imgui.h
        ${imgui_dir}imgui_internal.h
        ${imgui_dir}imstb_rectpack.h
        ${imgui_dir}imstb_textedit.h
        ${imgui_dir}imstb_truetype.h
        ${imgui_dir}imgui_impl_opengl3.h
        ${imgui_dir}imgui_impl_win32.h

        ${sacradar_src_dir}core/SacRadar.cpp
        ${sacradar_src_dir}core/SacRadar.h
        ${sacradar_src_dir}core/mapping/shapefile/shapefile.h
        ${sacradar_src_dir}core/mapping/shapefile/shapefile.cpp
        ${sacradar_src_dir}core/mapping/SacMap.cpp
        ${sacradar_src_dir}core/mapping/SacMap.h
        ${sacradar_src_dir}core/mapping/rendering/gl/gl_shapefile_rendering.cpp
        ${sacradar_src_dir}core/mapping/rendering/gl/gl_shapefile_rendering.h
        ${sacradar_src_dir}core/mapping/rendering/gl/render_info.h
        src/core/mapping/rendering/gl/gl_sacmap_render.cpp
        src/core/mapping/rendering/gl/gl_sacmap_render.h
        src/core/mapping/rendering/gl/gl_init.cpp
        src/core/mapping/rendering/gl/gl_init.h
)

add_executable(sacradar

        WIN32

        ${sacradar_src_files}
)

#target_link_libraries(${PROJECT_NAME})
