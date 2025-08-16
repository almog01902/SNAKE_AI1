#ifdef _WIN32
#define strdup _strdup
#endif
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cstring> 
#include <mutex>
#include "Game.h"
#include "StructsAndConstants.h"
#include "Snake.h"

namespace py = pybind11;

Game game(GRID_SIZE, GRID_SIZE, GRID_SIZE / 2, GRID_SIZE / 2, INITIAL_SNAKE_LENGTH);

PYBIND11_MODULE(snake_module,m){
    py::class_<stepResult>(m, "stepResult")
        .def(py::init<>())
        .def_readwrite("reward", &stepResult::reward)
        .def_readwrite("done", &stepResult::done)
        .def_readwrite("distFoodX", &stepResult::distFoodX)
        .def_readwrite("distFoodY", &stepResult::distFoodY)
        .def_readwrite("distToDangerForward", &stepResult::distToDangerForward)
        .def_readwrite("distToDangerLeft", &stepResult::distToDangerLeft)
        .def_readwrite("distToDangerRight", &stepResult::distToDangerRight)
        .def_readwrite("direction", &stepResult::direction);

       m.def("step", [](int action) {
        return game.step(action); // Call the step function with the action

    });
}



//g++ -O3 -Wall -shared -std=c++17 -I"D:/snake_python_lib/Python312/include" -I"D:/snake_python_lib/Python312/Lib/site-packages/pybind11/include" cpp_src\snake_binding.cpp cpp_src\snake.cpp cpp_src\game.cpp cpp_src\grid.cpp -L"D:/snake_python_lib/Python312/libs" -lpython312 -o snake_module.pyd -static-libgcc -static-libstdc++
// Compile command for Windows with pybind11 and Python 3.12