#!/opt/homebrew/bin/bash

source ../common/common.sh

[ $# -lt 1 ] && echo "Missing input!" && exit 1

function to_lower() {
    awk '{print tolower($0)}'
}

function replace_text_as_numbers() {
    sed 's/one/o1e/g; s/two/t2/g; s/three/t3e/g; s/four/f4r/g; s/five/f5e/g; s/six/s6x/g; s/seven/s7n/g; s/eight/e8t/g; s/nine/n9e/g'
}

function remove_letters() {
    sed 's#[a-zA-Z]##g'
}

function duplicate_line() {
    sed 's/.*/&&/'
}

function fetch_first_and_last() {
    sed -n 's/\(.\).*\(.\)/\1\2/p'
}

cat $1 | to_lower | replace_text_as_numbers | remove_letters  | duplicate_line | fetch_first_and_last  | sum
