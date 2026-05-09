cmake_minimum_required(VERSION 3.16)

project(Screen_Pest VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(LayerShellQt REQUIRED)

add_definitions(-DRESOURCE_DIR="${CMAKE_SOURCE_DIR}")

qt_add_executable(Screen_Pest
    src/main.cpp
    src/ManagerWindow/MainWindow.h src/ManagerWindow/MainWindow.cpp
    src/SubWindow/BehaviourData.h src/SubWindow/BehaviourData.cpp
    src/SubWindow/SpriteAtlas.h src/SubWindow/SpriteAtlas.cpp
    src/SubWindow/AnimationEngine.h src/SubWindow/AnimationEngine.cpp
    src/SubWindow/ActionDispatcher.h src/SubWindow/ActionDispatcher.cpp
    src/SubWindow/SpriteWindow.h src/SubWindow/SpriteWindow.cpp
)

target_link_libraries(Screen_Pest PRIVATE
    Qt6::Widgets
    LayerShellQt::Interface
)

install(TARGETS Screen_Pest
    RUNTIME DESTINATION bin
)

install(DIRECTORY assets/
    DESTINATION share/ScreenPest/assets
)
