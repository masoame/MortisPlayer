project(gui)


if(Qt6_ROOT STREQUAL "")
    message(STATUS "Qt6_DIR is empty")
else()
    find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)

    set(CMAKE_AUTOUIC_SEARCH_PATHS "${PROJECT_SOURCE_DIR}/ui")

    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    file(GLOB ADD_UI_FILES 
        ${PROJECT_SOURCE_DIR}/src/*.cpp 
        ${PROJECT_SOURCE_DIR}/ui/*.ui 
        ${PROJECT_SOURCE_DIR}/inc/*.hpp
    )

    qt6_add_library(${PROJECT_NAME} SHARED
        MANUAL_FINALIZATION
        ${ADD_UI_FILES}
    )

    file(GLOB IMAGE_ICON_FIELS ${PROJECT_SOURCE_DIR}/res/Image/icon/*.*)

    qt6_add_resources(${PROJECT_NAME} Image
        PREFIX "/Image"
        BASE "${PROJECT_SOURCE_DIR}/res/Image"
        FILES ${IMAGE_ICON_FIELS}
    )

    target_link_libraries(${PROJECT_NAME} PUBLIC
        media
        spider
        Qt6::Core
        Qt6::Widgets
        Qt6::OpenGLWidgets
    )

    target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/inc
        ${PROJECT_SOURCE_DIR}/res
        ${PROJECT_SOURCE_DIR}/ui
        ${CMAKE_BINARY_DIR}/gui/gui_autogen/include
    )


    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${Qt6_ROOT}/bin/windeployqt.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.dll
    )

endif()