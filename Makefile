.PHONY : rpc 
rpc:
	cd proto/def && protoc --proto_path=./ * --python_out=../output
ext:
	g++ -std=c++11 -I /usr/include/python3.5 -fpic --shared -o rpc.so Module.cpp Util.cpp Rpc.cpp -I./3rd/msgpack/include/  -I ./3rd/tinyxml2/ -L ./3rd/tinyxml2/ -ltinyxml2
