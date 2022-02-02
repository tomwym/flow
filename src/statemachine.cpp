#include <vector>
#include <iostream>
#include <string>
#include <map>

#include "state.hpp"
#include "display.hpp"
#include "window.hpp"

int statemachine() {

    State3 st3("state3", '3');
    State2 st2("state2", '2', &st3);
    State1 st1("state1", '1', &st2);
    State0 st0("state0", '0', &st1);
    State* here = &st0;

    State::masterkey = {{'0', &st0},
                        {'1', &st1},
                        {'2', &st2},
                        {'3', &st3}};

    Window wind("statemachine");
    while(!wind.IsClosed()) {
		wind.ClearScreen(0.0f, 0.2f, 0.4f, 1.0f);
        //wind.Update();
        wind.UpdateTransitions();
        here = here->Spin(wind);
        wind.LimitFrames();
	}

}