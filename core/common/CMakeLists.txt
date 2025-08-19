project(common LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
add_library(Player::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} INTERFACE 
	Mortis::Lib
	CURL::libcurl
	SDL2::SDL2
	spdlog::spdlog
	ffmpeg
)

target_include_directories(${PROJECT_NAME} INTERFACE 
	${CMAKE_CURRENT_SOURCE_DIR}
)


