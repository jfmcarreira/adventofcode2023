#!/opt/homebrew/bin/bash

RED_COUNT=12
GREEN_COUNT=13
BLUE_COUNT=14

function max_value() {
    printf "%s\n" "$@" | sort -n | tail -1
}

function process_game() {
    echo ${@} | awk -F ':' '{print $2}' | {
        readarray -d ';' -t sets
        max_red=0
        max_green=0
        max_blue=0
        for set in "${sets[@]}"
        do
            readarray -d ',' -t colors <<< $(echo ${set})
            for c in "${colors[@]}"
            do
                case $c in
                    (*" red"*)
                        count=$(echo ${c} | sed 's/ red//g')
                        max_red=$(max_value $count $max_red)
                        ;;
                    (*" green"*)
                        count=$(echo ${c} | sed 's/ green//g')
                        max_green=$(max_value $count $max_green)
                        ;;
                    (*" blue"*)
                        count=$(echo ${c} | sed 's/ blue//g')
                        max_blue=$(max_value $count $max_blue)
                        ;;
                esac

            done
        done
        [[ ${max_red} -gt ${RED_COUNT} ]] && return 1
        [[ ${max_green} -gt ${GREEN_COUNT} ]] && return 1
        [[ ${max_blue} -gt ${BLUE_COUNT} ]] && return 1
        return 0
    } && echo ${@}
}

function process_all_games() {
    while read line
    do
        process_game $line
    done
}

function get_game_id() {
    awk -F ':' '{print $1}' | sed 's/Game //g'
}


function sum() {
    awk '{sum += $1 } END { print sum}'
}

process_all_games | get_game_id #| sum


# 2727