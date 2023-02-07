/* swipl -s FSM.pl */

State = idle.
Distance = 0.
Name = "".

state(A, B) :- State = A, Name = B.
state(found, "parking found") :- State = found, Name.
state(not_found, "parking not found") :- State = found.
state(idle, "idle")  :- State = found. Distance = 0.
state(first_car, "1st car") :- State = found.
state(hole, "hole") :- State = found.
state(second_car, "2nd car") :- State = found.

start(idle).
final(found).
final(not_found).


transition(idle) :-
transition(first_car, hole, detect) :- state(first_car), state(hole), detect.
transition(hole, second_car, detect) :- .
transition(hole, second_car, Distance > 6.4) :- .
transition(hole, second_car) :- Distance = Distance + Speed * Dt.

transition(second_car, first_car, detect).
transition(second_car, idle, detect).
transition(second_car, found, detect).
