#!/opt/homebrew/bin/bash

function check_solution(){
    day=$1
    pushd $day > /dev/null
    if [[ -f ${day}.sh ]]
    then
        command=./${day}.sh
    elif [[ -f ${day}.cpp ]]
    then
        g++ --std=c++20 -Wall -O3 ${day}.cpp -o ${day}
        command=./${day}
    fi
    [[ $(${command} input | tail -1) == "$2" ]] && echo "Solution for day ${day} correct"
    popd > /dev/null
}

check_solution 01 54578
check_solution 02 56580
check_solution 03 84907174
check_solution 04 11827296