project(common LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
add_library(Player::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} INTERFACE 
	Mortis::Utils
	CURL::libcurl
	SDL2::SDL2
	ffmpeg
)

target_include_directories(${PROJECT_NAME} INTERFACE 
	${CMAKE_CURRENT_SOURCE_DIR}
)


