function sum() {
    awk '{sum += $1 } END { print sum}'
}
