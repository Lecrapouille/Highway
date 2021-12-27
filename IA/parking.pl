% Inspired by
% https://stackoverflow.com/questions/9337074/what-is-the-neatest-prolog-implementation-for-conways-game-of-life

% Parking world
world([ [1,1,0],
        [0,0,0],
	[1,1,4] ]).

% Display the world
display([]) :- nl.
display([H|T]) :-
    write(H), nl,
    display(T).

% Value = transpose(Matrix[Row][Col])
index(Matrix, Col, Row, Value) :-
    nth1(Row, Matrix, MatrixRow),
    nth1(Col, MatrixRow, Value).

% World states
road(World, X, Y) :-
    index(World, X, Y, V), V =:= 0.
empty_parking(World, X, Y) :-
    index(World, X, Y, V), V =:= 1.
occupied_parking(World, X, Y) :-
    index(World, X, Y, V), V =:= 2.
car(World, X, Y) :-
    index(World, X, Y, V), V =:= 3.
ego(World, X, Y) :-
    index(World, X, Y, V), V =:= 4.

% Simulation rules
%rule()
rules(World, NewWorld) :-
%    ego(W, X, Y), rule


% Simulation loop
simulate(World) :-
    display(World),
    rules(World, NewWorld), !,
    get_single_char(_), !,
    simulate(NewWorld).

% To run the simulation:
%  world(W), simulate(W).
