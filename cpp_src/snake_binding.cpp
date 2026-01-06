#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cstring> 
#include <mutex>
#include "Game.h"
#include "StructsAndConstants.h"
#include "Snake.h"
#include "SnakeBatch.h"

namespace py = pybind11;

PYBIND11_MODULE(snake_module, m) {
    py::class_<stepResult>(m, "stepResult")
        .def(py::init<>())
        .def_readwrite("reward", &stepResult::reward)
        .def_readwrite("done", &stepResult::done)
        .def_readwrite("won", &stepResult::won)
        .def_readwrite("foodEaten", &stepResult::foodEaten)
        .def_readwrite("snakeLen", &stepResult::snakeLen)
        .def_readwrite("distFoodX", &stepResult::distFoodX)
        .def_readwrite("distFoodY", &stepResult::distFoodY)
        .def_readwrite("headX_norm", &stepResult::headX_norm)
        .def_readwrite("headY_norm", &stepResult::headY_norm)
        .def_readwrite("distN", &stepResult::distN)
        .def_readwrite("distS", &stepResult::distS)
        .def_readwrite("distE", &stepResult::distE)
        .def_readwrite("distW", &stepResult::distW)
        .def_readwrite("distNW", &stepResult::distNW)
        .def_readwrite("distNE", &stepResult::distNE)
        .def_readwrite("distSW", &stepResult::distSW)
        .def_readwrite("distSE", &stepResult::distSE)
        .def_readwrite("isUp", &stepResult::isUp)
        .def_readwrite("isDown", &stepResult::isDown)
        .def_readwrite("isLeft", &stepResult::isLeft)
        .def_readwrite("isRight", &stepResult::isRight)
        .def_readwrite("accessibleSpace", &stepResult::accessibleSpace)
        .def_readwrite("accessibleSpaceN",&stepResult::accessibleSpaceN)
        .def_readwrite("accessibleSpaceS",&stepResult::accessibleSpaceS)
        .def_readwrite("accessibleSpaceE",&stepResult::accessibleSpaceE)
        .def_readwrite("accessibleSpaceW",&stepResult::accessibleSpaceW)
        .def_readwrite("diffX",&stepResult::diffX)
        .def_readwrite("diffY",&stepResult::diffy)
        .def_readwrite("timePressure",&stepResult::timePressure)
        .def_readwrite("fillPercentage", &stepResult::fillPercentage);

    py::class_<Game>(m, "Game")
        .def(py::init<int, int, int, int, int>(), 
             py::arg("gridRows"), py::arg("gridCols"), 
             py::arg("startX"), py::arg("startY"), 
             py::arg("initialLength"))
        .def("step", &Game::step)
        .def("InitilizeGrid", &Game::initilizeGrid)
        .def("render", &Game::render)
        .def("getGrid", &Game::getGrid);


    py::class_<SnakeBatch>(m, "SnakeBatch")
        .def(py::init<int, int, int, int, int, int>()) // בנאי
        .def("reset_all", &SnakeBatch::reset_all)       // איפוס
        .def("step_all", &SnakeBatch::step_all)         // הצעד המהיר
        .def("get_agent", &SnakeBatch::get_agent, py::return_value_policy::reference); // גישה לנחש בודד
}


//cd /d d:\snake_ai\cpp_src
//cl /O2 /LD /EHsc /std:c++17 /I"D:\snake_python_lib\Python312\include" /I"D:\snake_python_lib\Python312\Lib\site-packages\pybind11\include" snake_binding.cpp snake.cpp game.cpp grid.cpp SnakeBatch.cpp /link /LIBPATH:"D:\snake_python_lib\Python312\libs" python312.lib /OUT:snake_module.pyd
//compiled with visual studio cl without it the moudle will not work