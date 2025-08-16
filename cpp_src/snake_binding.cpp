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
    m.def("toggle_render", []() {
        game.toggleRender(); // Toggle rendering on or off
    });
}



//cl /O2 /LD /EHsc /I"D:\snake_python_lib\Python312\include" /I"D:\snake_python_lib\Python312\Lib\site-packages\pybind11\include" snake_binding.cpp snake.cpp game.cpp grid.cpp /link /LIBPATH:"D:\snake_python_lib\Python312\libs" python312.lib /OUT:snake_module.pyd
//compiled with visual studio cl without it the moudle will not work