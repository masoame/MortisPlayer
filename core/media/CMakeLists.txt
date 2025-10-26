project(media LANGUAGES CXX)

file(GLOB SRC_FILES "src/*.cpp")
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

target_link_libraries(${PROJECT_NAME} PUBLIC
	Player::common
)
