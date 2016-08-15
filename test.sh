prog=$1
shift 1
if [[ -z $LIB ]]
then LIB=./libsfmalloc-debug.so
fi
gdb -ex "set exec-wrapper env LD_PRELOAD=$LIB" --ex run --args  $prog "$@"
