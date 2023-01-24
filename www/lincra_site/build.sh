#!/bin/sh
clear

PROJECT_NAME=lincra_site
SOURCE_PATH=/home/user/dev/$PROJECT_NAME
CODE_LIB_PATH=/home/user/dev/code_lib

MAIN_PATH=$SOURCE_PATH/main.cpp
ROUTES_PATH=$SOURCE_PATH/routes
BUILD_PATH=$SOURCE_PATH/run_tree

CFLAGS="-g -O3 -mno-stack-arg-probe -maes -lm -pthread"
LDFLAGS=""
PAGES_LDFLAGS=""
# -nostdlib -nostdlib++ 
#"-W1,-subsystem:console"

CINCLUDES="-I $CODE_LIB_PATH/external/stb -I $CODE_LIB_PATH -I $CODE_LIB_PATH/external/unix_mysql/include -I $CODE_LIB_PATH/external/meow_hash"

PAGES_ARG="-pages"
FILE_ARG="-file"
MAIN_ARG="-main"

if [ ! -d "$BUILD_PATH" ]; then
    mkdir $BUILD_PATH
fi

compile_exe() {
	clang $CFLAGS $LDFLAGS $CINCLUDES $MAIN_PATH -o debug_clang /usr/lib/x86_64-linux-gnu/libmysqlclient.so -lc
}
compile_dll() {
	clang $CFLAGS $PAGES_LDFLAGS -shared -v -fPIC $CINCLUDES $1 -o $(basename -s .cpp $1).so
}
compile_pages() {
	ROUTES_PATH_DIRECTORY=$ROUTES_PATH/*
	for route_dir in $ROUTES_PATH_DIRECTORY
	do
		route_name=$(basename $route_dir)
		PAGES_LIB_PATH=$BUILD_PATH/$route_name/pages_lib

		if [ ! -d "$PAGES_LIB_PATH" ]; then
			mkdir $PAGES_LIB_PATH
		fi
		pushd $PAGES_LIB_PATH
		
		SOURCE_PAGES_DIRECTORY=$ROUTES_PATH/$route_name/pages
		FOR_PAGES_DIRECTORY=$SOURCE_PAGES_DIRECTORY/*.cpp
		for file in $FOR_PAGES_DIRECTORY
		do
			compile_dll $file
		done
		popd
	done
}

pushd $BUILD_PATH
if [ $# == 0 ]; then
    compile_exe
	compile_pages
else
    if [ "$1" = "$PAGES_ARG" ]; then
		compile_pages
    elif [ "$1" = "$FILE_ARG" ]; then
		if [ $# -gt 1 ]; then
			SOURCE_FILE_PATH="$SOURCE_PATH/$2"
			SOURCE_PAGES_FILE_PATH="$SOURCE_PAGES_DIRECTORY/$2"
			
			if [ -f "$SOURCE_FILE_PATH" ]; then
				compile_exe
			elif [ -f "$SOURCE_PAGES_FILE_PATH" ]; then
				compile_dll $SOURCE_PAGES_FILE_PATH
			fi
		else
			echo "Missing file name."
		fi
    elif [ "$1" = "$MAIN_ARG" ]; then
		compile_exe
    fi
fi
popd
