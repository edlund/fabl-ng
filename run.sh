
#!/bin/bash

script_path=`readlink -f "$0"`
script_dir=`dirname "${script_path}"`

normal=1

bin="./fabl-ng.bin"
build="Release/"
debug=""
valgrind=""

options="db:v:"
while getopts $options option
do
	case $option in
		d ) debug="gdb --args" ;;
		b ) build=$OPTARG ;;
		v ) valgrind=$OPTARG ;;
	esac
done

if [[ ! ( \
	"${valgrind}" == "" || \
	"${valgrind}" == "memcheck" || \
	"${valgrind}" == "cachegrind" || \
	"${valgrind}" == "callgrind" \
) || ! ( -d "${build}" && -f "${build}/${bin}" ) ]] ; then
	cat <<EOF
Usage: `basename $0` [-db:v:]
Run fabl-ng.
	-d         debug the build
	-b <str>   build to use
	-v <str>   valgrind tool to use
EOF
	exit 1
fi

export LD_LIBRARY_PATH="${script_dir}/vendor/builds/boost-1.58/lib-linux-x86-64:${LD_LIBRARY_PATH}"

cd "${build}"

if [[ "${debug}" != "" ]] ; then
	normal=0
	eval "${debug} ${bin}"
fi

if [[ "${valgrind}" != "" ]] ; then
	normal=0
	valgrind "--tool=${valgrind}" "${bin}"
fi

if [ $normal -eq 1 ] ; then
	"${bin}"
fi

exit 0

